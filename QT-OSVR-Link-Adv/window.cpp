#include "window.h"
#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "vertex.h"
#include "input.h"

// Front Verticies
#define VERTEX_FTR Vertex( QVector3D( 0.5f,  0.5f,  0.5f), QVector3D( 1.0f, 0.0f, 0.0f ) )
#define VERTEX_FTL Vertex( QVector3D(-0.5f,  0.5f,  0.5f), QVector3D( 0.0f, 1.0f, 0.0f ) )
#define VERTEX_FBL Vertex( QVector3D(-0.5f, -0.5f,  0.5f), QVector3D( 0.0f, 0.0f, 1.0f ) )
#define VERTEX_FBR Vertex( QVector3D( 0.5f, -0.5f,  0.5f), QVector3D( 0.0f, 0.0f, 0.0f ) )

// Back Verticies
#define VERTEX_BTR Vertex( QVector3D( 0.5f,  0.5f, -0.5f), QVector3D( 1.0f, 1.0f, 0.0f ) )
#define VERTEX_BTL Vertex( QVector3D(-0.5f,  0.5f, -0.5f), QVector3D( 0.0f, 1.0f, 1.0f ) )
#define VERTEX_BBL Vertex( QVector3D(-0.5f, -0.5f, -0.5f), QVector3D( 1.0f, 0.0f, 1.0f ) )
#define VERTEX_BBR Vertex( QVector3D( 0.5f, -0.5f, -0.5f), QVector3D( 1.0f, 1.0f, 1.0f ) )

// Create a colored cube
static const Vertex sg_vertexes[] = {
    // Face 1 (Front)
    VERTEX_FTR, VERTEX_FTL, VERTEX_FBL,
    VERTEX_FBL, VERTEX_FBR, VERTEX_FTR,
    // Face 2 (Back)
    VERTEX_BBR, VERTEX_BTL, VERTEX_BTR,
    VERTEX_BTL, VERTEX_BBR, VERTEX_BBL,
    // Face 3 (Top)
    VERTEX_FTR, VERTEX_BTR, VERTEX_BTL,
    VERTEX_BTL, VERTEX_FTL, VERTEX_FTR,
    // Face 4 (Bottom)
    VERTEX_FBR, VERTEX_FBL, VERTEX_BBL,
    VERTEX_BBL, VERTEX_BBR, VERTEX_FBR,
    // Face 5 (Left)
    VERTEX_FBL, VERTEX_FTL, VERTEX_BTL,
    VERTEX_FBL, VERTEX_BTL, VERTEX_BBL,
    // Face 6 (Right)
    VERTEX_FTR, VERTEX_FBR, VERTEX_BBR,
    VERTEX_BBR, VERTEX_BTR, VERTEX_FTR
};

#undef VERTEX_BBR
#undef VERTEX_BBL
#undef VERTEX_BTL
#undef VERTEX_BTR

#undef VERTEX_FBR
#undef VERTEX_FBL
#undef VERTEX_FTL
#undef VERTEX_FTR

/*******************************************************************************
 * OpenGL Events
 ******************************************************************************/

Window::Window()
    : ctx("com.osvr.example.QtOpenGL"),
      display(ctx)
{
    m_transform.translate(0.0f, 0.0f, -5.0f);
}

void Window::initializeGL()
{
    // Initialize OpenGL Backend
    initializeOpenGLFunctions();
    connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    printVersionInformation();

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

    // Set global information
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Application-specific initialization
    {
        // Create Shader (Do not release until VAO is created)
        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
        m_program->link();
        m_program->bind();

        // Cache Uniform Locations
        u_modelToWorld = m_program->uniformLocation("modelToWorld");
        u_worldToCamera = m_program->uniformLocation("worldToCamera");
        u_cameraToView = m_program->uniformLocation("cameraToView");

        // Create Buffer (Do not release until VAO is created)
        m_vertex.create();
        m_vertex.bind();
        m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_vertex.allocate(sg_vertexes, sizeof(sg_vertexes));

        // Create Vertex Array Object
        m_object.create();
        m_object.bind();
        m_program->enableAttributeArray(0);
        m_program->enableAttributeArray(1);
        m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
        m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

        // Release (unbind) all
        m_object.release();
        m_vertex.release();
        m_program->release();
    }
}

void Window::resizeGL(int width, int height)
{
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, width / float(height), 0.1f, 1000.0f);
}

void Window::paintGL()
{
    // Clear
    glClear(GL_COLOR_BUFFER_BIT);
    ctx.update();

    /// For each viewer, eye combination...
    display.forEachEye([&](osvr::clientkit::Eye eye) { // Insert & to avoid lambda error

        /// Try retrieving the view matrix (based on eye pose) from OSVR
        double viewMat[OSVR_MATRIX_SIZE];
        eye.getViewMatrix(OSVR_MATRIX_COLMAJOR | OSVR_MATRIX_COLVECTORS,
                          viewMat);

        /// NOTE : OSVR's viewMat is equivalent to m_camera in our example
        ///Adapt double array to QMatrix4x4
        QMatrix4x4 m_tempViewMat = QMatrix4x4(viewMat[0],viewMat[4],viewMat[8],viewMat[12],
                                              viewMat[1],viewMat[5],viewMat[9],viewMat[13],
                                              viewMat[2],viewMat[6],viewMat[10],viewMat[14],
                                              viewMat[3],viewMat[7],viewMat[11],viewMat[15]);
        m_tempViewMat= m_camera.toMatrix()*m_tempViewMat;

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
            QMatrix4x4 m_tempProjMat = QMatrix4x4(projMat[0],projMat[4],projMat[8],projMat[12],
                                                  projMat[1],projMat[5],projMat[9],projMat[13],
                                                  projMat[2],projMat[6],projMat[10],projMat[14],
                                                  projMat[3],projMat[7],projMat[11],projMat[15]);

            // Render using our shader
            m_program->bind();
            //m_program->setUniformValue(u_worldToCamera, m_camera.toMatrix());
            m_program->setUniformValue(u_worldToCamera, m_tempViewMat);
            //m_program->setUniformValue(u_cameraToView, m_projection);
            m_program->setUniformValue(u_cameraToView, m_tempProjMat);
            {
                m_object.bind();
                m_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());
                glDrawArrays(GL_TRIANGLES, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
                m_object.release();
            }
            m_program->release();


        });
    });


}

void Window::teardownGL()
{
    // Actually destroy our OpenGL information
    m_object.destroy();
    m_vertex.destroy();
    delete m_program;
}

void Window::update()
{
    // Update input
    Input::update();

    // Camera Transformation
    if (Input::buttonPressed(Qt::RightButton))
    {
        static const float transSpeed = 0.1f;
        static const float rotSpeed   = 0.1f;

        // Handle rotations
        m_camera.rotate(-rotSpeed * Input::mouseDelta().x(), Camera3D::LocalUp);
        m_camera.rotate(-rotSpeed * Input::mouseDelta().y(), m_camera.right());

        // Handle translations
        QVector3D translation;
        if (Input::keyPressed(Qt::Key_W))
        {

            translation += m_camera.forward();
        }
        if (Input::keyPressed(Qt::Key_S))
        {
            translation -= m_camera.forward();
        }
        if (Input::keyPressed(Qt::Key_A))
        {
            translation -= m_camera.right();
        }
        if (Input::keyPressed(Qt::Key_D))
        {
            translation += m_camera.right();
        }
        if (Input::keyPressed(Qt::Key_Q))
        {
            translation -= m_camera.up();
        }
        if (Input::keyPressed(Qt::Key_E))
        {
            translation += m_camera.up();
        }
        m_camera.translate(transSpeed * translation);
    }

    // Update instance information
    m_transform.rotate(1.0f, QVector3D(0.4f, 0.3f, 0.3f));

    // Schedule a redraw
    QOpenGLWindow::update();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
        event->ignore();
    }
    else
    {
        Input::registerKeyPress(event->key());
    }
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
        event->ignore();
    }
    else
    {
        Input::registerKeyRelease(event->key());
    }
}

void Window::mousePressEvent(QMouseEvent *event)
{
    Input::registerMousePress(event->button());
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    Input::registerMouseRelease(event->button());
}

/*******************************************************************************
 * Private Helpers
 ******************************************************************************/

void Window::printVersionInformation()
{
    QString glType;
    QString glVersion;
    QString glProfile;

    // Get Version Information
    glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

    // Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
    switch (format().profile())
    {
    CASE(NoProfile);
    CASE(CoreProfile);
    CASE(CompatibilityProfile);
    }
#undef CASE

    // qPrintable() will print our QString w/o quotes around it.
    qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}
