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
    syntaxrun.cpp \
    parser.cpp \
    variable.cpp \
    settingswindow.cpp

HEADERS  += mainwindow.h \
    highlighter.h \
    codeeditor.h \
    verificationrun.h \
    simulationrun.h \
    spinrun.h \
    syntaxrun.h \
    variable.h \
    settingswindow.h

FORMS    += mainwindow.ui \
    about.ui \
    options.ui

CONFIG   += c++11

target.path = /usr/local/bin/
INSTALLS += target

RESOURCES += \
    icons.qrc

DISTFILES += \
    winIcon.rc

win32 {
INCLUDEPATH  += C:\Users\Martin\Documents\qspin\boost_1_63_0 -lboost_filesystem
LIBS += "-LC:\Users\Martin\Documents\qspin\boost_1_63_0\stage\lib"
RC_FILE = winIcon.rc
}
