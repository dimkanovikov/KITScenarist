#-------------------------------------------------
#
# Project created by QtCreator 2014-01-26T12:01:05
#
#-------------------------------------------------

QT       += core gui sql xml widgets printsupport network

TARGET = Scenarist
TEMPLATE = app

#
# Отключаем предупреждения о статических функциях и переменных
# не используемых в собственных заголовочных файлах
#
QMAKE_CXXFLAGS += -Wno-unused-function -Wno-unused-variable

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
# Подключаем библиотеку fileformats
#
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../libs/fileformats/release/ -lfileformats
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../libs/fileformats/debug/ -lfileformats
else:unix: LIBS += -L$$OUT_PWD/../../libs/fileformats/ -lfileformats

INCLUDEPATH += $$PWD/../../libs/fileformats
DEPENDPATH += $$PWD/../../libs/fileformats
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

SOURCES += main.cpp \
    ManagementLayer/ApplicationManager.cpp \
    ManagementLayer/Scenario/ScenarioManager.cpp \
    ManagementLayer/Scenario/ScenarioTextEditManager.cpp \
    ManagementLayer/Scenario/ScenarioNavigatorManager.cpp \
    ManagementLayer/Scenario/ScenarioCardsManager.cpp \
    ManagementLayer/Locations/LocationsNavigatorManager.cpp \
    ManagementLayer/Locations/LocationsManager.cpp \
    ManagementLayer/Characters/CharactersManager.cpp \
    ManagementLayer/Characters/CharactersNavigatorManager.cpp \
    ManagementLayer/Settings/SettingsManager.cpp \
    ManagementLayer/StartUp/StartUpManager.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEdit.cpp \
    UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigator.cpp \
    UserInterfaceLayer/Scenario/ScenarioDataEdit/ScenarioDataEdit.cpp \
    ManagementLayer/Scenario/ScenarioDataEditManager.cpp \
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
    DataLayer/Database/Database.cpp \
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
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/StandardKeyHandler.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/TitleHandler.cpp \
	UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/TransitionHandler.cpp \
    3rd_party/Widgets/CompletableTextEdit/CompletableTextEdit.cpp \
    3rd_party/Widgets/SpellCheckTextEdit/SpellChecker.cpp \
    3rd_party/Widgets/SpellCheckTextEdit/SpellCheckHighlighter.cpp \
	3rd_party/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.cpp \
    UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigatorItemDelegate.cpp \
    3rd_party/Widgets/ElidedLabel/ElidedLabel.cpp \
    BusinessLayer/Export/PdfExporter.cpp \
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
	3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.cpp \
    3rd_party/Widgets/PhotosChooser/PhotosChooser.cpp \
    3rd_party/Widgets/PhotosChooser/PhotoLabel.cpp \
    3rd_party/Widgets/QLightBoxWidget/qlightboxwidget.cpp \
    3rd_party/Widgets/PhotosChooser/PhotoPreview.cpp \
    Domain/LocationPhoto.cpp \
    DataLayer/DataMappingLayer/LocationPhotoMapper.cpp \
    DataLayer/DataStorageLayer/LocationPhotoStorage.cpp \
    Domain/CharacterPhoto.cpp \
    DataLayer/DataMappingLayer/CharacterPhotoMapper.cpp \
    DataLayer/DataStorageLayer/CharacterPhotoStorage.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEditHelpers.cpp \
    BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.cpp \
	UserInterfaceLayer/Scenario/ScenarioSceneSynopsis/ScenarioSceneSynopsis.cpp \
    ManagementLayer/Scenario/ScenarioSceneSynopsisManager.cpp \
    3rd_party/Widgets/TabBar/TabBar.cpp \
    3rd_party/Widgets/SpellCheckTextEdit/SyntaxHighlighter.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioLineEdit.cpp \
    UserInterfaceLayer/Scenario/ScenarioItemDialog/ScenarioItemDialog.cpp \
	3rd_party/Widgets/PagesTextEdit/PagesTextEdit.cpp \
    3rd_party/Widgets/SearchWidget/SearchWidget.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioFastFormatWidget.cpp \
    3rd_party/Widgets/PagesTextEdit/PageMetrics.cpp \
    UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigatorProxyStyle.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/SceneCharactersHandler.cpp \
    ManagementLayer/Export/ExportManager.cpp \
    UserInterfaceLayer/Export/ExportDialog.cpp \
    Domain/CharacterState.cpp \
    DataLayer/DataMappingLayer/CharacterStateMapper.cpp \
    DataLayer/DataStorageLayer/CharacterStateStorage.cpp \
    BusinessLayer/Export/AbstractExporter.cpp \
    BusinessLayer/Counters/CountersFacade.cpp \
    Application.cpp \
    ManagementLayer/Import/ImportManager.cpp \
    UserInterfaceLayer/Import/ImportDialog.cpp \
    3rd_party/Widgets/ProgressWidget/ProgressWidget.cpp \
    BusinessLayer/Import/DocumentImporter.cpp \
    BusinessLayer/Export/DocxExporter.cpp \
    3rd_party/Widgets/ScalableWrapper/ScalableWrapper.cpp \
    BusinessLayer/Chronometry/PagesChronometer.cpp \
    3rd_party/Widgets/AcceptebleLineEdit/AcceptebleLineEdit.cpp \
    3rd_party/Delegates/ComboBoxItemDelegate/ComboBoxItemDelegate.cpp \
    3rd_party/Helpers/BackupHelper.cpp \
    3rd_party/Widgets/HierarchicalHeaderView/HierarchicalHeaderView.cpp \
    3rd_party/Widgets/HierarchicalHeaderView/HierarchicalTableModel.cpp \
    3rd_party/Widgets/FlatButton/FlatButton.cpp \
    3rd_party/Delegates/KeySequenceDelegate/KeySequenceDelegate.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEditPrivate.cpp \
    ManagementLayer/Synchronization/SynchronizationManager.cpp \
    UserInterfaceLayer/Settings/TemplateDialog.cpp \
    ManagementLayer/Settings/SettingsTemplatesManager.cpp \
    BusinessLayer/ScenarioDocument/ScenarioTemplate.cpp \
    UserInterfaceLayer/StartUp/LoginDialog.cpp \
    ManagementLayer/Project/ProjectsManager.cpp \
    ManagementLayer/Project/Project.cpp \
    DataLayer/DataMappingLayer/DatabaseHistoryMapper.cpp \
    DataLayer/DataStorageLayer/DatabaseHistoryStorage.cpp \
    Domain/ScenarioChange.cpp \
    DataLayer/DataMappingLayer/ScenarioChangeMapper.cpp \
    DataLayer/DataStorageLayer/ScenarioChangeStorage.cpp \
    3rd_party/Helpers/DiffMatchPatch.cpp \
    Domain/ScenarioData.cpp \
    DataLayer/DataStorageLayer/ScenarioDataStorage.cpp \
    DataLayer/DataMappingLayer/ScenarioDataMapper.cpp \
    3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.cpp \
    3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.cpp \
    3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/NoprintableTextHandler.cpp \
    3rd_party/Widgets/ToolTipLabel/ToolTipLabel.cpp \
    BusinessLayer/ScenarioDocument/ScenarioReviewModel.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioReviewPanel.cpp \
    3rd_party/Widgets/ColoredToolButton/ColoredToolButton.cpp \
    3rd_party/Widgets/ColoredToolButton/GoogleColorsPane.cpp \
    3rd_party/Widgets/ColoredToolButton/WordHighlightColorsPane.cpp \
    3rd_party/Widgets/QWidgetListView/qwidgetlistview.cpp \
    3rd_party/Widgets/QWidgetListView/qtmodelwidget.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioReviewView.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioReviewItemDelegate.cpp \
    3rd_party/Widgets/Ctk/ctkCollapsibleButton.cpp \
    UserInterfaceLayer/Statistics/StatisticsView.cpp \
    ManagementLayer/Statistics/StatisticsManager.cpp \
    3rd_party/Widgets/Ctk/ctkPopupWidget.cpp \
    3rd_party/Widgets/Ctk/ctkBasePopupWidget.cpp \
    UserInterfaceLayer/Statistics/StatisticsSettings.cpp \
    UserInterfaceLayer/Statistics/ReportButton.cpp \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/SceneHeadingHandler.cpp \
    BusinessLayer/Statistics/LocationReport.cpp \
    BusinessLayer/Statistics/SceneReport.cpp \
    BusinessLayer/Statistics/ReportFacade.cpp \
    BusinessLayer/Statistics/CastReport.cpp \
    BusinessLayer/Statistics/CharacterReport.cpp \
    BusinessLayer/Statistics/SummaryReport.cpp \
    3rd_party/Widgets/PopupWidget/PopupWidget.cpp

HEADERS += \
    ManagementLayer/ApplicationManager.h \
    ManagementLayer/Scenario/ScenarioManager.h \
    ManagementLayer/Scenario/ScenarioTextEditManager.h \
    ManagementLayer/Scenario/ScenarioNavigatorManager.h \
    ManagementLayer/Scenario/ScenarioCardsManager.h \
    ManagementLayer/Locations/LocationsNavigatorManager.h \
    ManagementLayer/Locations/LocationsManager.h \
    ManagementLayer/Characters/CharactersManager.h \
    ManagementLayer/Characters/CharactersNavigatorManager.h \
    ManagementLayer/Settings/SettingsManager.h \
    ManagementLayer/StartUp/StartUpManager.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEdit.h \
    UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigator.h \
    UserInterfaceLayer/Scenario/ScenarioDataEdit/ScenarioDataEdit.h \
    ManagementLayer/Scenario/ScenarioDataEditManager.h \
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
    DataLayer/Database/Database.h \
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
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/StandardKeyHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/TitleHandler.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/TitleHeaderHandler.h \
	UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/TransitionHandler.h \
    3rd_party/Widgets/CompletableTextEdit/CompletableTextEdit.h \
    3rd_party/Widgets/SpellCheckTextEdit/SpellChecker.h \
    3rd_party/Widgets/SpellCheckTextEdit/SpellCheckHighlighter.h \
	3rd_party/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.h \
    UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigatorItemDelegate.h \
    3rd_party/Widgets/ElidedLabel/ElidedLabel.h \
    BusinessLayer/Export/AbstractExporter.h \
    BusinessLayer/Export/PdfExporter.h \
    BusinessLayer/ScenarioDocument/ScenarioXml.h \
    BusinessLayer/ScenarioDocument/ScenarioTextDocument.h \
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
	3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.h \
    3rd_party/Widgets/PhotosChooser/PhotosChooser.h \
    3rd_party/Widgets/PhotosChooser/PhotoLabel.h \
    3rd_party/Widgets/QLightBoxWidget/qlightboxwidget.h \
    3rd_party/Widgets/PhotosChooser/PhotoPreview.h \
    Domain/LocationPhoto.h \
    DataLayer/DataMappingLayer/LocationPhotoMapper.h \
    DataLayer/DataStorageLayer/LocationPhotoStorage.h \
    Domain/CharacterPhoto.h \
    DataLayer/DataMappingLayer/CharacterPhotoMapper.h \
    DataLayer/DataStorageLayer/CharacterPhotoStorage.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEditHelpers.h \
    BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h \
	UserInterfaceLayer/Scenario/ScenarioSceneSynopsis/ScenarioSceneSynopsis.h \
    ManagementLayer/Scenario/ScenarioSceneSynopsisManager.h \
    3rd_party/Widgets/TabBar/TabBar.h \
    3rd_party/Widgets/SpellCheckTextEdit/SyntaxHighlighter.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioLineEdit.h \
    UserInterfaceLayer/Scenario/ScenarioItemDialog/ScenarioItemDialog.h \
	3rd_party/Widgets/PagesTextEdit/PagesTextEdit.h \
    3rd_party/Widgets/SearchWidget/SearchWidget.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioFastFormatWidget.h \
    3rd_party/Widgets/PagesTextEdit/PageMetrics.h \
    3rd_party/Helpers/TextEditHelper.h \
    UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigatorProxyStyle.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/SceneCharactersHandler.h \
    ManagementLayer/Export/ExportManager.h \
    UserInterfaceLayer/Export/ExportDialog.h \
    Domain/CharacterState.h \
    DataLayer/DataMappingLayer/CharacterStateMapper.h \
    DataLayer/DataStorageLayer/CharacterStateStorage.h \
    BusinessLayer/Counters/CountersFacade.h \
    Application.h \
    BusinessLayer/Import/AbstractImporter.h \
    ManagementLayer/Import/ImportManager.h \
    UserInterfaceLayer/Import/ImportDialog.h \
    3rd_party/Widgets/ProgressWidget/ProgressWidget.h \
    BusinessLayer/Import/DocumentImporter.h \
    BusinessLayer/Export/DocxExporter.h \
    3rd_party/Widgets/ScalableWrapper/ScalableWrapper.h \
    BusinessLayer/Chronometry/PagesChronometer.h \
    BusinessLayer/Counters/Counter.h \
    3rd_party/Widgets/AcceptebleLineEdit/AcceptebleLineEdit.h \
    3rd_party/Delegates/ComboBoxItemDelegate/ComboBoxItemDelegate.h \
    3rd_party/Helpers/BackupHelper.h \
    3rd_party/Widgets/HierarchicalHeaderView/HierarchicalHeaderView.h \
    3rd_party/Widgets/HierarchicalHeaderView/HierarchicalTableModel.h \
    3rd_party/Widgets/FlatButton/FlatButton.h \
    3rd_party/Delegates/KeySequenceDelegate/KeySequenceDelegate.h \
    3rd_party/Helpers/ShortcutHelper.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEditPrivate.h \
    ManagementLayer/Synchronization/SynchronizationManager.h \
    UserInterfaceLayer/Settings/TemplateDialog.h \
    ManagementLayer/Settings/SettingsTemplatesManager.h \
    BusinessLayer/ScenarioDocument/ScenarioTemplate.h \
    UserInterfaceLayer/StartUp/LoginDialog.h \
    3rd_party/Helpers/PasswordStorage.h \
    ManagementLayer/Project/ProjectsManager.h \
    ManagementLayer/Project/Project.h \
    3rd_party/Helpers/QVariantMapWriter.h \
    3rd_party/Helpers/ImageHelper.h \
    DataLayer/DataMappingLayer/DatabaseHistoryMapper.h \
    DataLayer/DataStorageLayer/DatabaseHistoryStorage.h \
    Domain/ScenarioChange.h \
    DataLayer/DataMappingLayer/ScenarioChangeMapper.h \
    DataLayer/DataStorageLayer/ScenarioChangeStorage.h \
    3rd_party/Helpers/DiffMatchPatchHelper.h \
    3rd_party/Helpers/DiffMatchPatch.h \
    Domain/ScenarioData.h \
    DataLayer/DataStorageLayer/ScenarioDataStorage.h \
    DataLayer/DataMappingLayer/ScenarioDataMapper.h \
    3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h \
    3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.h \
    3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/NoprintableTextHandler.h \
    3rd_party/Widgets/ToolTipLabel/ToolTipLabel.h \
    BusinessLayer/ScenarioDocument/ScenarioReviewModel.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioReviewPanel.h \
    3rd_party/Widgets/ColoredToolButton/ColoredToolButton.h \
    3rd_party/Widgets/ColoredToolButton/ColorsPane.h \
    3rd_party/Widgets/ColoredToolButton/GoogleColorsPane.h \
    3rd_party/Widgets/ColoredToolButton/WordHighlightColorsPane.h \
    3rd_party/Widgets/QWidgetListView/qwidgetlistview.h \
    3rd_party/Widgets/QWidgetListView/qtmodelwidget.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioReviewView.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioReviewItemDelegate.h \
    DataLayer/Database/DatabaseHelper.h \
    3rd_party/Widgets/Ctk/ctkCollapsibleButton.h \
    UserInterfaceLayer/Statistics/StatisticsView.h \
    ManagementLayer/Statistics/StatisticsManager.h \
    3rd_party/Widgets/Ctk/ctkPopupWidget.h \
    3rd_party/Widgets/Ctk/ctkPopupWidget_p.h \
    3rd_party/Widgets/Ctk/ctkBasePopupWidget.h \
    3rd_party/Widgets/Ctk/ctkBasePopupWidget_p.h \
    3rd_party/Widgets/Ctk/ctkPimpl.h \
    UserInterfaceLayer/Statistics/StatisticsSettings.h \
    BusinessLayer/Statistics/AbstractReport.h \
    UserInterfaceLayer/Statistics/ReportButton.h \
    UserInterfaceLayer/Scenario/ScenarioTextEdit/Handlers/SceneHeadingHandler.h \
    BusinessLayer/Statistics/LocationReport.h \
    BusinessLayer/Statistics/SceneReport.h \
    BusinessLayer/Statistics/ReportFacade.h \
    BusinessLayer/Statistics/CastReport.h \
    BusinessLayer/Statistics/CharacterReport.h \
    BusinessLayer/Statistics/SummaryReport.h \
    3rd_party/Widgets/PopupWidget/PopupWidget.h \
    3rd_party/Widgets/PopupWidget/PopupWidget_p.h

FORMS += \
    UserInterfaceLayer/StartUp/StartUpView.ui \
    UserInterfaceLayer/Characters/CharactersDataEdit/CharactersDataEdit.ui \
    UserInterfaceLayer/Locations/LocationsDataEdit/LocationsDataEdit.ui \
    UserInterfaceLayer/Settings/SettingsView.ui \
    UserInterfaceLayer/Scenario/ScenarioDataEdit/ScenarioDataEdit.ui \
    UserInterfaceLayer/Export/ExportDialog.ui \
    UserInterfaceLayer/Import/ImportDialog.ui \
    UserInterfaceLayer/Settings/TemplateDialog.ui \
    UserInterfaceLayer/StartUp/LoginDialog.ui \
    UserInterfaceLayer/Statistics/StatisticsSettings.ui


RESOURCES += \
    Resources/Resources.qrc

OTHER_FILES += \
	settings_keys \
	scenario_settings_keys \
    logo.ico \
    Scenarist.rc

win32:RC_FILE = Scenarist.rc
macx {
    ICON = logo.icns
    QMAKE_INFO_PLIST = Info.plist
}

