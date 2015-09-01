#-------------------------------------------------
#
# Project created by QtCreator 2015-03-17T16:41:04
#
#-------------------------------------------------

QT       -= gui
QT       += network xml

TARGET = webloader
TEMPLATE = lib

DEFINES += WEBLOADER_LIBRARY

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
    WebLoaderGlobal.h

RESOURCES += \
    resources.qrc

