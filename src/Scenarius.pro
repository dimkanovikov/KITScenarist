#-------------------------------------------------
#
# Project created by QtCreator 2013-11-17T14:31:29
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Scenarius
TEMPLATE = app

INCLUDEPATH += $$PWD

HEADERS  += \
	Domain/Action.h \
	Domain/Character.h \
	Domain/Dialog.h \
	Domain/Folder.h \
	Domain/Scene.h \
	Domain/SceneGroup.h \
	Domain/Parenthetical.h \
	Domain/Transition.h \
	Domain/Title.h \
	Domain/OtherText.h \
	Domain/SimpleText.h \
	Domain/ScenarioDay.h \
	Domain/Location.h \
	Domain/Place.h \
	Domain/Time.h \
	Domain/ScenarioObject.h \
	UserInterface/Widgets/SpellCheckTextEdit/SpellCheckHighlighter.h \
	UserInterface/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.h \
	UserInterface/Widgets/SpellCheckTextEdit/SpellChecker.h \
	UserInterface/Widgets/CompletableTextEdit/CompletableTextEdit.h \
#
	BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h \
	BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStyle.h \
	BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStylePrivate.h \
	BusinessLogic/ScenarioTextEdit/Parsers/SceneHeaderParser.h \
    BusinessLogic/ScenarioTextEdit/Handlers/KeyPressHandlerFacade.h \
    Database/Database.h \
    DataMapper/PlaceMapper.h \
    DataMapper/AbstractMapper.h \
    Domain/Identifier.h \
    Domain/DomainObject.h \
    DataMapper/MapperFacade.h \
    Storage/PlaceStorage.h \
    Storage/StorageFacade.h \
    DataMapper/LocationMapper.h \
    DataMapper/TimeMapper.h \
    DataMapper/ScenarioDayMapper.h \
    Storage/LocationStorage.h \
    Storage/ScenarioDayStorage.h \
    Storage/TimeStorage.h \
    DataMapper/CharacterMapper.h \
    Storage/CharacterStorage.h \
    BusinessLogic/ScenarioTextEdit/Handlers/AbstractKeyHandler.h \
    BusinessLogic/ScenarioTextEdit/Handlers/ActionHandler.h \
    BusinessLogic/ScenarioTextEdit/Handlers/CharacterHandler.h \
    BusinessLogic/ScenarioTextEdit/Handlers/SceneHeaderHandler.h \
    BusinessLogic/ScenarioTextEdit/Handlers/StandardKeyHandler.h \
    BusinessLogic/ScenarioTextEdit/Parsers/SceneGroupHeaderParser.h \
    BusinessLogic/ScenarioTextEdit/Handlers/PrepareHandler.h \
    BusinessLogic/ScenarioTextEdit/Handlers/PreHandler.h \
    BusinessLogic/ScenarioTextEdit/Handlers/DialogHandler.h \
    BusinessLogic/ScenarioTextEdit/Handlers/ParentheticalHandler.h

SOURCES += main.cpp \
    Domain/Action.cpp \
    Domain/Character.cpp \
    Domain/Dialog.cpp \
    Domain/Folder.cpp \
    Domain/Scene.cpp \
    Domain/SceneGroup.cpp \
    Domain/Parenthetical.cpp \
    Domain/Transition.cpp \
    Domain/Title.cpp \
    Domain/OtherText.cpp \
    Domain/SimpleText.cpp \
    Domain/ScenarioDay.cpp \
    Domain/Location.cpp \
    Domain/Place.cpp \
    Domain/Time.cpp \
	Domain/ScenarioObject.cpp \
	UserInterface/Widgets/SpellCheckTextEdit/SpellCheckHighlighter.cpp \
	UserInterface/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.cpp \
	UserInterface/Widgets/SpellCheckTextEdit/SpellChecker.cpp \
	UserInterface/Widgets/CompletableTextEdit/CompletableTextEdit.cpp \
#
	BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.cpp \
	BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStyle.cpp \
	BusinessLogic/ScenarioTextEdit/Parsers/SceneHeaderParser.cpp \
    BusinessLogic/ScenarioTextEdit/Handlers/KeyPressHandlerFacade.cpp \
    Database/Database.cpp \
    DataMapper/PlaceMapper.cpp \
    DataMapper/AbstractMapper.cpp \
    Domain/Identifier.cpp \
    Domain/DomainObject.cpp \
    DataMapper/MapperFacade.cpp \
    Storage/PlaceStorage.cpp \
    Storage/StorageFacade.cpp \
    DataMapper/LocationMapper.cpp \
    DataMapper/TimeMapper.cpp \
    DataMapper/ScenarioDayMapper.cpp \
    Storage/LocationStorage.cpp \
    Storage/ScenarioDayStorage.cpp \
    Storage/TimeStorage.cpp \
    DataMapper/CharacterMapper.cpp \
    Storage/CharacterStorage.cpp \
    BusinessLogic/ScenarioTextEdit/Handlers/AbstractKeyHandler.cpp \
    BusinessLogic/ScenarioTextEdit/Handlers/ActionHandler.cpp \
    BusinessLogic/ScenarioTextEdit/Handlers/CharacterHandler.cpp \
    BusinessLogic/ScenarioTextEdit/Handlers/SceneHeaderHandler.cpp \
    BusinessLogic/ScenarioTextEdit/Handlers/StandardKeyHandler.cpp \
    BusinessLogic/ScenarioTextEdit/Parsers/SceneGroupHeaderParser.cpp \
    BusinessLogic/ScenarioTextEdit/Handlers/PrepareHandler.cpp \
    BusinessLogic/ScenarioTextEdit/Handlers/PreHandler.cpp \
    BusinessLogic/ScenarioTextEdit/Handlers/DialogHandler.cpp \
    BusinessLogic/ScenarioTextEdit/Handlers/ParentheticalHandler.cpp


#
# Для удобной навигации по сторонним проектам
#
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

OTHER_FILES += \
	TODO
