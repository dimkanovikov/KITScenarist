#include "ApplicationManager.h"

#include "StartUp/StartUpManager.h"
#include "Scenario/ScenarioManager.h"
#include "Characters/CharactersManager.h"
#include "Locations/LocationsManager.h"
#include "Settings/SettingsManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/Export/PdfExporter.h>
#include <BusinessLayer/Export/RtfExporter.h>

#include <DataLayer/Database/Database.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/SideBar/SideBar.h>

#include <UserInterfaceLayer/ApplicationView.h>

#include <QApplication>
#include <QStackedLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolButton>
#include <QMenu>

using namespace ManagementLayer;
using UserInterface::ApplicationView;

namespace {
	const QString PROJECT_FILE_EXTENSION = ".kitsp"; // kit scenarist project

	/**
	 * @brief Неактивные при старте действия
	 */
	QList<QAction*> g_disableOnStartActions;

	/**
	 * @brief Отключить некоторые действия
	 *
	 * Используется при старте приложения, пока не загружен какой-либо проект
	 */
	static void disableActionsOnStart() {
		foreach (QAction* action, g_disableOnStartActions) {
			action->setEnabled(false);
		}
	}

	/**
	 * @brief Активировать отключенные при старте действия
	 */
	static void enableActionsOnProjectOpen() {
		foreach (QAction* action, g_disableOnStartActions) {
			action->setEnabled(true);
		}
		g_disableOnStartActions.clear();
	}
}


ApplicationManager::ApplicationManager(QObject *parent) :
	QObject(parent),
	m_view(new ApplicationView),
	m_menu(new QToolButton(m_view)),
	m_tabs(new SideTabBar(m_view)),
	m_tabsWidgets(new QStackedLayout),
	m_startUpManager(new StartUpManager(this, m_view)),
	m_scenarioManager(new ScenarioManager(this, m_view)),
	m_charactersManager(new CharactersManager(this, m_view)),
	m_locationsManager(new LocationsManager(this, m_view)),
	m_settingsManager(new SettingsManager(this, m_view))
{
	initView();
	initConnections();
}

ApplicationManager::~ApplicationManager()
{
	saveIfNeeded();

	delete m_view;
	m_view = 0;
}

void ApplicationManager::exec()
{
	loadViewState();
	m_view->show();
}

void ApplicationManager::aboutCreateNew()
{
	//
	// Если нужно сохранить проект
	//
	saveIfNeeded();

	//
	// Получим имя файла для нового проекта
	//
	QString newProjectFileName =
			QFileDialog::getSaveFileName(
				m_view,
				tr("Choose file for new project"),
				QString(),
				tr ("Scenarist project files (*%1)").arg(PROJECT_FILE_EXTENSION)
				);

	//
	// Если файл выбран
	//
	if (!newProjectFileName.isEmpty()) {
		//
		// ... установим расширение, если не задано
		//
		if (!newProjectFileName.endsWith(PROJECT_FILE_EXTENSION)) {
			newProjectFileName.append(PROJECT_FILE_EXTENSION);
		}

		//
		// ... очистим все загруженные на текущий момент данные
		//
		DataStorageLayer::StorageFacade::clearStorages();

		//
		// ... если файл существовал, удалим его для удаления данных в нём
		//
		if (QFile::exists(newProjectFileName)) {
			QFile::remove(newProjectFileName);
		}

		//
		// ... создаём новую базу данных в файле
		//
		DatabaseLayer::Database::setCurrentFile(newProjectFileName);

		//
		// ... перейдём к редактированию
		//
		goToEditCurrentProject();
	}
}

void ApplicationManager::aboutSaveAs()
{
	//
	// Получим имя файла для сохранения
	//
	QString saveAsProjectFileName =
			QFileDialog::getSaveFileName(
				m_view,
				tr("Choose file for save project"),
				QString(),
				tr ("Scenarist project files (*%1)").arg(PROJECT_FILE_EXTENSION)
				);

	//
	// Если файл выбран
	//
	if (!saveAsProjectFileName.isEmpty()) {
		//
		// ... установим расширение, если не задано
		//
		if (!saveAsProjectFileName.endsWith(PROJECT_FILE_EXTENSION)) {
			saveAsProjectFileName.append(PROJECT_FILE_EXTENSION);
		}

		//
		// ... если пользователь хочет просто пересохранить проект
		//
		if (saveAsProjectFileName == DatabaseLayer::Database::currentFile()) {
			aboutSave();
		}
		//
		// ... если сохраняем в новый файл
		//
		else {
			//
			// ... если файл существовал, удалим его для удаления данных в нём
			//
			if (QFile::exists(saveAsProjectFileName)) {
				QFile::remove(saveAsProjectFileName);
			}

			//
			// ... скопируем текущую базу в указанный файл
			//
			QFile::copy(DatabaseLayer::Database::currentFile(), saveAsProjectFileName);

			//
			// ... переключаемся на использование другого файла
			//
			DatabaseLayer::Database::setCurrentFile(saveAsProjectFileName);

			//
			// ... сохраняем изменения
			//
			aboutSave();
		}
	}
}

void ApplicationManager::aboutSave()
{
	//
	// Управляющие должны сохранить несохранённые данные
	//
	m_scenarioManager->saveCurrentProject();

	//
	// Добавим проект к недавно используемым
	//
	saveCurrentProjectInRecent();

	//
	// Изменим статус окна на сохранение изменений
	//
	m_view->setWindowModified(false);
}

void ApplicationManager::aboutLoad(const QString& _fileName)
{
	//
	// Если нужно сохранить проект
	//
	saveIfNeeded();

	//
	// Имя файла для загрузки
	//
	QString loadProjectFileName = _fileName;

	//
	// Если имя файла не определено, выберем его в диалоге выбора файла
	//
	if (loadProjectFileName.isEmpty()) {
		loadProjectFileName =
				QFileDialog::getOpenFileName(
					m_view,
					tr("Choose project file to open"),
					QString(),
					tr ("Scenarist project files (*%1)").arg(PROJECT_FILE_EXTENSION)
					);
	}

	//
	// Если файл выбран
	//
	if (!loadProjectFileName.isEmpty()) {
		//
		// ... очистим все загруженные на текущий момент данные
		//
		DataStorageLayer::StorageFacade::clearStorages();

		//
		// ... переключаемся на работу с выбранным файлом
		//
		DatabaseLayer::Database::setCurrentFile(loadProjectFileName);

		//
		// ... перейдём к редактированию
		//
		goToEditCurrentProject();
	}
}

void ApplicationManager::aboutExportToPdf()
{
	const QString PDF_EXTENSION = ".pdf";

	//
	// Получим имя файла для сохранения
	//
	QString exportFileName =
			QFileDialog::getSaveFileName(
				m_view,
				tr("Choose file to export scenario"),
				QString(),
				tr ("PDF files (*%1)").arg(PDF_EXTENSION)
				);

	//
	// Если файл выбран
	//
	if (!exportFileName.isEmpty()) {
		//
		// ... установим расширение, если не задано
		//
		if (!exportFileName.endsWith(PDF_EXTENSION)) {
			exportFileName.append(PDF_EXTENSION);
		}

		//
		// ... экспортируем сценарий
		//
		BusinessLogic::PdfExporter exporter;
		exporter.exportTo(m_scenarioManager->scenario()->document(), exportFileName);
	}
}

void ApplicationManager::aboutExportToRtf()
{
	const QString RTF_EXTENSION = ".rtf";

	//
	// Получим имя файла для сохранения
	//
	QString exportFileName =
			QFileDialog::getSaveFileName(
				m_view,
				tr("Choose file to export scenario"),
				QString(),
				tr ("RTF files (*%1)").arg(RTF_EXTENSION)
				);

	//
	// Если файл выбран
	//
	if (!exportFileName.isEmpty()) {
		//
		// ... установим расширение, если не задано
		//
		if (!exportFileName.endsWith(RTF_EXTENSION)) {
			exportFileName.append(RTF_EXTENSION);
		}

		//
		// ... экспортируем сценарий
		//
		BusinessLogic::RtfExporter exporter;
		exporter.exportTo(m_scenarioManager->scenario()->document(), exportFileName);
	}
}

void ApplicationManager::aboutExit()
{
	saveIfNeeded();
	saveViewState();
	qApp->exit();
}

void ApplicationManager::loadViewState()
{
	m_view->restoreGeometry(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/geometry",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);

	m_scenarioManager->loadViewState();
	m_charactersManager->loadViewState();
	m_locationsManager->loadViewState();
	m_settingsManager->loadViewState();
}

void ApplicationManager::saveViewState()
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/geometry", m_view->saveGeometry().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);

	m_scenarioManager->saveViewState();
	m_charactersManager->saveViewState();
	m_locationsManager->saveViewState();
	m_settingsManager->saveViewState();
}

void ApplicationManager::saveIfNeeded()
{
	//
	// Если какие-то данные изменены
	//
	if (m_view->isWindowModified()) {
		//
		// ... спрашиваем пользователя, хочет ли он сохранить изменения
		//
		int questionResult = QMessageBox::question(m_view, tr("Save project changes?"),
												   tr("Project was modified. Save changes?"),
												   QMessageBox::Yes | QMessageBox::No);
		//
		// ... и сохраняем, если хочет
		//
		if (questionResult == QMessageBox::Yes) {
			aboutSave();
		} else {
			m_view->setWindowModified(false);
		}
	}
}

void ApplicationManager::saveCurrentProjectInRecent()
{
	//
	// Сохраним текущий проект в недавно использованых
	//
	m_startUpManager->addRecentFile(DatabaseLayer::Database::currentFile());
}

void ApplicationManager::goToEditCurrentProject()
{
	//
	// Активируем вкладки
	//
	::enableActionsOnProjectOpen();

	//
	// Загрузить данные из файла
	//
	m_scenarioManager->loadCurrentProject();
	m_charactersManager->loadCurrentProject();
	m_locationsManager->loadCurrentProject();

	//
	// Установим заголовок
	//
	QString projectFileName = DatabaseLayer::Database::currentFile();
	projectFileName = projectFileName.split(QDir::separator()).last();
	m_view->setWindowTitle(tr("%1[*] - Scenarist").arg(projectFileName));

	//
	// Добавим проект к недавно используемым
	//
	saveCurrentProjectInRecent();

	//
	// Перейти на вкладку редактирования сценария
	//
	m_tabs->setCurrent(1);
}

void ApplicationManager::initView()
{
	//
	// Настроим меню
	//
	m_menu->setText(tr("Menu"));
	m_menu->setPopupMode(QToolButton::MenuButtonPopup);
	m_menu->setMenu(createMenu());

	//
	// Настроим боковую панель
	//
	m_tabs->addTab(tr("Start"), QIcon(":/Graphics/Icons/start.png"));
	g_disableOnStartActions << m_tabs->addTab(tr("Scenario"), QIcon(":/Graphics/Icons/script.png"));
	g_disableOnStartActions << m_tabs->addTab(tr("Characters"), QIcon(":/Graphics/Icons/characters.png"));
	g_disableOnStartActions << m_tabs->addTab(tr("Locations"), QIcon(":/Graphics/Icons/locations.png"));
	m_tabs->addTab(tr("Settings"), QIcon(":/Graphics/Icons/settings.png"));

	//
	// Настроим виджеты соответствующие вкладкам
	//
	m_tabsWidgets->addWidget(m_startUpManager->view());
	m_tabsWidgets->addWidget(m_scenarioManager->view());
	m_tabsWidgets->addWidget(m_charactersManager->view());
	m_tabsWidgets->addWidget(m_locationsManager->view());
	m_tabsWidgets->addWidget(m_settingsManager->view());

	//
	// Расположим всё на форме
	//
	QVBoxLayout* leftLayout = new QVBoxLayout;
	leftLayout->setContentsMargins(QMargins());
	leftLayout->setSpacing(0);
	leftLayout->addWidget(m_menu);
	leftLayout->addWidget(m_tabs);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addLayout(leftLayout);
	layout->addLayout(m_tabsWidgets);

	m_view->setLayout(layout);

	//
	// Отключим некоторые действия, которые не могут быть выполнены до момента загрузки проекта
	//
	::disableActionsOnStart();
}

QMenu* ApplicationManager::createMenu()
{
	//
	// Сформируем меню
	//
	QMenu* menu = new QMenu(m_menu);
	QAction* createNewProject = menu->addAction(tr("New"));
	QAction* openProject = menu->addAction(tr("Open"));
	QAction* saveProject = menu->addAction(tr("Save"));
	QAction* saveProjectAs = menu->addAction(tr("Save As..."));
	g_disableOnStartActions << saveProject;
	g_disableOnStartActions << saveProjectAs;

	//
	// ... экспорт
	//
	QMenu* exportMenu = new QMenu(tr("Export to..."), m_menu);
	QAction* exportToPdf = exportMenu->addAction(tr("PDF"));
	QAction* exportToRtf = exportMenu->addAction(tr("RTF"));
	g_disableOnStartActions << menu->addMenu(exportMenu);

	//
	// Настроим соединения
	//
	connect(createNewProject, SIGNAL(triggered()), this, SLOT(aboutCreateNew()));
	connect(openProject, SIGNAL(triggered()), this, SLOT(aboutLoad()));
	connect(saveProject, SIGNAL(triggered()), this, SLOT(aboutSave()));
	connect(saveProjectAs, SIGNAL(triggered()), this, SLOT(aboutSaveAs()));
	connect(exportToPdf, SIGNAL(triggered()), this, SLOT(aboutExportToPdf()));
	connect(exportToRtf, SIGNAL(triggered()), this, SLOT(aboutExportToRtf()));

	return menu;
}

void ApplicationManager::initConnections()
{
	connect(m_view, SIGNAL(wantToClose()), this, SLOT(aboutExit()));

	connect(m_menu, SIGNAL(clicked()), m_menu, SLOT(showMenu()));
	connect(m_tabs, SIGNAL(currentChanged(int)), m_tabsWidgets, SLOT(setCurrentIndex(int)));

	connect(m_startUpManager, SIGNAL(createProjectRequested()), this, SLOT(aboutCreateNew()));
	connect(m_startUpManager, SIGNAL(openProjectRequested()), this, SLOT(aboutLoad()));
	connect(m_startUpManager, SIGNAL(openRecentProjectRequested(QString)), this, SLOT(aboutLoad(QString)));

	connect(m_charactersManager, SIGNAL(characterNameChanged(QString,QString)),
			m_scenarioManager, SLOT(aboutCharacterNameChanged(QString,QString)));
	connect(m_charactersManager, SIGNAL(refreshCharacters()),
			m_scenarioManager, SLOT(refreshCharacters()));

	connect(m_locationsManager, SIGNAL(locationNameChanged(QString,QString)),
			m_scenarioManager, SLOT(aboutLocationNameChanged(QString,QString)));
	connect(m_locationsManager, SIGNAL(refreshLocations()),
			m_scenarioManager, SLOT(refreshLocations()));

	connect(m_settingsManager, SIGNAL(textEditSettingsUpdated()),
			m_scenarioManager, SLOT(aboutTextEditSettingsUpdated()));
	connect(m_settingsManager, SIGNAL(navigatorSettingsUpdated()),
			m_scenarioManager, SLOT(aboutNavigatorSettingsUpdated()));
	connect(m_settingsManager, SIGNAL(chronometrySettingsUpdated()),
			m_scenarioManager, SLOT(aboutChronometrySettingsUpdated()));
}
