TEMPLATE = lib
TARGET = qBreakpad
#Application version
#VERSION = 0.4.0

CONFIG += warn_on thread exceptions rtti stl
QT -= gui
QT += core

#
# Конфигурируем расположение файлов сборки
#
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../../../build/Debug/libs/qBreakpad
} else {
    DESTDIR = $$PWD/../../../../build/Release/libs/qBreakpad
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
#

### qBreakpad config
include($$PWD/../config.pri)

## google-breakpad
include($$PWD/../third_party/breakpad.pri)

HEADERS += \
    $$PWD/singletone/call_once.h \
    $$PWD/singletone/singleton.h \
    $$PWD/QBreakpadHandler.h

SOURCES += \
    $$PWD/QBreakpadHandler.cpp
