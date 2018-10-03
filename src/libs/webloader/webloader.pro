TEMPLATE = lib
TARGET = webloader
DEPENDPATH += src
INCLUDEPATH += src

CONFIG += c++11 staticlib

QT += network xml
QT -= gui

QMAKE_MAC_SDK = macosx10.13

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

# Включить создание pdb-файла для релизной сборки
win32-msvc* {
CONFIG += debug_and_release warn_on
CONFIG += thread exceptions rtti stl

QMAKE_CXXFLAGS_RELEASE +=  /Zi
QMAKE_LFLAGS_RELEASE += /DEBUG /OPT:REF /OPT:ICF
}

HEADERS += \
    src/NetworkRequest.h \
    src/NetworkRequestLoader.h \
    src/WebRequest.h \
    src/WebLoader.h \
    src/HttpMultiPart.h \
    src/NetworkQueue.h \
    src/WebRequestParameters.h \
    src/NetworkTypes.h

SOURCES += \
    src/NetworkRequest.cpp \
    src/NetworkRequestLoader.cpp \
    src/WebRequest.cpp \
    src/WebLoader.cpp \
    src/HttpMultiPart.cpp \
    src/NetworkQueue.cpp \
    src/WebRequestParameters.cpp
