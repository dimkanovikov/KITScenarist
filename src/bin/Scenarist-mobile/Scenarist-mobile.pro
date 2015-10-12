#-------------------------------------------------
#
# Project created by QtCreator 2015-10-11T13:44:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Scenarist-mobile
TEMPLATE = app


SOURCES += main.cpp\
    ManagementLayer/ApplicationManager.cpp \
    Application.cpp

HEADERS  += \
    ManagementLayer/ApplicationManager.h \
    Application.h

RESOURCES += \
    Resources/Resources.qrc
