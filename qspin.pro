#-------------------------------------------------
#
# Project created by QtCreator 2016-02-10T10:25:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT+= concurrent

TARGET = qspin
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    highlighter.cpp \
    codeeditor.cpp

HEADERS  += mainwindow.h \
    highlighter.h \
    codeeditor.h

FORMS    += mainwindow.ui
