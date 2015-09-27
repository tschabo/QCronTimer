#-------------------------------------------------
#
# Project created by QtCreator 2015-09-27T22:15:56
#
#-------------------------------------------------

QT       -= gui

TARGET = CronTimer
TEMPLATE = lib

DESTDIR = $$PWD/../build

CONFIG += c++11

DEFINES += CRONTIMER_LIBRARY

SOURCES += ccrontimer.cpp \
    ccroncalculator.cpp

HEADERS += ccrontimer.h\
        crontimer_global.h \
    ccroncalculator.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

CONFIG(debug, debug|release) {
    DEFINES += CRONTIMER_DEBUG
}
