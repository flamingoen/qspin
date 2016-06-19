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
    codeeditor.cpp \
    verificationrun.cpp \
    simulationrun.cpp \
    spinrun.cpp \
    syntaxrun.cpp

HEADERS  += mainwindow.h \
    highlighter.h \
    codeeditor.h \
    verificationrun.h \
    simulationrun.h \
    spinrun.h \
    syntaxrun.h

FORMS    += mainwindow.ui

CONFIG   += c++11

#target.path = /usr/local/bin/
#INSTALLS += target

RESOURCES += \
    icons.qrc
