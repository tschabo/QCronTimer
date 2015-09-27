#-------------------------------------------------
#
# Project created by QtCreator 2015-09-27T22:27:28
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = caller
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += c++11

TEMPLATE = app

INCLUDEPATH += $$PWD/../CronTimer

DESTDIR = $$PWD/../build

LIBS += -L$$PWD/../build \
-lCronTimer

SOURCES += main.cpp
