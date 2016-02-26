/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cuberenderer.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOffscreenSurface>
#include <QWindow>

//OSVR includes
#include <osvr/ClientKit/ClientKit.h>
#include <osvr/ClientKit/Display.h>
#include "object.h"

CubeRenderer::CubeRenderer(QOffscreenSurface *offscreenSurface)
    : m_offscreenSurface(offscreenSurface),
      m_context(0),
      m_program(0),
      m_vbo(0),
      m_vao(0),
      ctx("com.osvr.example.QtOpenGL"),     //OSVR context
      display(ctx)                          //OSVR display config

{

    //OSVR Checks
    if (!display.valid()) {
        qDebug() << "\nCould not get display config (server probably not "
                    "running or not behaving), exiting."
                 << endl;
        return;
    }

    qDebug() << "Waiting for the display to fully start up, including "
                "receiving initial pose update..."
             << endl;
    while (!display.checkStartup()) {
        ctx.update();
    }
    qDebug() << "OK, display startup status is good!" << endl;
}

CubeRenderer::~CubeRenderer()
{
    // Use a temporary offscreen surface to do the cleanup.
    // There may not be a native window surface available anymore at this stage.
    m_context->makeCurrent(m_offscreenSurface);

    delete m_program;
    delete m_vbo;
    delete m_vao;

    m_context->doneCurrent();
    delete m_context;
}

void CubeRenderer::init(QWindow *w, QOpenGLContext *share)
{
    m_context = new QOpenGLContext;
    m_context->setShareContext(share);
    m_context->setFormat(w->requestedFormat());
    m_context->create();
    if (!m_context->makeCurrent(w))
        return;

    QOpenGLFunctions *f = m_context->functions();
    f->glClearColor(0.0f, 0.1f, 0.25f, 1.0f);
    f->glViewport(0, 0, w->width() * w->devicePixelRatio(), w->height() * w->devicePixelRatio());

    static const char *vertexShaderSource =
        "attribute highp vec4 vertex;\n"
        "attribute lowp vec2 coord;\n"
        "varying lowp vec2 v_coord;\n"
        "uniform highp mat4 matrix;\n"
        "void main() {\n"
        "   v_coord = coord;\n"
        "   gl_Position = matrix * vertex;\n"
        "}\n";
    static const char *fragmentShaderSource =
        "varying lowp vec2 v_coord;\n"
        "uniform sampler2D sampler;\n"
        "void main() {\n"
        "   gl_FragColor = vec4(texture2D(sampler, v_coord).rgb, 1.0);\n"
        "}\n";
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("coord", 1);
    m_program->link();
    m_matrixLoc = m_program->uniformLocation("matrix");
    m_vao = new QOpenGLVertexArrayObject;
    m_vao->create();
    QOpenGLVertexArrayObject::Binder vaoBinder(m_vao);

    m_vbo = new QOpenGLBuffer;
    m_vbo->create();
    m_vbo->bind();

    //const int vertexCount = 36;
    const int vertexCount = 1350;
    m_vbo->allocate(sizeof(GLfloat) * vertexCount * 5);
    m_vbo->write(0, v, sizeof(GLfloat) * vertexCount * 3);
    m_vbo->write(sizeof(GLfloat) * vertexCount * 3, texCoords, sizeof(GLfloat) * vertexCount * 2);
    m_vbo->release();

    if (m_vao->isCreated())
        setupVertexAttribs();
}

void CubeRenderer::resize(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45, w / float(h), 0.01f, 100.0f);
}

void CubeRenderer::setupVertexAttribs()
{
    m_vbo->bind();
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);
    m_context->functions()->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    m_context->functions()->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
                                                  //(const void *)(36 * 3 * sizeof(GLfloat)));
                                                  (const void *)(1350 * 3 * sizeof(GLfloat)));
m_vbo->release();
    m_vbo->release();
}

void CubeRenderer::render(QWindow *w, QOpenGLContext *share, uint texture)
{
    if (!m_context)
        init(w, share);

    if (!m_context->makeCurrent(w))
        return;
    ctx.update();
    QOpenGLFunctions *f = m_context->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// For each viewer, eye combination...
    display.forEachEye([&](osvr::clientkit::Eye eye) { // Insert & to avoid lambda error

        /// Try retrieving the view matrix (based on eye pose) from OSVR
        double viewMat[OSVR_MATRIX_SIZE];
        eye.getViewMatrix(OSVR_MATRIX_COLMAJOR | OSVR_MATRIX_COLVECTORS,
                          viewMat);

        /// NOTE : OSVR's viewMat is equivalent to m_camera in our example
        ///Adapt double array to QMatrix4x4
        m_camera = QMatrix4x4(viewMat[0],viewMat[4],viewMat[8],viewMat[12],
                viewMat[1],viewMat[5],viewMat[9],viewMat[13],
                viewMat[2],viewMat[6],viewMat[10],viewMat[14],
                viewMat[3],viewMat[7],viewMat[11],viewMat[15]);
        //m_tempViewMat= m_camera * m_tempViewMat;

        /// For each display surface seen by the given eye of the given
        /// viewer...
        eye.forEachSurface([&](osvr::clientkit::Surface surface) { //insert ampersand to avoid lambda error
            // The following function is what actually splits the window in two different viewports
            auto viewport = surface.getRelativeViewport();
            glViewport(static_cast<GLint>(viewport.left),
                       static_cast<GLint>(viewport.bottom),
                       static_cast<GLsizei>(viewport.width),
                       static_cast<GLsizei>(viewport.height));

            /// Set the OpenGL projection matrix based on the one we
            /// computed.
            double zNear = 0.1;
            double zFar = 100;
            double projMat[OSVR_MATRIX_SIZE];
            surface.getProjectionMatrix(
                        zNear, zFar, OSVR_MATRIX_COLMAJOR | OSVR_MATRIX_COLVECTORS |
                        OSVR_MATRIX_SIGNEDZ | OSVR_MATRIX_RHINPUT,
                        projMat);

            ///OSVR provides a projection matrix as an array of double, which we turn into a QMatrix4x4
            m_projection = QMatrix4x4(projMat[0],projMat[4],projMat[8],projMat[12],
                    projMat[1],projMat[5],projMat[9],projMat[13],
                    projMat[2],projMat[6],projMat[10],projMat[14],
                    projMat[3],projMat[7],projMat[11],projMat[15]);

            if (texture) {
                f->glBindTexture(GL_TEXTURE_2D, texture);
                f->glFrontFace(GL_CW); // because our cube's vertex data is such
                //f->glEnable(GL_CULL_FACE);
                f->glEnable(GL_DEPTH_TEST);

                m_program->bind();
                QOpenGLVertexArrayObject::Binder vaoBinder(m_vao);
                // If VAOs are not supported, set the vertex attributes every time.
                if (!m_vao->isCreated())
                    setupVertexAttribs();

                //Rotation and translation adjust - shitty model
                QMatrix4x4 m;
                m.rotate(-90, 0, 1, 0);
                m.translate(+2, 0, 0);
                //Global transform matrix
                m_program->setUniformValue(m_matrixLoc, m_projection * m_camera * m);

                // Draw the cube.
                //f->glDrawArrays(GL_TRIANGLES, 0, 36);
                f->glDrawArrays(GL_TRIANGLES, 0, 1350);
            }
        });
    });
    m_context->swapBuffers(w);
}
