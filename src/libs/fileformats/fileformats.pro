TARGET   = fileformats
TEMPLATE = lib

#
# Build configuration
#
CONFIG += qt thread warn_on

mac {
     LIBS += -lz
}

HEADERS += \
    docx_reader.h \
    docx_writer.h \
    format_manager.h \
    format_reader.h \
    odt_reader.h \
    odt_writer.h \
    rtf_reader.h \
    rtf_tokenizer.h \
    rtf_writer.h \
    txt_reader.h \
    qtzip/qtzipreader.h \
    qtzip/QtZipReader \
    qtzip/qtzipwriter.h \
    qtzip/QtZipWriter

SOURCES += \
    docx_reader.cpp \
    docx_writer.cpp \
    format_manager.cpp \
    odt_reader.cpp \
    odt_writer.cpp \
    rtf_reader.cpp \
    rtf_tokenizer.cpp \
    rtf_writer.cpp \
    txt_reader.cpp \
    qtzip/qtzip.cpp
