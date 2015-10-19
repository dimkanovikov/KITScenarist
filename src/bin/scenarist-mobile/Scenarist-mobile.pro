#-------------------------------------------------
#
# Project created by QtCreator 2015-10-11T13:44:15
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = Scenarist
TEMPLATE = app

#
# Отключаем предупреждения о статических функциях и переменных
# не используемых в собственных заголовочных файлах
#
unix: QMAKE_CXXFLAGS += -Wno-unused-function -Wno-unused-variable

#
# Включить профилирование
#
#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg

#
# Подключаем библиотеку HUNSPELL
#
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../libs/hunspell/release/ -lhunspell
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../libs/hunspell/debug/ -lhunspell
else:unix: LIBS += -L$$OUT_PWD/../../libs/hunspell/ -lhunspell

INCLUDEPATH += $$PWD/../../libs/hunspell/src
DEPENDPATH += $$PWD/../../libs/hunspell
#

#
# Подключаем библилотеку WebLoader
#
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../libs/webloader/release/ -lwebloader
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../libs/webloader/debug/ -lwebloader
else:unix: LIBS += -L$$OUT_PWD/../../libs/webloader/ -lwebloader

INCLUDEPATH += $$PWD/../../libs/webloader
DEPENDPATH += $$PWD/../../libs/webloader
#

INCLUDEPATH += $$PWD


SOURCES += main.cpp\
    ManagementLayer/ApplicationManager.cpp \
    Application.cpp \
    ../Scenarist/DataLayer/Database/Database.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/AbstractMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/CharacterMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/CharacterPhotoMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/CharacterStateMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/DatabaseHistoryMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/LocationMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/LocationPhotoMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/MapperFacade.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/PlaceMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/ScenarioChangeMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/ScenarioDataMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/ScenarioDayMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/ScenarioMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/SettingsMapper.cpp \
    ../Scenarist/DataLayer/DataMappingLayer/TimeMapper.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/CharacterPhotoStorage.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/CharacterStateStorage.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/CharacterStorage.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/DatabaseHistoryStorage.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/LocationPhotoStorage.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/LocationStorage.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/PlaceStorage.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/ScenarioChangeStorage.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/ScenarioDataStorage.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/ScenarioDayStorage.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/ScenarioStorage.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/SettingsStorage.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/StorageFacade.cpp \
    ../Scenarist/DataLayer/DataStorageLayer/TimeStorage.cpp

HEADERS  += \
    ManagementLayer/ApplicationManager.h \
    Application.h \
    ../Scenarist/DataLayer/Database/Database.h \
    ../Scenarist/DataLayer/Database/DatabaseHelper.h \
    ../Scenarist/DataLayer/DataMappingLayer/AbstractMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/CharacterMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/CharacterPhotoMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/CharacterStateMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/DatabaseHistoryMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/LocationMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/LocationPhotoMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/MapperFacade.h \
    ../Scenarist/DataLayer/DataMappingLayer/PlaceMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/ScenarioChangeMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/ScenarioDataMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/ScenarioDayMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/ScenarioMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/SettingsMapper.h \
    ../Scenarist/DataLayer/DataMappingLayer/TimeMapper.h \
    ../Scenarist/DataLayer/DataStorageLayer/CharacterPhotoStorage.h \
    ../Scenarist/DataLayer/DataStorageLayer/CharacterStateStorage.h \
    ../Scenarist/DataLayer/DataStorageLayer/CharacterStorage.h \
    ../Scenarist/DataLayer/DataStorageLayer/DatabaseHistoryStorage.h \
    ../Scenarist/DataLayer/DataStorageLayer/LocationPhotoStorage.h \
    ../Scenarist/DataLayer/DataStorageLayer/LocationStorage.h \
    ../Scenarist/DataLayer/DataStorageLayer/PlaceStorage.h \
    ../Scenarist/DataLayer/DataStorageLayer/ScenarioChangeStorage.h \
    ../Scenarist/DataLayer/DataStorageLayer/ScenarioDataStorage.h \
    ../Scenarist/DataLayer/DataStorageLayer/ScenarioDayStorage.h \
    ../Scenarist/DataLayer/DataStorageLayer/ScenarioStorage.h \
    ../Scenarist/DataLayer/DataStorageLayer/SettingsStorage.h \
    ../Scenarist/DataLayer/DataStorageLayer/StorageFacade.h \
    ../Scenarist/DataLayer/DataStorageLayer/TimeStorage.h

RESOURCES += \
    Resources/Resources.qrc
