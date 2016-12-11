TEMPLATE = subdirs

#
# Конфигурируем расположение файлов сборки
#
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../../build/Debug/libs
} else {
    DESTDIR = $$PWD/../../../build/Release/libs
}
#

SUBDIRS = \
    hunspell \
    fileformats \
    webloader \
    mythes

win32: SUBDIRS += qBreakpad
