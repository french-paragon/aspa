#-------------------------------------------------
#
# Project created by QtCreator 2016-01-18T14:58:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = aspa
TEMPLATE = app


SOURCES += src/main.cpp\
	src/mainwindow.cpp \
    src/models/projectlistmodel.cpp

HEADERS  += src/mainwindow.h \
    src/models/projectlistmodel.h

FORMS    += src/mainwindow.ui
