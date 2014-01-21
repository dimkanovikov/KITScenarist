#include "ApplicationWidget.h"

#include "Widgets/SideBar/SideBar.h"
#include "StartUp/StartUpWidget.h"
#include "ScenarioEditor/ScenarioEditorWidget.h"
#include "CharactersEditor/CharactersEditorWidget.h"
#include "LocationsEditor/LocationsEditorWidget.h"
#include "SettingsEditor/SettingsEditorWidget.h"

#include <Storage/StorageFacade.h>
#include <Storage/SettingsStorage.h>
#include <Storage/ScenarioStorage.h>

#include <Database/DatabaseHelper.h>

#include <Domain/Scenario.h>

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>
#include <BusinessLogic/ScenarioTextEdit/ScenarioXml.h>

#include <QToolButton>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVariant>
#include <QFile>
#include <QMenu>
#include <QStackedLayout>
#include <QFileDialog>


ApplicationWidget::ApplicationWidget(QWidget *parent) :
	QWidget(parent),
	m_tabs(0),
	m_tabsWidgets(0),
	m_startUp(0),
	m_scenarioEditor(0),
	m_charactersEditor(0),
	m_locationsEditor(0),
	m_settingsEditor(0),
	m_menu(0)
{
	initView();
	initConnections();
	initStyleSheet();
}

void ApplicationWidget::aboutCreateNewScenario()
{
	StorageLayer::StorageFacade::clearStorages();
	DatabaseLayer::DatabaseHelper::loadDatabaseFromFile(QString::null);
	scenarioEditor()->scenarioTextEdit()->clear();

	scenarioEditor()->scenarioTextEdit()->changeScenarioBlockType(ScenarioTextBlockStyle::Action);
	scenarioEditor()->scenarioTextEdit()->changeScenarioBlockType(ScenarioTextBlockStyle::TimeAndPlace);
}

void ApplicationWidget::aboutOpenScenario()
{
	QString fileName =
			QFileDialog::getOpenFileName(
				this,
				tr("Open Scenario"),
				StorageLayer::StorageFacade::settingsStorage()->value(
					"application/file-path",
					StorageLayer::SettingsStorage::ApplicationSettings),
				"");

	if (!fileName.isEmpty()) {
		StorageLayer::StorageFacade::settingsStorage()->setValue(
					"application/file-path",
					fileName,
					StorageLayer::SettingsStorage::ApplicationSettings);


		StorageLayer::StorageFacade::clearStorages();
		DatabaseLayer::DatabaseHelper::loadDatabaseFromFile(fileName);
		scenarioEditor()->scenarioTextEdit()->clear();
		if (Domain::Scenario* scenario = StorageLayer::StorageFacade::scenarioStorage()->current()) {
			ScenarioXmlReader::xmlToScenario(scenario->text(), scenarioEditor()->scenarioTextEdit());
		}
	}
}

void ApplicationWidget::aboutSaveScenario()
{
	QString fileName =
			QFileDialog::getSaveFileName(
				this,
				tr("Save Scenario"),
				StorageLayer::StorageFacade::settingsStorage()->value(
					"application/file-path",
					StorageLayer::SettingsStorage::ApplicationSettings),
				"");

	if (!fileName.isEmpty()) {
		StorageLayer::StorageFacade::settingsStorage()->setValue(
					"application/file-path",
					fileName,
					StorageLayer::SettingsStorage::ApplicationSettings);

		StorageLayer::StorageFacade::scenarioStorage()->storeScenario(ScenarioXmlWriter::scenarioToXml(scenarioEditor()->scenarioTextEdit()));
		DatabaseLayer::DatabaseHelper::saveDatabaseToFile(fileName);
	}
}

void ApplicationWidget::initView()
{
	QVBoxLayout* leftLayout = new QVBoxLayout;
	leftLayout->setContentsMargins(QMargins());
	leftLayout->setSpacing(0);

	leftLayout->addWidget(menu());
	leftLayout->addWidget(tabs());

//	tabsWidgets()->addWidget(startUp());
	tabsWidgets()->addWidget(scenarioEditor());
	tabsWidgets()->addWidget(charactersEditor());
	tabsWidgets()->addWidget(locationsEditor());
	tabsWidgets()->addWidget(settingsEditor());

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);

	layout->addLayout(leftLayout);
	layout->addLayout(tabsWidgets());

	this->setLayout(layout);
}

void ApplicationWidget::initConnections()
{
	connect(menu(), SIGNAL(clicked()), menu(), SLOT(showMenu()));
	connect(tabs(), SIGNAL(currentChanged(int)), tabsWidgets(), SLOT(setCurrentIndex(int)));

	//
	// Действия меню
	//
	connect(m_menuCreateNew, SIGNAL(triggered()), this, SLOT(aboutCreateNewScenario()));
	connect(m_menuOpen, SIGNAL(triggered()), this, SLOT(aboutOpenScenario()));
	connect(m_menuSave, SIGNAL(triggered()), this, SLOT(aboutSaveScenario()));

	//
	// Действия вкладок
	//
	connect(settingsEditor(), SIGNAL(useSpellCheckingChanged(bool)), scenarioEditor()->scenarioTextEdit(), SLOT(aboutUseSpellChecker(bool)));
}

void ApplicationWidget::initStyleSheet()
{
	menu()->setProperty("menuButton", true);
	menu()->setProperty("topPanelTopBordered", true);
	menu()->setProperty("topPanelRightBordered", true);

	//
	// Считываем стиль из ресурсов
	//
	QFile styleSheetFile(":/Interface/UI/ScenarioEditor/style.qss");
	styleSheetFile.open(QIODevice::ReadOnly);
	QString styleSheet = styleSheetFile.readAll();

	//
	// Применяем стиль
	//
	setStyleSheet(styleSheet);
}

QToolButton* ApplicationWidget::menu()
{
	if (m_menu == 0) {
		m_menu = new QToolButton(this);
		m_menu->setText(tr("Scenarius"));
		m_menu->setPopupMode(QToolButton::MenuButtonPopup);
		m_menu->setCheckable(true);

		QMenu* menu = new QMenu;
		m_menuCreateNew = menu->addAction(tr("New"));
//		m_menuCreateNew->setShortcut(QKeySequence::Save);
		m_menuOpen = menu->addAction(tr("Open"));
//		m_menuOpen->setShortcut(QKeySequence::Open);
		QMenu* saveMenu = new QMenu;
		m_menuSave = saveMenu->addAction(tr("Save Scenario"));
//		m_menuSave->setShortcut(QKeySequence::Save);
		m_menuSaveAs = saveMenu->addAction(tr("Save Scenario As..."));
		m_menuSaveAsPdf = saveMenu->addAction(tr("Save To Pdf..."));
		m_menuSaveAsRtf = saveMenu->addAction(tr("Save To Rtf..."));
		QAction* saveMenuGroup = menu->addAction(tr("Save"));
		saveMenuGroup->setMenu(saveMenu);
		m_menuExit = menu->addAction(tr("Exit"));
//		m_menuExit->setShortcut(QKeySequence::Quit);

		m_menu->setMenu(menu);
	}
	return m_menu;
}

SideBar* ApplicationWidget::tabs()
{
	if (m_tabs == 0) {
		m_tabs = new SideBar(this);
		m_tabs->setCursor(QCursor(Qt::PointingHandCursor));

//		m_tabs->addAction(tr("Start"), QIcon(":/Graphics/Icons/start.png"));
//		m_tabs->addAction(tr("Project"), QIcon(":/Graphics/Icons/project.png"));
		m_tabs->addAction(tr("Script"), QIcon(":/Graphics/Icons/script.png"));
		m_tabs->addAction(tr("Characters"), QIcon(":/Graphics/Icons/characters.png"));
		m_tabs->addAction(tr("Locations"), QIcon(":/Graphics/Icons/locations.png"));
		m_tabs->addAction(tr("Settings"), QIcon(":/Graphics/Icons/settings.png"));
	}
	return m_tabs;
}

QStackedLayout* ApplicationWidget::tabsWidgets()
{
	if (m_tabsWidgets == 0) {
		m_tabsWidgets = new QStackedLayout;
	}
	return m_tabsWidgets;
}

StartUpWidget* ApplicationWidget::startUp()
{
	if (m_startUp == 0) {
		m_startUp = new StartUpWidget(this);
	}
	return m_startUp;
}

ScenarioEditorWidget* ApplicationWidget::scenarioEditor()
{
	if (m_scenarioEditor == 0) {
		m_scenarioEditor = new ScenarioEditorWidget(this);
	}
	return m_scenarioEditor;
}

CharactersEditorWidget* ApplicationWidget::charactersEditor()
{
	if (m_charactersEditor == 0) {
		m_charactersEditor = new CharactersEditorWidget(this);
	}
	return m_charactersEditor;
}

LocationsEditorWidget* ApplicationWidget::locationsEditor()
{
	if (m_locationsEditor == 0) {
		m_locationsEditor = new LocationsEditorWidget(this);
	}
	return m_locationsEditor;
}

SettingsEditorWidget* ApplicationWidget::settingsEditor()
{
	if (m_settingsEditor == 0) {
		m_settingsEditor= new SettingsEditorWidget(this);
	}
	return m_settingsEditor;
}
