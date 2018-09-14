TARGET   = fileformats
TEMPLATE = lib

DEFINES += FILEFORMATS_LIBRARY

#
# Build configuration
#
CONFIG += qt thread warn_on staticlib

QMAKE_MAC_SDK = macosx10.12

#
# Конфигурируем расположение файлов сборки
#
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../../build/Debug/libs/fileformats
} else {
    DESTDIR = $$PWD/../../../build/Release/libs/fileformats
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
#

mac {
     LIBS += -lz
}

HEADERS += \
    docx_reader.h \
    docx_writer.h \
    format_manager.h \
    format_reader.h \
    odt_reader.h \
    rtf_reader.h \
    rtf_tokenizer.h \
    txt_reader.h \
    qtzip/qtzipreader.h \
    qtzip/QtZipReader \
    qtzip/qtzipwriter.h \
    qtzip/QtZipWriter \
    format_helpers.h \
    fileformatsglobal.h

SOURCES += \
    docx_reader.cpp \
    docx_writer.cpp \
    format_manager.cpp \
    odt_reader.cpp \
    rtf_reader.cpp \
    rtf_tokenizer.cpp \
    txt_reader.cpp \
    qtzip/qtzip.cpp
