#-------------------------------------------------
#
# Project created by QtCreator 2016-01-22T12:44:00
#
#-------------------------------------------------

QT       -= gui

#QMAKE_MAC_SDK = macosx10.11

TARGET = mythes
TEMPLATE = lib

DEFINES += MYTHES_LIBRARY

#
# Конфигурируем расположение файлов сборки
#
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../../build/Debug/libs/mythes
} else {
    DESTDIR = $$PWD/../../../build/Release/libs/mythes
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
#

SOURCES += mythes.cpp

HEADERS += mythes.h \
    MyThesGlobal.h

DISTFILES += \
    license.readme
