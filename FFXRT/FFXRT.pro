#-------------------------------------------------
#
# Project created by QtCreator 2016-05-16T20:15:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FFXRT
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

win32 {
    RC_FILE = ffxrt.rc
    LIBS += -lUser32
}
