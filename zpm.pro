#-------------------------------------------------
#
# Project created by QtCreator 2012-11-28T16:41:52
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = zpm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    packagemodel.cpp \
    packageitemdelegate.cpp

HEADERS  += mainwindow.h \
    packagemodel.h \
    packageitemdelegate.h

FORMS    += mainwindow.ui

TRANSLATIONS = zpm_de.ts
