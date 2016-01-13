#-------------------------------------------------
#
# Project created by QtCreator 2014-01-26T12:01:05
#
#-------------------------------------------------

QT += core core-private gui gui-private sql xml widgets widgets-private network
android: QT += androidextras

TARGET = Scenarist
TEMPLATE = app

CONFIG += c++11

DEFINES += MOBILE_OS NO_ANIMATIONS

#
# Отключаем предупреждения о статических функциях и переменных
# не используемых в собственных заголовочных файлах
#
unix: QMAKE_CXXFLAGS += -Wno-unused-function -Wno-unused-variable

QMAKE_MAC_SDK = macosx10.11

#
# Включить профилирование
#
#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg
#

#
# Конфигурируем расположение файлов сборки
#
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../build/Debug/bin/scenarist-mobile
} else {
    DESTDIR = $$PWD/../../build/Release/bin/scenarist-mobile
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
#

#
# Подключаем библиотеку HUNSPELL
#
win32:CONFIG(release, debug|release): LIBS += -L$$DESTDIR/../../libs/hunspell/release/ -lhunspell
else:win32:CONFIG(debug, debug|release): LIBS += -L$$DESTDIR/../../libs/hunspell/debug/ -lhunspell
else:unix: LIBS += -L$$DESTDIR/../../libs/hunspell/ -lhunspell

INCLUDEPATH += $$PWD/../libs/hunspell/src
DEPENDPATH += $$PWD/../libs/hunspell
#

#
# Подключаем библилотеку WebLoader
#
win32:CONFIG(release, debug|release): LIBS += -L$$DESTDIR/../../libs/webloader/release/ -lwebloader
else:win32:CONFIG(debug, debug|release): LIBS += -L$$DESTDIR/../../libs/webloader/debug/ -lwebloader
else:unix: LIBS += -L$$DESTDIR/../../libs/webloader/ -lwebloader

INCLUDEPATH += $$PWD/../libs/webloader
DEPENDPATH += $$PWD/../libs/webloader
#

INCLUDEPATH += $$PWD/scenarist-core
INCLUDEPATH += $$PWD/scenarist-mobile

SOURCES += \
    scenarist-mobile/main.cpp \
    scenarist-mobile/Application.cpp \
    scenarist-mobile/ManagementLayer/ApplicationManager.cpp \
    scenarist-core/3rd_party/Delegates/ComboBoxItemDelegate/ComboBoxItemDelegate.cpp \
    scenarist-core/3rd_party/Delegates/KeySequenceDelegate/KeySequenceDelegate.cpp \
    scenarist-core/3rd_party/Helpers/BackupHelper.cpp \
    scenarist-core/3rd_party/Helpers/DiffMatchPatch.cpp \
    scenarist-core/3rd_party/Widgets/AcceptebleLineEdit/AcceptebleLineEdit.cpp \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/ColoredToolButton.cpp \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/GoogleColorsPane.cpp \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/WordHighlightColorsPane.cpp \
    scenarist-core/3rd_party/Widgets/CompletableTextEdit/CompletableTextEdit.cpp \
    scenarist-core/3rd_party/Widgets/Ctk/ctkBasePopupWidget.cpp \
    scenarist-core/3rd_party/Widgets/Ctk/ctkCollapsibleButton.cpp \
    scenarist-core/3rd_party/Widgets/Ctk/ctkPopupWidget.cpp \
    scenarist-core/3rd_party/Widgets/ElidedLabel/ElidedLabel.cpp \
    scenarist-core/3rd_party/Widgets/FlatButton/FlatButton.cpp \
    scenarist-core/3rd_party/Widgets/HierarchicalHeaderView/HierarchicalHeaderView.cpp \
    scenarist-core/3rd_party/Widgets/HierarchicalHeaderView/HierarchicalTableModel.cpp \
    scenarist-core/3rd_party/Widgets/PagesTextEdit/PageMetrics.cpp \
    scenarist-core/3rd_party/Widgets/PagesTextEdit/PageTextEdit.cpp \
    scenarist-core/3rd_party/Widgets/PhotosChooser/PhotoLabel.cpp \
    scenarist-core/3rd_party/Widgets/PhotosChooser/PhotoPreview.cpp \
    scenarist-core/3rd_party/Widgets/PhotosChooser/PhotosChooser.cpp \
    scenarist-core/3rd_party/Widgets/ProgressWidget/ProgressWidget.cpp \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.cpp \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.cpp \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.cpp \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxwidget.cpp \
    scenarist-core/3rd_party/Widgets/QWidgetListView/qtmodelwidget.cpp \
    scenarist-core/3rd_party/Widgets/QWidgetListView/qwidgetlistview.cpp \
    scenarist-core/3rd_party/Widgets/ScalableWrapper/ScalableWrapper.cpp \
    scenarist-core/3rd_party/Widgets/SearchWidget/SearchWidget.cpp \
    scenarist-core/3rd_party/Widgets/SideBar/SideBar.cpp \
    scenarist-core/3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.cpp \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SpellChecker.cpp \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SpellCheckHighlighter.cpp \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.cpp \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SyntaxHighlighter.cpp \
    scenarist-core/3rd_party/Widgets/TabBar/TabBar.cpp \
    scenarist-core/3rd_party/Widgets/ToolTipLabel/ToolTipLabel.cpp \
    scenarist-core/BusinessLayer/Chronometry/CharactersChronometer.cpp \
    scenarist-core/BusinessLayer/Chronometry/ChronometerFacade.cpp \
    scenarist-core/BusinessLayer/Chronometry/ConfigurableChronometer.cpp \
    scenarist-core/BusinessLayer/Chronometry/PagesChronometer.cpp \
    scenarist-core/BusinessLayer/Counters/CountersFacade.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioDocument.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioModel.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioModelItem.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioReviewModel.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTemplate.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTextDocument.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioXml.cpp \
    scenarist-core/DataLayer/Database/Database.cpp \
    scenarist-core/DataLayer/DataMappingLayer/AbstractMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/CharacterMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/CharacterPhotoMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/CharacterStateMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/DatabaseHistoryMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/LocationMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/LocationPhotoMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/MapperFacade.cpp \
    scenarist-core/DataLayer/DataMappingLayer/PlaceMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioChangeMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioDataMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioDayMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/SettingsMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/TimeMapper.cpp \
    scenarist-core/DataLayer/DataStorageLayer/CharacterPhotoStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/CharacterStateStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/CharacterStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/DatabaseHistoryStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/LocationPhotoStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/LocationStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/PlaceStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioChangeStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioDataStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioDayStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/SettingsStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/StorageFacade.cpp \
    scenarist-core/DataLayer/DataStorageLayer/TimeStorage.cpp \
    scenarist-core/Domain/Character.cpp \
    scenarist-core/Domain/CharacterPhoto.cpp \
    scenarist-core/Domain/CharacterState.cpp \
    scenarist-core/Domain/DomainObject.cpp \
    scenarist-core/Domain/Identifier.cpp \
    scenarist-core/Domain/Location.cpp \
    scenarist-core/Domain/LocationPhoto.cpp \
    scenarist-core/Domain/Place.cpp \
    scenarist-core/Domain/Scenario.cpp \
    scenarist-core/Domain/ScenarioChange.cpp \
    scenarist-core/Domain/ScenarioData.cpp \
    scenarist-core/Domain/ScenarioDay.cpp \
    scenarist-core/Domain/Time.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/AbstractKeyHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/ActionHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/CharacterHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/DialogHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/FolderFooterHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/FolderHeaderHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/KeyPressHandlerFacade.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/NoprintableTextHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/NoteHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/ParentheticalHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/PreHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/PrepareHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneCharactersHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneGroupFooterHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneGroupHeaderHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneHeadingHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/StandardKeyHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/TitleHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/TransitionHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioLineEdit.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEdit.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEditHelpers.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEditPrivate.cpp \
    scenarist-mobile/UserInterfaceLayer/ApplicationView.cpp \
    scenarist-mobile/ManagementLayer/Menu/MenuManager.cpp \
    scenarist-mobile/UserInterfaceLayer/Menu/MenuView.cpp \
    scenarist-mobile/ManagementLayer/StartUp/StartUpManager.cpp \
    scenarist-mobile/UserInterfaceLayer/StartUp/RecentFilesDelegate.cpp \
    scenarist-mobile/UserInterfaceLayer/StartUp/RecentFileWidget.cpp \
    scenarist-mobile/UserInterfaceLayer/StartUp/StartUpView.cpp \
    scenarist-core/3rd_party/Widgets/WAF/Animation.cpp \
    scenarist-core/3rd_party/Widgets/WAF/SideSlide/SideSlideBackgroundDecorator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/SideSlide/SideSlideAnimator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/Slide/SlideBackgroundDecorator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/Slide/SlideAnimator.cpp \
    scenarist-mobile/ManagementLayer/Scenario/ScenarioManager.cpp \
    scenarist-mobile/UserInterfaceLayer/Scenario/ScenarioTextView.cpp \
    scenarist-mobile/UserInterfaceLayer/StartUp/AddProjectDialog.cpp \
    scenarist-mobile/ManagementLayer/Project/ProjectsManager.cpp \
    scenarist-mobile/ManagementLayer/Scenario/ScenarioNavigatorManager.cpp \
    scenarist-mobile/ManagementLayer/Scenario/ScenarioTextEditManager.cpp \
    scenarist-mobile/UserInterfaceLayer/Scenario/ScenarioNavigatorView.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorItemDelegate.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorProxyStyle.cpp \
    scenarist-mobile/ManagementLayer/Synchronization/SynchronizationManager.cpp \
    scenarist-core/DataLayer/DataMappingLayer/ResearchMapper.cpp \
    scenarist-core/DataLayer/DataStorageLayer/ResearchStorage.cpp \
    scenarist-core/Domain/Research.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneDescriptionHandler.cpp \
    scenarist-mobile/UserInterfaceLayer/Cabin/LoginView.cpp \
    scenarist-mobile/ManagementLayer/Cabin/LoginManager.cpp \
    scenarist-mobile/ManagementLayer/Settings/SettingsManager.cpp \
    scenarist-mobile/UserInterfaceLayer/Settings/SettingsView.cpp \
    scenarist-mobile/UserInterfaceLayer/Settings/LanguageDialog.cpp \
    scenarist-mobile/UserInterfaceLayer/Cabin/CabinView.cpp \
    scenarist-mobile/ManagementLayer/Cabin/CabinManager.cpp \
    scenarist-core/ManagementLayer/Project/Project.cpp

HEADERS += \
    scenarist-mobile/Application.h \
    scenarist-mobile/ManagementLayer/ApplicationManager.h \
    scenarist-core/3rd_party/Delegates/ComboBoxItemDelegate/ComboBoxItemDelegate.h \
    scenarist-core/3rd_party/Delegates/KeySequenceDelegate/KeySequenceDelegate.h \
    scenarist-core/3rd_party/Helpers/BackupHelper.h \
    scenarist-core/3rd_party/Helpers/DiffMatchPatch.h \
    scenarist-core/3rd_party/Helpers/DiffMatchPatchHelper.h \
    scenarist-core/3rd_party/Helpers/ImageHelper.h \
    scenarist-core/3rd_party/Helpers/PasswordStorage.h \
    scenarist-core/3rd_party/Helpers/QVariantMapWriter.h \
    scenarist-core/3rd_party/Helpers/ShortcutHelper.h \
    scenarist-core/3rd_party/Helpers/TextEditHelper.h \
    scenarist-core/3rd_party/Widgets/AcceptebleLineEdit/AcceptebleLineEdit.h \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/ColoredToolButton.h \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/ColorsPane.h \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/GoogleColorsPane.h \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/WordHighlightColorsPane.h \
    scenarist-core/3rd_party/Widgets/CompletableTextEdit/CompletableTextEdit.h \
    scenarist-core/3rd_party/Widgets/Ctk/ctkBasePopupWidget.h \
    scenarist-core/3rd_party/Widgets/Ctk/ctkBasePopupWidget_p.h \
    scenarist-core/3rd_party/Widgets/Ctk/ctkCollapsibleButton.h \
    scenarist-core/3rd_party/Widgets/Ctk/ctkPimpl.h \
    scenarist-core/3rd_party/Widgets/Ctk/ctkPopupWidget.h \
    scenarist-core/3rd_party/Widgets/Ctk/ctkPopupWidget_p.h \
    scenarist-core/3rd_party/Widgets/ElidedLabel/ElidedLabel.h \
    scenarist-core/3rd_party/Widgets/FlatButton/FlatButton.h \
    scenarist-core/3rd_party/Widgets/HierarchicalHeaderView/HierarchicalHeaderView.h \
    scenarist-core/3rd_party/Widgets/HierarchicalHeaderView/HierarchicalTableModel.h \
    scenarist-core/3rd_party/Widgets/PagesTextEdit/PageMetrics.h \
    scenarist-core/3rd_party/Widgets/PagesTextEdit/PageTextEdit.h \
    scenarist-core/3rd_party/Widgets/PagesTextEdit/PageTextEdit_p.h \
    scenarist-core/3rd_party/Widgets/PhotosChooser/PhotoLabel.h \
    scenarist-core/3rd_party/Widgets/PhotosChooser/PhotoPreview.h \
    scenarist-core/3rd_party/Widgets/PhotosChooser/PhotosChooser.h \
    scenarist-core/3rd_party/Widgets/ProgressWidget/ProgressWidget.h \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.h \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxwidget.h \
    scenarist-core/3rd_party/Widgets/QWidgetListView/qtmodelwidget.h \
    scenarist-core/3rd_party/Widgets/QWidgetListView/qwidgetlistview.h \
    scenarist-core/3rd_party/Widgets/ScalableWrapper/ScalableWrapper.h \
    scenarist-core/3rd_party/Widgets/SearchWidget/SearchWidget.h \
    scenarist-core/3rd_party/Widgets/SideBar/SideBar.h \
    scenarist-core/3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.h \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SpellChecker.h \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SpellCheckHighlighter.h \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.h \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SyntaxHighlighter.h \
    scenarist-core/3rd_party/Widgets/TabBar/TabBar.h \
    scenarist-core/3rd_party/Widgets/ToolTipLabel/ToolTipLabel.h \
    scenarist-core/BusinessLayer/Chronometry/AbstractChronometer.h \
    scenarist-core/BusinessLayer/Chronometry/CharactersChronometer.h \
    scenarist-core/BusinessLayer/Chronometry/ChronometerFacade.h \
    scenarist-core/BusinessLayer/Chronometry/ConfigurableChronometer.h \
    scenarist-core/BusinessLayer/Chronometry/PagesChronometer.h \
    scenarist-core/BusinessLayer/Counters/Counter.h \
    scenarist-core/BusinessLayer/Counters/CountersFacade.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioDocument.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioModel.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioModelItem.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioReviewModel.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTemplate.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTextDocument.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioXml.h \
    scenarist-core/DataLayer/Database/Database.h \
    scenarist-core/DataLayer/Database/DatabaseHelper.h \
    scenarist-core/DataLayer/DataMappingLayer/AbstractMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/CharacterMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/CharacterPhotoMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/CharacterStateMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/DatabaseHistoryMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/LocationMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/LocationPhotoMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/MapperFacade.h \
    scenarist-core/DataLayer/DataMappingLayer/PlaceMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioChangeMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioDataMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioDayMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/SettingsMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/TimeMapper.h \
    scenarist-core/DataLayer/DataStorageLayer/CharacterPhotoStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/CharacterStateStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/CharacterStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/DatabaseHistoryStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/LocationPhotoStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/LocationStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/PlaceStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioChangeStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioDataStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioDayStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/SettingsStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/StorageFacade.h \
    scenarist-core/DataLayer/DataStorageLayer/TimeStorage.h \
    scenarist-core/Domain/Character.h \
    scenarist-core/Domain/CharacterPhoto.h \
    scenarist-core/Domain/CharacterState.h \
    scenarist-core/Domain/DomainObject.h \
    scenarist-core/Domain/Identifier.h \
    scenarist-core/Domain/Location.h \
    scenarist-core/Domain/LocationPhoto.h \
    scenarist-core/Domain/Place.h \
    scenarist-core/Domain/Scenario.h \
    scenarist-core/Domain/ScenarioChange.h \
    scenarist-core/Domain/ScenarioData.h \
    scenarist-core/Domain/ScenarioDay.h \
    scenarist-core/Domain/Time.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/AbstractKeyHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/ActionHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/CharacterHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/DialogHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/FolderFooterHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/FolderHeaderHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/KeyPressHandlerFacade.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/NoprintableTextHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/NoteHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/ParentheticalHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/PreHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/PrepareHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneCharactersHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneGroupFooterHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneGroupHeaderHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneHeadingHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/StandardKeyHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/TitleHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/TitleHeaderHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/TransitionHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioLineEdit.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEdit.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEditHelpers.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEditPrivate.h \
    scenarist-mobile/UserInterfaceLayer/ApplicationView.h \
    scenarist-mobile/ManagementLayer/Menu/MenuManager.h \
    scenarist-mobile/UserInterfaceLayer/Menu/MenuView.h \
    scenarist-mobile/ManagementLayer/StartUp/StartUpManager.h \
    scenarist-mobile/UserInterfaceLayer/StartUp/RecentFilesDelegate.h \
    scenarist-mobile/UserInterfaceLayer/StartUp/RecentFileWidget.h \
    scenarist-mobile/UserInterfaceLayer/StartUp/StartUpView.h \
    scenarist-core/3rd_party/Widgets/WAF/AbstractAnimator.h \
    scenarist-core/3rd_party/Widgets/WAF/Animation.h \
    scenarist-core/3rd_party/Widgets/WAF/AnimationPrivate.h \
    scenarist-core/3rd_party/Widgets/WAF/SideSlide/SideSlideBackgroundDecorator.h \
    scenarist-core/3rd_party/Widgets/WAF/SideSlide/SideSlideAnimator.h \
    scenarist-core/3rd_party/Widgets/WAF/Slide/SlideBackgroundDecorator.h \
    scenarist-core/3rd_party/Widgets/WAF/Slide/SlideAnimator.h \
    scenarist-mobile/ManagementLayer/Scenario/ScenarioManager.h \
    scenarist-mobile/UserInterfaceLayer/Scenario/ScenarioTextView.h \
    scenarist-core/3rd_party/Helpers/StyleSheetHelper.h \
    scenarist-mobile/UserInterfaceLayer/StartUp/AddProjectDialog.h \
    scenarist-mobile/ManagementLayer/Project/ProjectsManager.h \
    scenarist-mobile/ManagementLayer/Scenario/ScenarioNavigatorManager.h \
    scenarist-mobile/ManagementLayer/Scenario/ScenarioTextEditManager.h \
    scenarist-mobile/UserInterfaceLayer/Scenario/ScenarioNavigatorView.h \
    scenarist-core/UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorItemDelegate.h \
    scenarist-core/UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorProxyStyle.h \
    scenarist-mobile/ManagementLayer/Synchronization/SynchronizationManager.h \
    scenarist-core/3rd_party/Helpers/ScrollerHelper.h \
    scenarist-core/DataLayer/DataMappingLayer/ResearchMapper.h \
    scenarist-core/DataLayer/DataStorageLayer/ResearchStorage.h \
    scenarist-core/Domain/Research.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneDescriptionHandler.h \
    scenarist-mobile/UserInterfaceLayer/Cabin/LoginView.h \
    scenarist-mobile/ManagementLayer/Cabin/LoginManager.h \
    scenarist-mobile/ManagementLayer/Settings/SettingsManager.h \
    scenarist-mobile/UserInterfaceLayer/Settings/SettingsView.h \
    scenarist-mobile/UserInterfaceLayer/Settings/LanguageDialog.h \
    scenarist-mobile/UserInterfaceLayer/Cabin/CabinView.h \
    scenarist-mobile/ManagementLayer/Cabin/CabinManager.h \
    scenarist-core/ManagementLayer/Project/Project.h


RESOURCES += \
    scenarist-core/Resources/Resources.qrc

OTHER_FILES += \
    scenarist-mobile/settings_keys \
    scenarist-mobile/scenario_settings_keys

FORMS += \
    scenarist-mobile/UserInterfaceLayer/ApplicationView.ui \
    scenarist-mobile/UserInterfaceLayer/Menu/MenuView.ui \
    scenarist-mobile/UserInterfaceLayer/StartUp/StartUpView.ui \
    scenarist-mobile/UserInterfaceLayer/Scenario/ScenarioTextView.ui \
    scenarist-mobile/UserInterfaceLayer/StartUp/AddProjectDialog.ui \
    scenarist-mobile/UserInterfaceLayer/Scenario/ScenarioNavigatorView.ui \
    scenarist-mobile/UserInterfaceLayer/Cabin/LoginView.ui \
    scenarist-mobile/UserInterfaceLayer/Settings/SettingsView.ui \
    scenarist-mobile/UserInterfaceLayer/Settings/LanguageDialog.ui \
    scenarist-mobile/UserInterfaceLayer/Cabin/CabinView.ui

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
	/Users/macuser/dev/dimkanovikov/Scenarist/src/bin/../../build/Release/libs/hunspell/libhunspell.so \
	$$PWD/../../build/Release/libs/webloader/libwebloader.so
}

DISTFILES += \
    Scenarist-mobile/AndroidManifest.xml \
    Scenarist-mobile/gradle/wrapper/gradle-wrapper.jar \
    Scenarist-mobile/gradlew \
    Scenarist-mobile/res/values/libs.xml \
    Scenarist-mobile/build.gradle \
    Scenarist-mobile/gradle/wrapper/gradle-wrapper.properties \
    Scenarist-mobile/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Scenarist-mobile

OBJECTIVE_SOURCES +=

