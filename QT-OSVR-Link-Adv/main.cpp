#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
#include "window.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  // Set OpenGL Version information
  // Note: This format must be set before show() is called.
  QSurfaceFormat format;
  format.setRenderableType(QSurfaceFormat::OpenGL);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setVersion(3, 3);

  // Set the window up
  Window window;
  window.setFormat(format);


  //Display the desktop properties
  qDebug()<<endl<< "[APP] Screens available:"<<QGuiApplication::screens().size();
  qDebug() << "[APP] Primary screen:" << QGuiApplication::primaryScreen()->name();
  qDebug() << "[APP] Primary screen coordinates : " <<QGuiApplication::primaryScreen()->availableGeometry();
  qDebug() << "[APP] OSVR screen:" <<QGuiApplication::screens().last()->name();
  qDebug() << "[APP] OSVR screen coordinates : " <<QGuiApplication::screens().last()->availableGeometry()<< endl;

  //Move window beyond the primary screen, show the window and apply fullscreen property
  window.setPosition((QGuiApplication::primaryScreen()->geometry().width()+5),5);
  window.show();
  window.setWindowState(Qt::WindowFullScreen);

  return app.exec();
}
