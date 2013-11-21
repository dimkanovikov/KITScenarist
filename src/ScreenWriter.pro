#-------------------------------------------------
#
# Project created by QtCreator 2013-11-17T14:31:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ScreenWriter
TEMPLATE = app


SOURCES += main.cpp \
	UserInterface/Widgets/ScenarioTextEdit/ScenarioTextEdit.cpp \
    UserInterface/Widgets/SpellCheckTextEdit/SpellCheckHighlighter.cpp \
    UserInterface/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.cpp \
    UserInterface/Widgets/SpellCheckTextEdit/SpellChecker.cpp

HEADERS  += \
	UserInterface/Widgets/ScenarioTextEdit/ScenarioTextEdit.h \
    UserInterface/Widgets/SpellCheckTextEdit/SpellCheckHighlighter.h \
    UserInterface/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.h \
    UserInterface/Widgets/SpellCheckTextEdit/SpellChecker.h

INCLUDEPATH += 3rd_party

#
# Hunspell
#
SOURCES += \
	3rd_party/hunspell/hunzip.cxx \
	3rd_party/hunspell/hunspell.cxx \
	3rd_party/hunspell/hashmgr.cxx \
	3rd_party/hunspell/utf_info.cxx \
	3rd_party/hunspell/suggestmgr.cxx \
	3rd_party/hunspell/phonet.cxx \
	3rd_party/hunspell/filemgr.cxx \
	3rd_party/hunspell/dictmgr.cxx \
	3rd_party/hunspell/csutil.cxx \
	3rd_party/hunspell/affixmgr.cxx \
	3rd_party/hunspell/affentry.cxx

HEADERS  += \
	3rd_party/hunspell/langnum.hxx \
	3rd_party/hunspell/hunzip.hxx \
	3rd_party/hunspell/hunspell.hxx \
	3rd_party/hunspell/hunspell.h \
	3rd_party/hunspell/htypes.hxx \
	3rd_party/hunspell/hashmgr.hxx \
	3rd_party/hunspell/filemgr.hxx \
	3rd_party/hunspell/w_char.hxx \
	3rd_party/hunspell/suggestmgr.hxx \
	3rd_party/hunspell/phonet.hxx \
	3rd_party/hunspell/dictmgr.hxx \
	3rd_party/hunspell/csutil.hxx \
	3rd_party/hunspell/baseaffix.hxx \
	3rd_party/hunspell/atypes.hxx \
	3rd_party/hunspell/affixmgr.hxx \
	3rd_party/hunspell/affentry.hxx
#

RESOURCES += \
    Resources/Resources.qrc
