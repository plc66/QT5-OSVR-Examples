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

#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
#include "window_singlethreaded.h"
#include "window_multithreaded.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QScopedPointer<QWindow> window;
    qWarning("Using single-threaded rendering");
    //window.reset(new WindowSingleThreaded);
    window.reset(new WindowMultiThreaded);


    //Display the desktop properties
    qDebug()<<endl<< "[APP] Screens available:"<<QGuiApplication::screens().size();
    qDebug() << "[APP] Primary screen:" << QGuiApplication::primaryScreen()->name();
    qDebug() << "[APP] Primary screen coordinates : " <<QGuiApplication::primaryScreen()->availableGeometry();
    qDebug() << "[APP] OSVR screen:" <<QGuiApplication::screens().last()->name();
    qDebug() << "[APP] OSVR screen coordinates : " <<QGuiApplication::screens().last()->availableGeometry()<< endl;

    //Move window beyond the primary screen, show the window and apply fullscreen property
    window->setPosition((QGuiApplication::primaryScreen()->geometry().width()+5),5);
    window->resize(1920, 1080); //app specific resize function - refer to window_singlethreaded.cpp
    window->show();
    window->setWindowState(Qt::WindowFullScreen);

    return app.exec();
}