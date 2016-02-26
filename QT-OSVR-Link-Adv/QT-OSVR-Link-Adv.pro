#-------------------------------------------------
#
# Project created by QtCreator 2015-01-11T15:14:10
#
#-------------------------------------------------

QT       += core gui

TARGET = QT-OSVR-Link-Adv
CONFIG   += c++11
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    window.cpp \
    camera3d.cpp \
    input.cpp \
    transform3d.cpp

HEADERS += \
    window.h \
    camera3d.h \
    input.h \
    transform3d.h \
    vertex.h

RESOURCES += \
    resources.qrc

INCLUDEPATH += /usr/include

LIBS += -L$$PWD/../../osvr/lib/ -losvrClientKit -losvrClient -losvrCommon -losvrUtil -ljsoncpp

INCLUDEPATH += $$PWD/../../osvr/include/
INCLUDEPATH += $$PWD/../../json/include/
DEPENDPATH += $$PWD/../../osvr/lib/
