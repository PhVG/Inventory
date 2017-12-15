#-------------------------------------------------
#
# Project created by QtCreator 2017-12-12T16:48:02
#
#-------------------------------------------------

QT       += core gui sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = inventory
TEMPLATE = app


SOURCES += main.cpp\
    mainwidget.cpp \
    gamefield.cpp \
    mimedata.cpp

HEADERS  += \
    mainwidget.h \
    gamefield.h \
    mimedata.h

FORMS += \
    mainwidget.ui

RESOURCES += \
    resources.qrc

CONFIG += c++11
