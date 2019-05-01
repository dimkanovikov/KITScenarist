#-------------------------------------------------
#
# Project created by QtCreator 2014-01-26T12:01:05
#
#-------------------------------------------------

QT += core core-private gui gui-private sql xml widgets widgets-private printsupport network concurrent multimedia svg
!win32-g++: QT += webengine webenginewidgets

TARGET = Scenarist
TEMPLATE = app

CONFIG += c++11 warn_on
unix: QMAKE_CXXFLAGS_WARN_ON += -Werror

#
# Конфигурируем расположение файлов сборки
#
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../build/Debug/bin/scenarist-desktop
} else {
    DESTDIR = $$PWD/../../build/Release/bin/scenarist-desktop
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
#

#
# Включить профилирование
#
#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg
#

#
# Подключаем библиотеку HUNSPELL
#
LIBS += -L$$DESTDIR/../../libs/hunspell/ -lhunspell

INCLUDEPATH += $$PWD/../libs/hunspell/src
DEPENDPATH += $$PWD/../libs/hunspell
PRE_TARGETDEPS += $$PWD/../libs/hunspell
#

##
## Подключаем библиотеку MYTHES
##
#LIBS += -L$$DESTDIR/../../libs/mythes/ -lmythes

#INCLUDEPATH += $$PWD/../libs/mythes
#DEPENDPATH += $$PWD/../libs/mythes
##

#
# Подключаем библиотеку fileformats
#
LIBS += -L$$DESTDIR/../../libs/fileformats/ -lfileformats

INCLUDEPATH += $$PWD/../libs/fileformats
DEPENDPATH += $$PWD/../libs/fileformats
PRE_TARGETDEPS += $$PWD/../libs/fileformats
#

#
# Подключаем библилотеку WebLoader
#
LIBS += -L$$DESTDIR/../../libs/webloader/ -lwebloader

INCLUDEPATH += $$PWD/../libs/webloader/src
DEPENDPATH += $$PWD/../libs/webloader/src
PRE_TARGETDEPS += $$PWD/../libs/webloader/src
#

#
# Подключаем библилотеку QGumboParser
#
LIBS += -L$$DESTDIR/../../libs/qgumboparser/ -lqgumboparser

INCLUDEPATH += $$PWD/../libs/qgumboparser
DEPENDPATH += $$PWD/../libs/qgumboparser
PRE_TARGETDEPS += $$PWD/../libs/qgumboparser
#

unix {
LIBS += -lz
}
#

#
# Подключаем библиотеку qBreakpad и делаем настройки для неё
#
win32-msvc* {
CONFIG += debug_and_release warn_on
CONFIG += thread exceptions rtti stl

# Включить создание pdb-файла для релизной сборки
QMAKE_CXXFLAGS_RELEASE +=  /Zi
QMAKE_LFLAGS_RELEASE += /DEBUG /OPT:REF /OPT:ICF

QMAKE_LIBDIR += $$DESTDIR/../../libs/qBreakpad
LIBS += -L$$DESTDIR/../../libs/qBreakpad -lqBreakpad
PRE_TARGETDEPS += $$DESTDIR/../../libs/qBreakpad/qBreakpad.lib

INCLUDEPATH += $$PWD/../libs/qBreakpad/handler/
DEPENDPATH += $$PWD/../libs/qBreakpad/handler/

HEADERS += $$PWD/../libs/qBreakpad/handler/QBreakpadHandler.h
}
#

INCLUDEPATH += $$PWD/scenarist-core
INCLUDEPATH += $$PWD/scenarist-desktop

SOURCES += \
    scenarist-desktop/main.cpp \
    scenarist-desktop/ManagementLayer/ApplicationManager.cpp \
    scenarist-desktop/ManagementLayer/Scenario/ScenarioManager.cpp \
    scenarist-desktop/ManagementLayer/Scenario/ScenarioTextEditManager.cpp \
    scenarist-desktop/ManagementLayer/Scenario/ScenarioNavigatorManager.cpp \
    scenarist-desktop/ManagementLayer/Scenario/ScenarioCardsManager.cpp \
    scenarist-desktop/ManagementLayer/Settings/SettingsManager.cpp \
    scenarist-desktop/ManagementLayer/StartUp/StartUpManager.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigator.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioDocument.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioModel.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioModelItem.cpp \
    scenarist-core/BusinessLayer/Chronometry/CharactersChronometer.cpp \
    scenarist-core/BusinessLayer/Chronometry/ChronometerFacade.cpp \
    scenarist-core/BusinessLayer/Chronometry/ConfigurableChronometer.cpp \
    scenarist-core/DataLayer/Database/Database.cpp \
    scenarist-core/DataLayer/DataMappingLayer/AbstractMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/MapperFacade.cpp \
    scenarist-core/DataLayer/DataMappingLayer/PlaceMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioDayMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/SettingsMapper.cpp \
    scenarist-core/DataLayer/DataMappingLayer/TimeMapper.cpp \
    scenarist-core/DataLayer/DataStorageLayer/PlaceStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioDayStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/SettingsStorage.cpp \
    scenarist-core/DataLayer/DataStorageLayer/StorageFacade.cpp \
    scenarist-core/DataLayer/DataStorageLayer/TimeStorage.cpp \
    scenarist-core/Domain/DomainObject.cpp \
    scenarist-core/Domain/Identifier.cpp \
    scenarist-core/Domain/Place.cpp \
    scenarist-core/Domain/Scenario.cpp \
    scenarist-core/Domain/ScenarioDay.cpp \
    scenarist-core/Domain/SceneTime.cpp \
    scenarist-core/3rd_party/Widgets/CompletableTextEdit/CompletableTextEdit.cpp \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SpellChecker.cpp \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SpellCheckHighlighter.cpp \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorItemDelegate.cpp \
    scenarist-core/3rd_party/Widgets/ElidedLabel/ElidedLabel.cpp \
    scenarist-core/BusinessLayer/Export/PdfExporter.cpp \
    scenarist-core/BusinessLayer/Export/FountainExporter.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioXml.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTextDocument.cpp \
    scenarist-desktop/UserInterfaceLayer/StartUp/StartUpView.cpp \
    scenarist-core/3rd_party/Widgets/SideBar/SideBar.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEditWidget.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.cpp \
    scenarist-desktop/UserInterfaceLayer/Settings/SettingsView.cpp \
    scenarist-desktop/UserInterfaceLayer/Application/ApplicationView.cpp \
    scenarist-core/3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.cpp \
    scenarist-core/3rd_party/Widgets/PhotosChooser/PhotosChooser.cpp \
    scenarist-core/3rd_party/Widgets/PhotosChooser/PhotoLabel.cpp \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxwidget.cpp \
    scenarist-core/3rd_party/Widgets/PhotosChooser/PhotoPreview.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.cpp \
    scenarist-core/3rd_party/Widgets/TabBar/TabBar.cpp \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SyntaxHighlighter.cpp \
    scenarist-core/3rd_party/Widgets/SearchWidget/SearchWidget.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioFastFormatWidget.cpp \
    scenarist-core/3rd_party/Widgets/PagesTextEdit/PageMetrics.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorProxyStyle.cpp \
    scenarist-desktop/ManagementLayer/Export/ExportManager.cpp \
    scenarist-desktop/UserInterfaceLayer/Export/ExportDialog.cpp \
    scenarist-core/Domain/CharacterState.cpp \
    scenarist-core/DataLayer/DataMappingLayer/CharacterStateMapper.cpp \
    scenarist-core/DataLayer/DataStorageLayer/CharacterStateStorage.cpp \
    scenarist-core/BusinessLayer/Export/AbstractExporter.cpp \
    scenarist-core/BusinessLayer/Counters/CountersFacade.cpp \
    scenarist-desktop/Application.cpp \
    scenarist-desktop/ManagementLayer/Import/ImportManager.cpp \
    scenarist-desktop/UserInterfaceLayer/Import/ImportDialog.cpp \
    scenarist-core/BusinessLayer/Import/DocumentImporter.cpp \
    scenarist-core/BusinessLayer/Import/FountainImporter.cpp \
    scenarist-core/BusinessLayer/Export/DocxExporter.cpp \
    scenarist-core/3rd_party/Widgets/ScalableWrapper/ScalableWrapper.cpp \
    scenarist-core/BusinessLayer/Chronometry/PagesChronometer.cpp \
    scenarist-core/3rd_party/Widgets/AcceptebleLineEdit/AcceptebleLineEdit.cpp \
    scenarist-core/3rd_party/Delegates/ComboBoxItemDelegate/ComboBoxItemDelegate.cpp \
    scenarist-core/3rd_party/Helpers/BackupHelper.cpp \
    scenarist-core/3rd_party/Widgets/HierarchicalHeaderView/HierarchicalHeaderView.cpp \
    scenarist-core/3rd_party/Widgets/HierarchicalHeaderView/HierarchicalTableModel.cpp \
    scenarist-core/3rd_party/Widgets/FlatButton/FlatButton.cpp \
    scenarist-core/3rd_party/Delegates/KeySequenceDelegate/KeySequenceDelegate.cpp \
    scenarist-desktop/UserInterfaceLayer/Settings/TemplateDialog.cpp \
    scenarist-desktop/ManagementLayer/Settings/SettingsTemplatesManager.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTemplate.cpp \
    scenarist-desktop/UserInterfaceLayer/Account/LoginDialog.cpp \
    scenarist-core/ManagementLayer/Project/ProjectsManager.cpp \
    scenarist-core/ManagementLayer/Project/Project.cpp \
    scenarist-core/DataLayer/DataMappingLayer/DatabaseHistoryMapper.cpp \
    scenarist-core/DataLayer/DataStorageLayer/DatabaseHistoryStorage.cpp \
    scenarist-core/Domain/ScenarioChange.cpp \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioChangeMapper.cpp \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioChangeStorage.cpp \
    scenarist-core/3rd_party/Helpers/DiffMatchPatch.cpp \
    scenarist-core/Domain/ScenarioData.cpp \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioDataStorage.cpp \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioDataMapper.cpp \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.cpp \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.cpp \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.cpp \
    scenarist-core/3rd_party/Widgets/ToolTipLabel/ToolTipLabel.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioReviewModel.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioReviewPanel.cpp \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/ColoredToolButton.cpp \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/GoogleColorsPane.cpp \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/WordHighlightColorsPane.cpp \
    scenarist-core/3rd_party/Widgets/QWidgetListView/qwidgetlistview.cpp \
    scenarist-core/3rd_party/Widgets/QWidgetListView/qtmodelwidget.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioReviewView.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioReviewItemDelegate.cpp \
    scenarist-desktop/UserInterfaceLayer/Statistics/StatisticsView.cpp \
    scenarist-desktop/ManagementLayer/Statistics/StatisticsManager.cpp \
    scenarist-desktop/UserInterfaceLayer/Statistics/StatisticsSettings.cpp \
    scenarist-core/BusinessLayer/Statistics/Reports/LocationReport.cpp \
    scenarist-core/BusinessLayer/Statistics/Reports/SceneReport.cpp \
    scenarist-core/BusinessLayer/Statistics/Reports/CastReport.cpp \
    scenarist-core/BusinessLayer/Statistics/Reports/CharacterReport.cpp \
    scenarist-core/BusinessLayer/Statistics/Reports/SummaryReport.cpp \
    scenarist-desktop/UserInterfaceLayer/Settings/LanguageDialog.cpp \
    scenarist-core/DataLayer/DataMappingLayer/ResearchMapper.cpp \
    scenarist-core/Domain/Research.cpp \
    scenarist-core/DataLayer/DataStorageLayer/ResearchStorage.cpp \
    scenarist-desktop/UserInterfaceLayer/Research/ResearchView.cpp \
    scenarist-desktop/ManagementLayer/Research/ResearchManager.cpp \
    scenarist-core/BusinessLayer/Research/ResearchModel.cpp \
    scenarist-core/BusinessLayer/Research/ResearchModelItem.cpp \
    scenarist-desktop/UserInterfaceLayer/Research/ResearchItemDialog.cpp \
    scenarist-core/3rd_party/Widgets/QCutomPlot/qcustomplot.cpp \
    scenarist-core/BusinessLayer/Statistics/Plots/StoryStructureAnalisysPlot.cpp \
    scenarist-core/BusinessLayer/Statistics/StatisticsFacade.cpp \
    scenarist-core/3rd_party/Widgets/QCutomPlot/qcustomplotextended.cpp \
    scenarist-core/BusinessLayer/Statistics/Plots/CharactersActivityPlot.cpp \
    scenarist-core/3rd_party/Widgets/PagesTextEdit/PageTextEdit.cpp \
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
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneHeadingHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/StandardKeyHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/TitleHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/TransitionHandler.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioLineEdit.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEdit.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEditHelpers.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEditPrivate.cpp \
    scenarist-core/3rd_party/Widgets/ImagesPane/AddImageButton.cpp \
    scenarist-core/3rd_party/Widgets/ImagesPane/ImagesPane.cpp \
    scenarist-core/3rd_party/Widgets/ImagesPane/ImagePreview.cpp \
    scenarist-core/3rd_party/Widgets/ImagesPane/FlowLayout.cpp \
    scenarist-core/3rd_party/Widgets/ImagesPane/ImageLabel.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneDescriptionHandler.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioSceneDescription/ScenarioSceneDescription.cpp \
    scenarist-desktop/ManagementLayer/Scenario/ScenarioSceneDescriptionManager.cpp \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.cpp \
    scenarist-core/3rd_party/Widgets/SimpleTextEditor/SimpleTextEditorWidget.cpp \
    scenarist-core/3rd_party/Widgets/QtMindMap/src/commands.cpp \
    scenarist-core/3rd_party/Widgets/QtMindMap/src/edge.cpp \
    scenarist-core/3rd_party/Widgets/QtMindMap/src/graphlogic.cpp \
    scenarist-core/3rd_party/Widgets/QtMindMap/src/graphwidget.cpp \
    scenarist-core/3rd_party/Widgets/QtMindMap/src/node.cpp \
    scenarist-core/3rd_party/Widgets/TabBarExpanded/TabBarExpanded.cpp \
    scenarist-core/3rd_party/Widgets/PasswordLineEdit/PasswordLineEdit.cpp \
    scenarist-desktop/UserInterfaceLayer/Account/ChangePasswordDialog.cpp \
    scenarist-desktop/UserInterfaceLayer/Account/RenewSubscriptionDialog.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioCards/ScenarioCardsView.cpp \
    scenarist-core/BusinessLayer/Export/FdxExporter.cpp \
    scenarist-core/BusinessLayer/Import/FdxImporter.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioCards/CardsResizer.cpp \
    scenarist-desktop/UserInterfaceLayer/_tools/UIConfigurator.cpp \
    scenarist-core/BusinessLayer/Import/TrelbyImporter.cpp \
    scenarist-desktop/UserInterfaceLayer/Project/AddProjectDialog.cpp \
    scenarist-core/BusinessLayer/Import/KitScenaristImporter.cpp \
    scenarist-desktop/UserInterfaceLayer/Project/ShareDialog.cpp \
    scenarist-desktop/UserInterfaceLayer/Application/CrashReportDialog.cpp \
    scenarist-core/3rd_party/Widgets/WAF/Animation/CircleFill/CircleFillAnimator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/Animation/CircleFill/CircleFillDecorator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/Animation/SideSlide/SideSlideAnimator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/Animation/SideSlide/SideSlideDecorator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/Animation/Slide/SlideAnimator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/Animation/Slide/SlideForegroundDecorator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/Animation/Animation.cpp \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetFadeIn/StackedWidgetFadeInAnimator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetFadeIn/StackedWidgetFadeInDecorator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetSlide/StackedWidgetSlideAnimator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetSlide/StackedWidgetSlideDecorator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetSlideOver/StackedWidgetSlideOverAnimator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetSlideOver/StackedWidgetSlideOverDecorator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetAnimation.cpp \
    scenarist-core/3rd_party/Widgets/WAF/Animation/Expand/ExpandAnimator.cpp \
    scenarist-core/3rd_party/Widgets/WAF/Animation/Expand/ExpandDecorator.cpp \
    scenarist-core/ManagementLayer/Synchronization/SynchronizationManager.cpp \
    scenarist-desktop/UserInterfaceLayer/Application/UpdateDialog.cpp \
    scenarist-core/3rd_party/Widgets/CardsEdit/ActItem.cpp \
    scenarist-core/3rd_party/Widgets/CardsEdit/CardItem.cpp \
    scenarist-core/3rd_party/Widgets/CardsEdit/CardsScene.cpp \
    scenarist-core/3rd_party/Widgets/CardsEdit/CardsUndoStack.cpp \
    scenarist-core/3rd_party/Widgets/CardsEdit/CardsView.cpp \
    scenarist-core/3rd_party/Widgets/CardsEdit/ScalableGraphicsView.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioCards/PrintCardsDialog.cpp \
    scenarist-core/UserInterfaceLayer/ProjectsList/ProjectFileWidget.cpp \
    scenarist-core/UserInterfaceLayer/ProjectsList/ProjectsList.cpp \
    scenarist-core/UserInterfaceLayer/ProjectsList/ProjectUserWidget.cpp \
    scenarist-core/3rd_party/Delegates/TreeViewItemDelegate/TreeViewItemDelegate.cpp \
    scenarist-core/3rd_party/Styles/TreeViewProxyStyle/TreeViewProxyStyle.cpp \
    scenarist-desktop/UserInterfaceLayer/Onboarding/OnboardingView.cpp \
    scenarist-desktop/ManagementLayer/Onboarding/OnboardingManager.cpp \
    scenarist-core/3rd_party/Helpers/RunOnce.cpp \
    scenarist-core/3rd_party/Widgets/QProgressIndicator/QProgressIndicator.cpp \
    scenarist-core/3rd_party/Widgets/Stepper/Stepper.cpp \
    scenarist-core/Domain/Transition.cpp \
    scenarist-core/DataLayer/DataMappingLayer/TransitionMapper.cpp \
    scenarist-core/DataLayer/DataStorageLayer/TransitionStorage.cpp \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/LyricsHandler.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioTextEdit/ScriptZenModeControls.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScriptTextCorrector.cpp \
    scenarist-desktop/ManagementLayer/Scenario/ScriptDictionariesManager.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScriptDictionaries/ScriptDictionaries.cpp \
    scenarist-desktop/UserInterfaceLayer/Application/MenuView.cpp \
    scenarist-core/3rd_party/Widgets/ClickableLabel/ClickableLabel.cpp \
    scenarist-desktop/ManagementLayer/MenuManager.cpp \
    scenarist-core/3rd_party/Widgets/ClickableLabel/ClickableFrame.cpp \
    scenarist-core/BusinessLayer/Import/CeltxImporter.cpp \
    scenarist-desktop/UserInterfaceLayer/Application/AboutDialog.cpp \
    scenarist-desktop/ManagementLayer/Tools/ToolsManager.cpp \
    scenarist-desktop/UserInterfaceLayer/Tools/ToolsView.cpp \
    scenarist-desktop/UserInterfaceLayer/Tools/ToolsSettings.cpp \
    scenarist-core/BusinessLayer/Tools/RestoreFromBackupTool.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScriptBookmarksModel.cpp \
    scenarist-desktop/ManagementLayer/Scenario/ScriptBookmarksManager.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScriptBookmarks/ScriptBookmarks.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScriptBookmarks/BookmarkDialog.cpp \
    scenarist-core/Domain/ScriptVersion.cpp \
    scenarist-core/DataLayer/DataMappingLayer/ScriptVersionMapper.cpp \
    scenarist-core/DataLayer/DataStorageLayer/ScriptVersionStorage.cpp \
    scenarist-desktop/UserInterfaceLayer/Project/ProjectVersionDialog.cpp \
    scenarist-core/BusinessLayer/ScenarioDocument/ScriptTextCursor.cpp \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/ColorsPane.cpp \
    scenarist-core/UserInterfaceLayer/ScriptVersions/ScriptVersionWidget.cpp \
    scenarist-core/UserInterfaceLayer/ScriptVersions/ScriptVersionsList.cpp \
    scenarist-core/BusinessLayer/Tools/CompareScriptVersionsTool.cpp \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioItemDialog/ScenarioItemDialog.cpp \
    scenarist-core/3rd_party/Widgets/CircularProgressBar/CircularProgressBar.cpp

HEADERS += \
    scenarist-desktop/ManagementLayer/ApplicationManager.h \
    scenarist-desktop/ManagementLayer/Scenario/ScenarioManager.h \
    scenarist-desktop/ManagementLayer/Scenario/ScenarioTextEditManager.h \
    scenarist-desktop/ManagementLayer/Scenario/ScenarioNavigatorManager.h \
    scenarist-desktop/ManagementLayer/Scenario/ScenarioCardsManager.h \
    scenarist-desktop/ManagementLayer/Settings/SettingsManager.h \
    scenarist-desktop/ManagementLayer/StartUp/StartUpManager.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigator.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioDocument.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioModel.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioModelItem.h \
    scenarist-core/BusinessLayer/Chronometry/AbstractChronometer.h \
    scenarist-core/BusinessLayer/Chronometry/CharactersChronometer.h \
    scenarist-core/BusinessLayer/Chronometry/ChronometerFacade.h \
    scenarist-core/BusinessLayer/Chronometry/ConfigurableChronometer.h \
    scenarist-core/DataLayer/Database/Database.h \
    scenarist-core/DataLayer/DataMappingLayer/AbstractMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/MapperFacade.h \
    scenarist-core/DataLayer/DataMappingLayer/PlaceMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioDayMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/SettingsMapper.h \
    scenarist-core/DataLayer/DataMappingLayer/TimeMapper.h \
    scenarist-core/DataLayer/DataStorageLayer/PlaceStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioDayStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/SettingsStorage.h \
    scenarist-core/DataLayer/DataStorageLayer/StorageFacade.h \
    scenarist-core/DataLayer/DataStorageLayer/TimeStorage.h \
    scenarist-core/Domain/DomainObject.h \
    scenarist-core/Domain/Identifier.h \
    scenarist-core/Domain/Place.h \
    scenarist-core/Domain/Scenario.h \
    scenarist-core/Domain/ScenarioDay.h \
    scenarist-core/Domain/SceneTime.h \
    scenarist-core/3rd_party/Widgets/CompletableTextEdit/CompletableTextEdit.h \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SpellChecker.h \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SpellCheckHighlighter.h \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.h \
    scenarist-core/UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorItemDelegate.h \
    scenarist-core/3rd_party/Widgets/ElidedLabel/ElidedLabel.h \
    scenarist-core/BusinessLayer/Export/AbstractExporter.h \
    scenarist-core/BusinessLayer/Export/PdfExporter.h \
    scenarist-core/BusinessLayer/Export/FountainExporter.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioXml.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTextDocument.h \
    scenarist-desktop/UserInterfaceLayer/StartUp/StartUpView.h \
    scenarist-core/3rd_party/Widgets/SideBar/SideBar.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEditWidget.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h \
    scenarist-desktop/UserInterfaceLayer/Settings/SettingsView.h \
    scenarist-desktop/UserInterfaceLayer/Application/ApplicationView.h \
    scenarist-core/3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.h \
    scenarist-core/3rd_party/Widgets/PhotosChooser/PhotosChooser.h \
    scenarist-core/3rd_party/Widgets/PhotosChooser/PhotoLabel.h \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxwidget.h \
    scenarist-core/3rd_party/Widgets/PhotosChooser/PhotoPreview.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h \
    scenarist-core/3rd_party/Widgets/TabBar/TabBar.h \
    scenarist-core/3rd_party/Widgets/SpellCheckTextEdit/SyntaxHighlighter.h \
    scenarist-core/3rd_party/Widgets/SearchWidget/SearchWidget.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioFastFormatWidget.h \
    scenarist-core/3rd_party/Widgets/PagesTextEdit/PageMetrics.h \
    scenarist-core/3rd_party/Helpers/TextEditHelper.h \
    scenarist-core/UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorProxyStyle.h \
    scenarist-desktop/ManagementLayer/Export/ExportManager.h \
    scenarist-desktop/UserInterfaceLayer/Export/ExportDialog.h \
    scenarist-core/Domain/CharacterState.h \
    scenarist-core/DataLayer/DataMappingLayer/CharacterStateMapper.h \
    scenarist-core/DataLayer/DataStorageLayer/CharacterStateStorage.h \
    scenarist-core/BusinessLayer/Counters/CountersFacade.h \
    scenarist-desktop/Application.h \
    scenarist-core/BusinessLayer/Import/AbstractImporter.h \
    scenarist-desktop/ManagementLayer/Import/ImportManager.h \
    scenarist-desktop/UserInterfaceLayer/Import/ImportDialog.h \
    scenarist-core/BusinessLayer/Import/DocumentImporter.h \
    scenarist-core/BusinessLayer/Import/FountainImporter.h \
    scenarist-core/BusinessLayer/Export/DocxExporter.h \
    scenarist-core/3rd_party/Widgets/ScalableWrapper/ScalableWrapper.h \
    scenarist-core/BusinessLayer/Chronometry/PagesChronometer.h \
    scenarist-core/BusinessLayer/Counters/Counter.h \
    scenarist-core/3rd_party/Widgets/AcceptebleLineEdit/AcceptebleLineEdit.h \
    scenarist-core/3rd_party/Delegates/ComboBoxItemDelegate/ComboBoxItemDelegate.h \
    scenarist-core/3rd_party/Helpers/BackupHelper.h \
    scenarist-core/3rd_party/Widgets/HierarchicalHeaderView/HierarchicalHeaderView.h \
    scenarist-core/3rd_party/Widgets/HierarchicalHeaderView/HierarchicalTableModel.h \
    scenarist-core/3rd_party/Widgets/FlatButton/FlatButton.h \
    scenarist-core/3rd_party/Delegates/KeySequenceDelegate/KeySequenceDelegate.h \
    scenarist-core/3rd_party/Helpers/ShortcutHelper.h \
    scenarist-desktop/UserInterfaceLayer/Settings/TemplateDialog.h \
    scenarist-desktop/ManagementLayer/Settings/SettingsTemplatesManager.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioTemplate.h \
    scenarist-desktop/UserInterfaceLayer/Account/LoginDialog.h \
    scenarist-core/3rd_party/Helpers/PasswordStorage.h \
    scenarist-core/ManagementLayer/Project/ProjectsManager.h \
    scenarist-core/ManagementLayer/Project/Project.h \
    scenarist-core/3rd_party/Helpers/QVariantMapWriter.h \
    scenarist-core/3rd_party/Helpers/ImageHelper.h \
    scenarist-core/DataLayer/DataMappingLayer/DatabaseHistoryMapper.h \
    scenarist-core/DataLayer/DataStorageLayer/DatabaseHistoryStorage.h \
    scenarist-core/Domain/ScenarioChange.h \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioChangeMapper.h \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioChangeStorage.h \
    scenarist-core/3rd_party/Helpers/DiffMatchPatchHelper.h \
    scenarist-core/3rd_party/Helpers/DiffMatchPatch.h \
    scenarist-core/Domain/ScenarioData.h \
    scenarist-core/DataLayer/DataStorageLayer/ScenarioDataStorage.h \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioDataMapper.h \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.h \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h \
    scenarist-core/3rd_party/Widgets/ToolTipLabel/ToolTipLabel.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScenarioReviewModel.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioReviewPanel.h \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/ColoredToolButton.h \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/ColorsPane.h \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/GoogleColorsPane.h \
    scenarist-core/3rd_party/Widgets/ColoredToolButton/WordHighlightColorsPane.h \
    scenarist-core/3rd_party/Widgets/QWidgetListView/qwidgetlistview.h \
    scenarist-core/3rd_party/Widgets/QWidgetListView/qtmodelwidget.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioReviewView.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioReviewItemDelegate.h \
    scenarist-core/DataLayer/Database/DatabaseHelper.h \
    scenarist-desktop/UserInterfaceLayer/Statistics/StatisticsView.h \
    scenarist-desktop/ManagementLayer/Statistics/StatisticsManager.h \
    scenarist-desktop/UserInterfaceLayer/Statistics/StatisticsSettings.h \
    scenarist-core/BusinessLayer/Statistics/Reports/AbstractReport.h \
    scenarist-core/BusinessLayer/Statistics/Reports/LocationReport.h \
    scenarist-core/BusinessLayer/Statistics/Reports/SceneReport.h \
    scenarist-core/BusinessLayer/Statistics/Reports/CastReport.h \
    scenarist-core/BusinessLayer/Statistics/Reports/CharacterReport.h \
    scenarist-core/BusinessLayer/Statistics/Reports/SummaryReport.h \
    scenarist-desktop/UserInterfaceLayer/Settings/LanguageDialog.h \
    scenarist-core/DataLayer/DataMappingLayer/ResearchMapper.h \
    scenarist-core/Domain/Research.h \
    scenarist-core/DataLayer/DataStorageLayer/ResearchStorage.h \
    scenarist-desktop/UserInterfaceLayer/Research/ResearchView.h \
    scenarist-desktop/ManagementLayer/Research/ResearchManager.h \
    scenarist-core/BusinessLayer/Research/ResearchModel.h \
    scenarist-core/BusinessLayer/Research/ResearchModelItem.h \
    scenarist-desktop/UserInterfaceLayer/Research/ResearchItemDialog.h \
    scenarist-core/3rd_party/Helpers/ScrollerHelper.h \
    scenarist-core/3rd_party/Helpers/StyleSheetHelper.h \
    scenarist-core/3rd_party/Widgets/QCutomPlot/qcustomplot.h \
    scenarist-core/BusinessLayer/Statistics/Plots/AbstractPlot.h \
    scenarist-core/BusinessLayer/Statistics/StatisticsParameters.h \
    scenarist-core/BusinessLayer/Statistics/Plots/StoryStructureAnalisysPlot.h \
    scenarist-core/BusinessLayer/Statistics/StatisticsFacade.h \
    scenarist-core/3rd_party/Widgets/QCutomPlot/qcustomplotextended.h \
    scenarist-core/BusinessLayer/Statistics/Plots/CharactersActivityPlot.h \
    scenarist-core/3rd_party/Widgets/PagesTextEdit/PageTextEdit.h \
    scenarist-core/3rd_party/Widgets/PagesTextEdit/PageTextEdit_p.h \
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
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneHeadingHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/StandardKeyHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/TitleHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/TitleHeaderHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/TransitionHandler.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioLineEdit.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEdit.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEditHelpers.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEditPrivate.h \
    scenarist-core/3rd_party/Widgets/ImagesPane/ImagesPane.h \
    scenarist-core/3rd_party/Widgets/ImagesPane/ImagePreview.h \
    scenarist-core/3rd_party/Widgets/ImagesPane/AddImageButton.h \
    scenarist-core/3rd_party/Widgets/ImagesPane/FlowLayout.h \
    scenarist-core/3rd_party/Widgets/ImagesPane/ImageLabel.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/SceneDescriptionHandler.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioSceneDescription/ScenarioSceneDescription.h \
    scenarist-desktop/ManagementLayer/Scenario/ScenarioSceneDescriptionManager.h \
    scenarist-core/3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h \
    scenarist-core/3rd_party/Helpers/FileHelper.h \
    scenarist-core/3rd_party/Widgets/SimpleTextEditor/SimpleTextEditorWidget.h \
    scenarist-core/3rd_party/Widgets/QtMindMap/include/commands.h \
    scenarist-core/3rd_party/Widgets/QtMindMap/include/edge.h \
    scenarist-core/3rd_party/Widgets/QtMindMap/include/graphlogic.h \
    scenarist-core/3rd_party/Widgets/QtMindMap/include/graphwidget.h \
    scenarist-core/3rd_party/Widgets/QtMindMap/include/node.h \
    scenarist-core/3rd_party/Widgets/TabBarExpanded/TabBarExpanded.h \
    scenarist-core/3rd_party/Widgets/PasswordLineEdit/PasswordLineEdit.h \
    scenarist-desktop/UserInterfaceLayer/Account/ChangePasswordDialog.h \
    scenarist-desktop/UserInterfaceLayer/Account/RenewSubscriptionDialog.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioCards/ScenarioCardsView.h \
    scenarist-core/BusinessLayer/Export/FdxExporter.h \
    scenarist-core/BusinessLayer/Import/FdxImporter.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioCards/CardsResizer.h \
    scenarist-desktop/UserInterfaceLayer/_tools/UIConfigurator.h \
    scenarist-core/BusinessLayer/Import/TrelbyImporter.h \
    scenarist-desktop/UserInterfaceLayer/Project/AddProjectDialog.h \
    scenarist-core/BusinessLayer/Import/KitScenaristImporter.h \
    scenarist-desktop/UserInterfaceLayer/Project/ShareDialog.h \
    scenarist-core/3rd_party/Helpers/Validators.h \
    scenarist-desktop/UserInterfaceLayer/Application/CrashReportDialog.h \
    scenarist-core/ManagementLayer/Synchronization/Sync.h \
    scenarist-core/3rd_party/Widgets/WAF/Animation/CircleFill/CircleFillAnimator.h \
    scenarist-core/3rd_party/Widgets/WAF/Animation/CircleFill/CircleFillDecorator.h \
    scenarist-core/3rd_party/Widgets/WAF/Animation/SideSlide/SideSlideAnimator.h \
    scenarist-core/3rd_party/Widgets/WAF/Animation/SideSlide/SideSlideDecorator.h \
    scenarist-core/3rd_party/Widgets/WAF/Animation/Slide/SlideAnimator.h \
    scenarist-core/3rd_party/Widgets/WAF/Animation/Slide/SlideForegroundDecorator.h \
    scenarist-core/3rd_party/Widgets/WAF/Animation/Animation.h \
    scenarist-core/3rd_party/Widgets/WAF/Animation/AnimationPrivate.h \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetFadeIn/StackedWidgetFadeInAnimator.h \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetFadeIn/StackedWidgetFadeInDecorator.h \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetSlide/StackedWidgetSlideAnimator.h \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetSlide/StackedWidgetSlideDecorator.h \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetSlideOver/StackedWidgetSlideOverAnimator.h \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetSlideOver/StackedWidgetSlideOverDecorator.h \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetAnimation.h \
    scenarist-core/3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetAnimationPrivate.h \
    scenarist-core/3rd_party/Widgets/WAF/WAF.h \
    scenarist-core/3rd_party/Widgets/WAF/Animation/Expand/ExpandAnimator.h \
    scenarist-core/3rd_party/Widgets/WAF/Animation/Expand/ExpandDecorator.h \
    scenarist-core/3rd_party/Widgets/WAF/AbstractAnimator.h \
    scenarist-core/ManagementLayer/Synchronization/SynchronizationManager.h \
    scenarist-core/3rd_party/Helpers/ColorHelper.h \
    scenarist-desktop/UserInterfaceLayer/Application/UpdateDialog.h \
    scenarist-core/3rd_party/Widgets/CardsEdit/ActItem.h \
    scenarist-core/3rd_party/Widgets/CardsEdit/CardItem.h \
    scenarist-core/3rd_party/Widgets/CardsEdit/CardsScene.h \
    scenarist-core/3rd_party/Widgets/CardsEdit/CardsUndoStack.h \
    scenarist-core/3rd_party/Widgets/CardsEdit/CardsView.h \
    scenarist-core/3rd_party/Widgets/CardsEdit/ScalableGraphicsView.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioCards/PrintCardsDialog.h \
    scenarist-core/3rd_party/Helpers/TextUtils.h \
    scenarist-core/3rd_party/Helpers/StyleHelper.h \
    scenarist-core/UserInterfaceLayer/ProjectsList/ProjectFileWidget.h \
    scenarist-core/UserInterfaceLayer/ProjectsList/ProjectsList.h \
    scenarist-core/UserInterfaceLayer/ProjectsList/ProjectUserWidget.h \
    scenarist-core/3rd_party/Delegates/TreeViewItemDelegate/TreeViewItemDelegate.h \
    scenarist-core/3rd_party/Styles/TreeViewProxyStyle/TreeViewProxyStyle.h \
    scenarist-desktop/UserInterfaceLayer/Onboarding/OnboardingView.h \
    scenarist-desktop/ManagementLayer/Onboarding/OnboardingManager.h \
    scenarist-core/3rd_party/Helpers/RunOnce.h \
    scenarist-core/3rd_party/Widgets/QProgressIndicator/QProgressIndicator.h \
    scenarist-core/3rd_party/Widgets/Stepper/Stepper.h \
    scenarist-core/Domain/Transition.h \
    scenarist-core/DataLayer/DataMappingLayer/TransitionMapper.h \
    scenarist-core/DataLayer/DataStorageLayer/TransitionStorage.h \
    scenarist-core/UserInterfaceLayer/ScenarioTextEdit/Handlers/LyricsHandler.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioTextEdit/ScriptZenModeControls.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScriptTextCorrector.h \
    scenarist-core/DataLayer/DataMappingLayer/ScenarioMapper.h \
    scenarist-desktop/ManagementLayer/Scenario/ScriptDictionariesManager.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScriptDictionaries/ScriptDictionaries.h \
    scenarist-desktop/UserInterfaceLayer/Application/MenuView.h \
    scenarist-core/3rd_party/Widgets/ClickableLabel/ClickableLabel.h \
    scenarist-desktop/ManagementLayer/MenuManager.h \
    scenarist-core/3rd_party/Widgets/ClickableLabel/ClickableFrame.h \
    scenarist-core/BusinessLayer/Import/CeltxImporter.h \
    scenarist-desktop/UserInterfaceLayer/Application/AboutDialog.h \
    scenarist-desktop/ManagementLayer/Tools/ToolsManager.h \
    scenarist-desktop/UserInterfaceLayer/Tools/ToolsView.h \
    scenarist-desktop/UserInterfaceLayer/Tools/ToolsSettings.h \
    scenarist-core/BusinessLayer/Tools/RestoreFromBackupTool.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScriptBookmarksModel.h \
    scenarist-desktop/ManagementLayer/Scenario/ScriptBookmarksManager.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScriptBookmarks/ScriptBookmarks.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScriptBookmarks/BookmarkDialog.h \
    scenarist-core/Domain/ScriptVersion.h \
    scenarist-core/DataLayer/DataMappingLayer/ScriptVersionMapper.h \
    scenarist-core/DataLayer/DataStorageLayer/ScriptVersionStorage.h \
    scenarist-desktop/UserInterfaceLayer/Project/ProjectVersionDialog.h \
    scenarist-core/BusinessLayer/ScenarioDocument/ScriptTextCursor.h \
    scenarist-core/UserInterfaceLayer/ScriptVersions/ScriptVersionWidget.h \
    scenarist-core/UserInterfaceLayer/ScriptVersions/ScriptVersionsList.h \
    scenarist-core/BusinessLayer/Tools/CompareScriptVersionsTool.h \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioItemDialog/ScenarioItemDialog.h \
    scenarist-core/3rd_party/Widgets/CircularProgressBar/CircularProgressBar.h

FORMS += \
    scenarist-desktop/UserInterfaceLayer/StartUp/StartUpView.ui \
    scenarist-desktop/UserInterfaceLayer/Settings/SettingsView.ui \
    scenarist-desktop/UserInterfaceLayer/Export/ExportDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Import/ImportDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Settings/TemplateDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Account/LoginDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Settings/LanguageDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Research/ResearchView.ui \
    scenarist-desktop/UserInterfaceLayer/Research/ResearchItemDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Account/ChangePasswordDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Account/RenewSubscriptionDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioCards/CardsResizer.ui \
    scenarist-desktop/UserInterfaceLayer/_tools/UIConfigurator.ui \
    scenarist-desktop/UserInterfaceLayer/Project/AddProjectDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Project/ShareDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Application/CrashReportDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Application/UpdateDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioCards/PrintCardsDialog.ui \
    scenarist-core/UserInterfaceLayer/ProjectsList/ProjectFileWidget.ui \
    scenarist-core/UserInterfaceLayer/ProjectsList/ProjectUserWidget.ui \
    scenarist-desktop/UserInterfaceLayer/Onboarding/OnboardingView.ui \
    scenarist-desktop/UserInterfaceLayer/Application/MenuView.ui \
    scenarist-desktop/UserInterfaceLayer/Application/AboutDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Tools/ToolsSettings.ui \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScriptBookmarks/BookmarkDialog.ui \
    scenarist-desktop/UserInterfaceLayer/Statistics/StatisticsSettings.ui \
    scenarist-desktop/UserInterfaceLayer/Project/ProjectVersionDialog.ui \
    scenarist-core/UserInterfaceLayer/ScriptVersions/ScriptVersionWidget.ui \
    scenarist-desktop/UserInterfaceLayer/Scenario/ScenarioItemDialog/ScenarioItemDialog.ui


RESOURCES += \
    scenarist-core/Resources/Resources.qrc \
    scenarist-core/Resources/Translations.qrc

OTHER_FILES += \
    scenarist-desktop/settings_keys \
    scenarist-desktop/scenario_settings_keys \
    scenarist-desktop/logo.ico \
    scenarist-desktop/Scenarist.rc

win32:RC_FILE = scenarist-desktop/Scenarist.rc
macx {
    ICON = scenarist-desktop/logo.icns
    QMAKE_INFO_PLIST = scenarist-desktop/Info.plist
}

#
# Включаем поддержку компиляции под Windows XP для MSVC 2012+
#
win32-msvc*:QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01

#
# Настраиваем вывод в консоль в Windows в нормальной кодировке
#
win32 {
    QMAKE_EXTRA_TARGETS += before_build makefilehook

    makefilehook.target = $(MAKEFILE)
    makefilehook.depends = .beforebuild

    PRE_TARGETDEPS += .beforebuild

    before_build.target = .beforebuild
    before_build.depends = FORCE
    before_build.commands = chcp 1251
}
