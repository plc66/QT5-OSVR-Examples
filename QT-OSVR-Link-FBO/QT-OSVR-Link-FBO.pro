TEMPLATE = app

QT += quick qml
CONFIG += c++11

SOURCES += main.cpp \
           window_singlethreaded.cpp \
           cuberenderer.cpp \
    window_multithreaded.cpp

HEADERS += window_singlethreaded.h \
           cuberenderer.h \
    object.h \
    window_multithreaded.h

RESOURCES += QT-OSVR-Link-FBO.qrc



INCLUDEPATH += /usr/include

LIBS += -L$$PWD/../../osvr/lib/ -losvrClientKit -losvrClient -losvrCommon -losvrUtil -ljsoncpp

INCLUDEPATH += $$PWD/../../osvr/include/
INCLUDEPATH += $$PWD/../../json/include/
DEPENDPATH += $$PWD/../../osvr/lib/

DISTFILES +=
