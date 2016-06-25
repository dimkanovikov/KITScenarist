#-------------------------------------------------
#
# Project created by QtCreator 2015-03-17T16:41:04
#
#-------------------------------------------------

QT       -= gui
QT       += network xml

#QMAKE_MAC_SDK = macosx10.11

TARGET = webloader
TEMPLATE = lib

DEFINES += WEBLOADER_LIBRARY

#
# Конфигурируем расположение файлов сборки
#
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../../build/Debug/libs/webloader
} else {
    DESTDIR = $$PWD/../../../build/Release/libs/webloader
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
#

SOURCES += \
    WebRequest.cpp \
    WebLoader.cpp \
    HttpMultiPart.cpp \
    QFreeDesktopMime/freedesktopmime.cpp

HEADERS += \
    WebRequest.h \
    WebLoader.h \
    HttpMultiPart.h \
    QFreeDesktopMime/freedesktopmime.h \
    WebLoaderGlobal.h \
    WebLoaderGlobal.h

RESOURCES += \
    resources.qrc

