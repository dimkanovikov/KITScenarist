TEMPLATE = lib
TARGET = webloader
DEPENDPATH += src
INCLUDEPATH += src

CONFIG += c++11

QT += network xml
QT -= gui

QMAKE_MAC_SDK = macosx10.12
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

HEADERS += src/HttpMultiPart_p.h \
	   src/NetworkQueue_p.h \
	   src/NetworkRequest.h \
	   src/NetworkRequestPrivate_p.h \
	   src/WebLoader_p.h \
	   src/WebRequest_p.h \
	   src/NetworkRequestLoader.h \
           src/WebLoaderGlobal.h

SOURCES += src/HttpMultiPart_p.cpp \
	   src/WebLoader_p.cpp \
	   src/WebRequest_p.cpp \
	   src/NetworkQueue_p.cpp \
	   src/NetworkRequest.cpp \
    src/NetworkRequestLoader.cpp
