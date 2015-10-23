#-------------------------------------------------
#
# Project created by QtCreator 2015-10-16T21:36:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RV_CTImage
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bitwriter.cpp \
    bitreader.cpp

HEADERS  += mainwindow.h \
    bitwriter.h \
    bitreader.h

FORMS    += mainwindow.ui

RESOURCES = colors.qrc
