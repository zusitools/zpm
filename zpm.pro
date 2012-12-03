#-------------------------------------------------
#
# Project created by QtCreator 2012-11-28T16:41:52
#
#-------------------------------------------------

QT       += core gui xml xmlpatterns

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = zpm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    packageitemdelegate.cpp \
    package.cpp \
    packageversion.cpp \
    folder.cpp \
    packagetreeitem.cpp \
    packagetreemodel.cpp \
    packagetreesortfilterproxymodel.cpp

HEADERS  += mainwindow.h \
    packageitemdelegate.h \
    package.h \
    packageversion.h \
    folder.h \
    packagetreeitem.h \
    packagetreemodel.h \
    packagetreesortfilterproxymodel.h

FORMS    += mainwindow.ui

TRANSLATIONS = zpm_de.ts
