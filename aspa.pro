#-------------------------------------------------
#
# Project created by QtCreator 2016-01-18T14:58:16
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = aspa
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += ./src
INCLUDEPATH += ./src/models
INCLUDEPATH += ./src/utils
INCLUDEPATH += ./src/views


SOURCES += src/main.cpp\
	src/mainwindow.cpp \
    src/models/projectlistmodel.cpp \
    src/models/demandlistmodel.cpp \
    src/utils/attributor.cpp \
    src/models/attributionmodel.cpp \
    src/views/aboutdialog.cpp \
    src/utils/munkres.cpp

HEADERS  += src/mainwindow.h \
    src/models/projectlistmodel.h \
    src/models/demandlistmodel.h \
    src/utils/attributor.h \
    src/models/attributionmodel.h \
    src/views/aboutdialog.h \
    src/utils/munkres.h

FORMS    += src/mainwindow.ui \
    src/views/aboutdialog.ui

RESOURCES += \
    rc/icons.qrc
