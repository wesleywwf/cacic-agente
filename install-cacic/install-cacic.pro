#-------------------------------------------------
#
# Project created by QtCreator 2014-07-03T16:13:12
#
#-------------------------------------------------

QT += core

QT -= gui

TARGET = install-cacic
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    installcacic.cpp \
    ../src/ccacic.cpp

HEADERS += \
    installcacic.h \
    ../src/CACIC_comm.h \
    ../src/ccacic.h
