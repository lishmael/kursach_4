#-------------------------------------------------
#
# Project created by QtCreator 2012-12-02T14:35:28
#
#-------------------------------------------------

QT       += core gui network

TARGET = Server
TEMPLATE = app


SOURCES += main.cpp\
	server.cpp \
    mainwindow.cpp

HEADERS  += server.h \
    mainwindow.h

FORMS    += \
    mainwindow.ui

QMAKE_CXXFLAGS += -lcryptopp
