#-------------------------------------------------
#
# Project created by QtCreator 2015-07-09T21:40:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = work_task
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    statgetter.cpp \
    progressworker.cpp \
    controller.cpp \
    dirtreebuilder.cpp \
    simplefsmodel.cpp
HEADERS  += mainwindow.h \
    statgetter.h \
    progressworker.h \
    controller.h \
    dirtreebuilder.h \
    simplefsmodel.h

FORMS    += mainwindow.ui
