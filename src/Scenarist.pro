#-------------------------------------------------
#
# Project created by QtCreator 2014-01-26T12:01:05
#
#-------------------------------------------------

QT       += core gui sql xml widgets printsupport

TARGET = Scenarist
TEMPLATE = app

#
# Для создания предустановленных справочников на русском языке
#
DEFINES += USE_RUSSIAN_DATABASE_ENUMS

INCLUDEPATH += $$PWD

TRANSLATIONS += \
	Resources/Translations/Scenarist_ru.ts

SOURCES += main.cpp \
    ManagementLayer/ApplicationManager.cpp \
    ManagementLayer/Scenario/ScenarioManager.cpp \
    ManagementLayer/Scenario/ScenarioTextEditManager.cpp \
    ManagementLayer/Scenario/ScenarioNavigatorManager.cpp \
    ManagementLayer/Scenario/ScenarioCardsManager.cpp \
    ManagementLayer/Project/ProjectNavigatorManager.cpp \
    ManagementLayer/Project/ProjectCardsManager.cpp \
    ManagementLayer/Locations/LocationsNavigatorManager.cpp \
    ManagementLayer/Locations/LocationsManager.cpp \
    ManagementLayer/Project/ProjectManager.cpp \
    ManagementLayer/Characters/CharactersManager.cpp \
    ManagementLayer/Characters/CharactersNavigatorManager.cpp \
    ManagementLayer/Settings/SettingsManager.cpp \
    ManagementLayer/StartUp/StartUpManager.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEdit.cpp \
    UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigator.cpp \
    UserInterfaceLayer/Scenario/ScenarioDataEdit/ScenarioDataEdit.cpp \
    ManagementLayer/Scenario/ScenarioDataEditManager.cpp \
    ManagementLayer/Project/ProjectDataEditManager.cpp \
    ManagementLayer/Locations/LocationsDataEditManager.cpp \
    ManagementLayer/Characters/CharactersDataEditManager.cpp \
    UserInterfaceLayer/Project/ProjectDataEdit/ProjectDataEdit.cpp \
    UserInterfaceLayer/Project/ProjectNavigator/ProjectNavigator.cpp \
    UserInterfaceLayer/Scenario/ScenarioCards/ScenarioCards.cpp \
    UserInterfaceLayer/Project/ProjectCards/ProjectCards.cpp \
    UserInterfaceLayer/Characters/CharactersDataEdit/CharactersDataEdit.cpp \
    UserInterfaceLayer/Characters/CharactersNavigator/CharactersNavigator.cpp \
    UserInterfaceLayer/Locations/LocationsDataEdit/LocationsDataEdit.cpp \
    UserInterfaceLayer/Locations/LocationsNavigator/LocationsNavigator.cpp \
    BusinessLayer/ScenarioDocument/ScenarioDocument.cpp \
    BusinessLayer/ScenarioDocument/ScenarioModel.cpp \
    BusinessLayer/ScenarioDocument/ScenarioModelItem.cpp \
    BusinessLayer/Chronometry/CharactersChronometer.cpp \
    BusinessLayer/Chronometry/ChronometerFacade.cpp \
    BusinessLayer/Chronometry/ConfigurableChronometer.cpp \
    BusinessLayer/Chronometry/PagesChronometer.cpp \
    DataLayer/Database/Database.cpp \
    DataLayer/Database/DatabaseHelper.cpp \
    DataLayer/DataMappingLayer/AbstractMapper.cpp \
    DataLayer/DataMappingLayer/CharacterMapper.cpp \
    DataLayer/DataMappingLayer/LocationMapper.cpp \
    DataLayer/DataMappingLayer/MapperFacade.cpp \
    DataLayer/DataMappingLayer/PlaceMapper.cpp \
    DataLayer/DataMappingLayer/ScenarioDayMapper.cpp \
    DataLayer/DataMappingLayer/ScenarioMapper.cpp \
    DataLayer/DataMappingLayer/SettingsMapper.cpp \
    DataLayer/DataMappingLayer/TimeMapper.cpp \
    DataLayer/DataStorageLayer/CharacterStorage.cpp \
    DataLayer/DataStorageLayer/LocationStorage.cpp \
    DataLayer/DataStorageLayer/PlaceStorage.cpp \
    DataLayer/DataStorageLayer/ScenarioDayStorage.cpp \
    DataLayer/DataStorageLayer/ScenarioStorage.cpp \
    DataLayer/DataStorageLayer/SettingsStorage.cpp \
    DataLayer/DataStorageLayer/StorageFacade.cpp \
    DataLayer/DataStorageLayer/TimeStorage.cpp \
    Domain/Character.cpp \
    Domain/DomainObject.cpp \
    Domain/Identifier.cpp \
    Domain/Location.cpp \
    Domain/Place.cpp \
    Domain/Scenario.cpp \
    Domain/ScenarioDay.cpp \
    Domain/Time.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/AbstractKeyHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/ActionHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/CharacterHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/DialogHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/FolderFooterHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/FolderHeaderHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/KeyPressHandlerFacade.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/NoteHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/ParentheticalHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/PreHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/PrepareHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/SceneGroupFooterHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/SceneGroupHeaderHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/SimpleTextHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/StandardKeyHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/TimeAndPlaceHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/TitleHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/TransitionHandler.cpp \
    3rd_party/Hunspell/affentry.cxx \
    3rd_party/Hunspell/affixmgr.cxx \
    3rd_party/Hunspell/csutil.cxx \
    3rd_party/Hunspell/dictmgr.cxx \
    3rd_party/Hunspell/filemgr.cxx \
    3rd_party/Hunspell/hashmgr.cxx \
    3rd_party/Hunspell/hunspell.cxx \
    3rd_party/Hunspell/hunzip.cxx \
    3rd_party/Hunspell/phonet.cxx \
    3rd_party/Hunspell/suggestmgr.cxx \
    3rd_party/Hunspell/utf_info.cxx \
    3rd_party/Widgets/CompletableTextEdit/CompletableTextEdit.cpp \
    3rd_party/Widgets/SpellCheckTextEdit/SpellChecker.cpp \
    3rd_party/Widgets/SpellCheckTextEdit/SpellCheckHighlighter.cpp \
    3rd_party/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.cpp \
    BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.cpp \
    UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigatorItemDelegate.cpp \
    UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigatorItemWidget.cpp \
    3rd_party/Widgets/ElidedLabel/ElidedLabel.cpp \
    BusinessLayer/Export/PdfExporter.cpp \
    BusinessLayer/Export/RtfExporter.cpp \
    BusinessLayer/ScenarioDocument/ScenarioXml.cpp \
    BusinessLayer/ScenarioDocument/ScenarioTextDocument.cpp \
    UserInterfaceLayer/StartUp/StartUpView.cpp \
    UserInterfaceLayer/StartUp/RecentFilesDelegate.cpp \
    UserInterfaceLayer/StartUp/RecentFileWidget.cpp \
    3rd_party/Widgets/SideBar/SideBar.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEditWidget.cpp \
    BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.cpp \
    UserInterfaceLayer/Settings/SettingsView.cpp \
    UserInterfaceLayer/ApplicationView.cpp \
    UserInterfaceLayer/Characters/CharactersNavigator/CharactersNavigatorItemDelegate.cpp \
    UserInterfaceLayer/Characters/CharactersNavigator/CharactersNavigatorItemWidget.cpp \
    UserInterfaceLayer/Locations/LocationsNavigator/LocationsNavigatorItemDelegate.cpp \
	UserInterfaceLayer/Locations/LocationsNavigator/LocationsNavigatorItemWidget.cpp \
	3rd_party/Widgets/ZoomableTextEdit/ZoomableTextEdit.cpp \
	3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.cpp \
    3rd_party/Widgets/PhotosChooser/PhotosChooser.cpp \
    3rd_party/Widgets/PhotosChooser/PhotoLabel.cpp \
    3rd_party/Widgets/QLightBoxWidget/qlightboxwidget.cpp \
    3rd_party/Widgets/PhotosChooser/PhotoPreview.cpp \
    Domain/LocationPhoto.cpp \
    DataLayer/DataMappingLayer/LocationPhotoMapper.cpp \
    DataLayer/DataStorageLayer/LocationPhotoStorage.cpp

HEADERS += \
    ManagementLayer/ApplicationManager.h \
    ManagementLayer/Scenario/ScenarioManager.h \
    ManagementLayer/Scenario/ScenarioTextEditManager.h \
    ManagementLayer/Scenario/ScenarioNavigatorManager.h \
    ManagementLayer/Scenario/ScenarioCardsManager.h \
    ManagementLayer/Project/ProjectNavigatorManager.h \
    ManagementLayer/Project/ProjectCardsManager.h \
    ManagementLayer/Locations/LocationsNavigatorManager.h \
    ManagementLayer/Locations/LocationsManager.h \
    ManagementLayer/Project/ProjectManager.h \
    ManagementLayer/Characters/CharactersManager.h \
    ManagementLayer/Characters/CharactersNavigatorManager.h \
    ManagementLayer/Settings/SettingsManager.h \
    ManagementLayer/StartUp/StartUpManager.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEdit.h \
    UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigator.h \
    UserInterfaceLayer/Scenario/ScenarioDataEdit/ScenarioDataEdit.h \
    ManagementLayer/Scenario/ScenarioDataEditManager.h \
    ManagementLayer/Project/ProjectDataEditManager.h \
    ManagementLayer/Locations/LocationsDataEditManager.h \
    ManagementLayer/Characters/CharactersDataEditManager.h \
    UserInterfaceLayer/Project/ProjectDataEdit/ProjectDataEdit.h \
    UserInterfaceLayer/Project/ProjectNavigator/ProjectNavigator.h \
    UserInterfaceLayer/Scenario/ScenarioCards/ScenarioCards.h \
    UserInterfaceLayer/Project/ProjectCards/ProjectCards.h \
    UserInterfaceLayer/Characters/CharactersDataEdit/CharactersDataEdit.h \
    UserInterfaceLayer/Characters/CharactersNavigator/CharactersNavigator.h \
    UserInterfaceLayer/Locations/LocationsDataEdit/LocationsDataEdit.h \
    UserInterfaceLayer/Locations/LocationsNavigator/LocationsNavigator.h \
    BusinessLayer/ScenarioDocument/ScenarioDocument.h \
    BusinessLayer/ScenarioDocument/ScenarioModel.h \
    BusinessLayer/ScenarioDocument/ScenarioModelItem.h \
    BusinessLayer/Chronometry/AbstractChronometer.h \
    BusinessLayer/Chronometry/CharactersChronometer.h \
    BusinessLayer/Chronometry/ChronometerFacade.h \
    BusinessLayer/Chronometry/ConfigurableChronometer.h \
    BusinessLayer/Chronometry/PagesChronometer.h \
    DataLayer/Database/Database.h \
    DataLayer/Database/DatabaseHelper.h \
    DataLayer/DataMappingLayer/AbstractMapper.h \
    DataLayer/DataMappingLayer/CharacterMapper.h \
    DataLayer/DataMappingLayer/LocationMapper.h \
    DataLayer/DataMappingLayer/MapperFacade.h \
    DataLayer/DataMappingLayer/PlaceMapper.h \
    DataLayer/DataMappingLayer/ScenarioDayMapper.h \
    DataLayer/DataMappingLayer/ScenarioMapper.h \
    DataLayer/DataMappingLayer/SettingsMapper.h \
    DataLayer/DataMappingLayer/TimeMapper.h \
    DataLayer/DataStorageLayer/CharacterStorage.h \
    DataLayer/DataStorageLayer/LocationStorage.h \
    DataLayer/DataStorageLayer/PlaceStorage.h \
    DataLayer/DataStorageLayer/ScenarioDayStorage.h \
    DataLayer/DataStorageLayer/ScenarioStorage.h \
    DataLayer/DataStorageLayer/SettingsStorage.h \
    DataLayer/DataStorageLayer/StorageFacade.h \
    DataLayer/DataStorageLayer/TimeStorage.h \
    Domain/Character.h \
    Domain/DomainObject.h \
    Domain/Identifier.h \
    Domain/Location.h \
    Domain/Place.h \
    Domain/Scenario.h \
    Domain/ScenarioDay.h \
    Domain/Time.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/AbstractKeyHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/ActionHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/CharacterHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/DialogHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/FolderFooterHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/FolderHeaderHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/KeyPressHandlerFacade.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/NoteHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/ParentheticalHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/PreHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/PrepareHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/SceneGroupFooterHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/SceneGroupHeaderHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/SimpleTextHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/StandardKeyHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/TimeAndPlaceHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/TitleHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/TitleHeaderHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/TransitionHandler.h \
    3rd_party/Hunspell/affentry.hxx \
    3rd_party/Hunspell/affixmgr.hxx \
    3rd_party/Hunspell/atypes.hxx \
    3rd_party/Hunspell/baseaffix.hxx \
    3rd_party/Hunspell/csutil.hxx \
    3rd_party/Hunspell/dictmgr.hxx \
    3rd_party/Hunspell/filemgr.hxx \
    3rd_party/Hunspell/hashmgr.hxx \
    3rd_party/Hunspell/htypes.hxx \
    3rd_party/Hunspell/hunspell.h \
    3rd_party/Hunspell/hunspell.hxx \
    3rd_party/Hunspell/hunzip.hxx \
    3rd_party/Hunspell/langnum.hxx \
    3rd_party/Hunspell/phonet.hxx \
    3rd_party/Hunspell/suggestmgr.hxx \
    3rd_party/Hunspell/w_char.hxx \
    3rd_party/Widgets/CompletableTextEdit/CompletableTextEdit.h \
    3rd_party/Widgets/SpellCheckTextEdit/SpellChecker.h \
    3rd_party/Widgets/SpellCheckTextEdit/SpellCheckHighlighter.h \
    3rd_party/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.h \
    BusinessLayer/ScenarioDocument/ScenarioTextBlockStylePrivate.h \
    BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h \
    UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigatorItemDelegate.h \
    UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigatorItemWidget.h \
    3rd_party/Widgets/ElidedLabel/ElidedLabel.h \
    BusinessLayer/Export/AbstractExporter.h \
    BusinessLayer/Export/PdfExporter.h \
    BusinessLayer/Export/RtfExporter.h \
    BusinessLayer/ScenarioDocument/ScenarioXml.h \
    BusinessLayer/ScenarioDocument/ScenarioTextDocument.h \
    DataLayer/DataMappingLayer/AbstractMapperPrivate.h \
    UserInterfaceLayer/StartUp/StartUpView.h \
    UserInterfaceLayer/StartUp/RecentFilesDelegate.h \
    UserInterfaceLayer/StartUp/RecentFileWidget.h \
    3rd_party/Widgets/SideBar/SideBar.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEditWidget.h \
    BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h \
    UserInterfaceLayer/Settings/SettingsView.h \
    UserInterfaceLayer/ApplicationView.h \
    UserInterfaceLayer/Characters/CharactersNavigator/CharactersNavigatorItemDelegate.h \
    UserInterfaceLayer/Characters/CharactersNavigator/CharactersNavigatorItemWidget.h \
    UserInterfaceLayer/Locations/LocationsNavigator/LocationsNavigatorItemDelegate.h \
	UserInterfaceLayer/Locations/LocationsNavigator/LocationsNavigatorItemWidget.h \
	3rd_party/Widgets/ZoomableTextEdit/ZoomableTextEdit.h \
	3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.h \
    3rd_party/Widgets/PhotosChooser/PhotosChooser.h \
    3rd_party/Widgets/PhotosChooser/PhotoLabel.h \
    3rd_party/Widgets/QLightBoxWidget/qlightboxwidget.h \
    3rd_party/Widgets/PhotosChooser/PhotoPreview.h \
    Domain/LocationPhoto.h \
    DataLayer/DataMappingLayer/LocationPhotoMapper.h \
    DataLayer/DataStorageLayer/LocationPhotoStorage.h

FORMS += \
    UserInterfaceLayer/StartUp/StartUpView.ui \
    UserInterfaceLayer/Characters/CharactersDataEdit/CharactersDataEdit.ui \
    UserInterfaceLayer/Locations/LocationsDataEdit/LocationsDataEdit.ui \
    UserInterfaceLayer/Settings/SettingsView.ui


RESOURCES += \
	Resources/Resources.qrc

OTHER_FILES += \
	TODO \
	settings_keys \
	scenario_settings_keys \
    logo.ico \
    Scenarist.rc

RC_FILE = Scenarist.rc
