#include "ApplicationManager.h"

#include "Project/ProjectsManager.h"
#include "StartUp/StartUpManager.h"
#include "Scenario/ScenarioManager.h"
#include "Characters/CharactersManager.h"
#include "Locations/LocationsManager.h"
#include "Settings/SettingsManager.h"
#include "Import/ImportManager.h"
#include "Export/ExportManager.h"
#include "Synchronization/SynchronizationManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/Export/PdfExporter.h>
#include <BusinessLayer/Export/RtfExporter.h>

#include <DataLayer/Database/Database.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/SideBar/SideBar.h>
#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>

#include <UserInterfaceLayer/ApplicationView.h>

#include <QApplication>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QStyle>
#include <QStyleFactory>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

using namespace ManagementLayer;
using UserInterface::ApplicationView;

namespace {
	const QString PROJECT_FILE_EXTENSION = ".kitsp"; // kit scenarist project
	const char* MAC_CHANGED_SUFFIX =
			QT_TRANSLATE_NOOP("ManagementLayer::ApplicationManager", " - changed");

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

	/**
	 * @brief Получить путь к папке проектов
	 */
	static QString projectsFolderPath() {
		QString projectsFolderPath =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/project-files",
					DataStorageLayer::SettingsStorage::ApplicationSettings);
		if (projectsFolderPath.isEmpty()) {
			projectsFolderPath = QDir::homePath();
		}
		return projectsFolderPath;
	}

	/**
	 * @brief Сохранить путь к папке проектов
	 */
	static void saveProjectsFolderPath(const QString& _path) {
		DataStorageLayer::StorageFacade::settingsStorage()->setValue(
					"application/project-files",
					QFileInfo(_path).absoluteDir().absolutePath(),
					DataStorageLayer::SettingsStorage::ApplicationSettings);
	}

	static void updateWindowModified(QWidget* _widget, bool _modified) {
#ifdef Q_OS_MAC
		static const QString suffix =
				QApplication::translate("ManagementLayer::ApplicationManager", MAC_CHANGED_SUFFIX);
		if (_modified) {
			if (!_widget->windowTitle().endsWith(suffix)) {
				_widget->setWindowTitle(_widget->windowTitle() + suffix);
			}
		} else {
			if (_widget->windowTitle().endsWith(suffix)) {
				_widget->setWindowTitle(_widget->windowTitle().remove(suffix));
			}
		}
#endif
		_widget->setWindowModified(_modified);
	}
}


ApplicationManager::ApplicationManager(QObject *parent) :
	QObject(parent),
	m_view(new ApplicationView),
	m_menu(new QToolButton(m_view)),
	m_tabs(new SideTabBar(m_view)),
	m_tabsWidgets(new QStackedWidget),
	m_projectsManager(new ProjectsManager(this)),
	m_startUpManager(new StartUpManager(this, m_view)),
	m_scenarioManager(new ScenarioManager(this, m_view)),
	m_charactersManager(new CharactersManager(this, m_view)),
	m_locationsManager(new LocationsManager(this, m_view)),
	m_settingsManager(new SettingsManager(this, m_view)),
	m_importManager(new ImportManager(this, m_view)),
	m_exportManager(new ExportManager(this, m_view)),
	m_synchronizationManager(new SynchronizationManager(this, m_view))
{
	initView();
	initConnections();
	initStyleSheet();

	aboutUpdateProjectsList();

	reloadApplicationSettings();

	QTimer::singleShot(0, m_synchronizationManager, SLOT(login()));
}

ApplicationManager::~ApplicationManager()
{
	delete m_view;
	m_view = 0;
}

void ApplicationManager::exec(const QString& _fileToOpen)
{
	loadViewState();
	m_view->show();

	if (!_fileToOpen.isEmpty()) {
		aboutLoad(_fileToOpen);
	}
}

void ApplicationManager::openFile(const QString &_fileToOpen)
{
	aboutLoad(_fileToOpen);
}

void ApplicationManager::aboutUpdateProjectsList()
{
	m_startUpManager->setRecentProjects(m_projectsManager->recentProjects());
	m_startUpManager->setRemoteProjects(m_projectsManager->remoteProjects());
}

void ApplicationManager::aboutCreateNew()
{
	//
	// Спросить у пользователя хочет ли он сохранить проект
	//
	if (saveIfNeeded()) {
		//
		// Получим имя файла для нового проекта
		//
		QString newProjectFileName =
				QFileDialog::getSaveFileName(
					m_view,
					tr("Choose file for new project"),
					projectsFolderPath(),
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
			// ... закроем текущий проект
			//
			closeCurrentProject();

			//
			// ... если файл существовал, удалим его для удаления данных в нём
			//
			if (QFile::exists(newProjectFileName)) {
				QFile::remove(newProjectFileName);
			}

			//
			// ... создаём новую базу данных в файле и делаем её текущим проектом
			//
			m_projectsManager->setCurrentProject(newProjectFileName);

			//
			// ... сохраняем путь
			//
			saveProjectsFolderPath(newProjectFileName);

			//
			// ... перейдём к редактированию
			//
			goToEditCurrentProject();
		}
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
				projectsFolderPath(),
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
		if (saveAsProjectFileName == ProjectsManager::currentProject().path()) {
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
			QFile::copy(ProjectsManager::currentProject().path(), saveAsProjectFileName);

			//
			// ... переключаемся на использование другого файла
			//
			DatabaseLayer::Database::setCurrentFile(saveAsProjectFileName);

			//
			// ... сохраняем изменения
			//
			aboutSave();
			m_view->setWindowModified(true);

			//
			// ... обновим заголовок
			//
			updateWindowTitle();
		}

		//
		// ... сохраняем путь
		//
		saveProjectsFolderPath(saveAsProjectFileName);
	}
}

void ApplicationManager::aboutSave()
{
	//
	// Если какие-то данные изменены
	//
	if (m_view->isWindowModified()) {
		//
		// Определим место хранения проекта
		//
		const bool currentProjectIsRemote = m_projectsManager->currentProject().type() == Project::Remote;

		//
		// Управляющие должны сохранить несохранённые данные
		//
		DatabaseLayer::Database::transaction();
		m_scenarioManager->saveCurrentProject();
		m_charactersManager->saveCharacters();
		m_locationsManager->saveLocations();
		DatabaseLayer::Database::commit();

		//
		// Для проекта из облака отправляем данные на сервер
		//
		if (currentProjectIsRemote) {
			m_synchronizationManager->aboutSaveScenario(m_scenarioManager->scenario()->scenario());
			m_synchronizationManager->aboutSaveScenario(m_scenarioManager->scenarioDraft()->scenario());
			m_synchronizationManager->aboutSaveData();
		}

		//
		// Изменим статус окна на сохранение изменений
		//
		::updateWindowModified(m_view, false);

		//
		// Если необходимо создадим резервную копию закрываемого файла
		//
		m_backupHelper.saveBackup(ProjectsManager::currentProject().path());
	}
}

void ApplicationManager::aboutLoad(const QString& _fileName)
{
	//
	// Если нужно сохранить проект
	//
	if (saveIfNeeded()) {
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
						projectsFolderPath(),
						tr ("Scenarist project files (*%1)").arg(PROJECT_FILE_EXTENSION)
						);
		}

		//
		// Если файл выбран
		//
		if (!loadProjectFileName.isEmpty()) {
			//
			// ... закроем текущий проект
			//
			closeCurrentProject();

			//
			// ... переключаемся на работу с выбранным файлом
			//
			m_projectsManager->setCurrentProject(loadProjectFileName);

			//
			// ... сохраняем путь
			//
			saveProjectsFolderPath(loadProjectFileName);

			//
			// ... перейдём к редактированию
			//
			goToEditCurrentProject();
		}

		//
		// Изменим статус окна на сохранение изменений
		//
		::updateWindowModified(m_view, false);
	}
}

void ApplicationManager::aboutLoadFromRecent(const QModelIndex& _projectIndex)
{
	//
	// Если нужно сохранить проект
	//
	if (saveIfNeeded()) {
		//
		// ... закроем текущий проект
		//
		closeCurrentProject();

		//
		// ... переключаемся на работу с выбранным файлом
		//
		m_projectsManager->setCurrentProject(_projectIndex);

		//
		// ... перейдём к редактированию
		//
		goToEditCurrentProject();
	}
}

void ApplicationManager::aboutLoadFromRemote(const QModelIndex& _projectIndex)
{
	//
	// Если нужно сохранить проект
	//
	if (saveIfNeeded()) {
		//
		// ... закроем текущий проект
		//
		closeCurrentProject();

		//
		// ... переключаемся на работу с выбранным файлом
		//
		const bool isRemote = false;
		m_projectsManager->setCurrentProject(_projectIndex, isRemote);

		//
		// ... перейдём к редактированию
		//
		goToEditCurrentProject();
	}
}

void ApplicationManager::aboutImport()
{
	m_importManager->importScenario(m_scenarioManager->scenario(), m_scenarioManager->cursorPosition());
}

void ApplicationManager::aboutExportTo()
{
	m_exportManager->exportScenario(m_scenarioManager->scenario());
}

void ApplicationManager::aboutPrintPreview()
{
	m_exportManager->printPreviewScenario(m_scenarioManager->scenario());
}

void ApplicationManager::aboutExit()
{
	//
	// Сохраняем, если необходимо
	//
	if (saveIfNeeded()) {
		//
		// Выводим информацию для пользователя, о закрытии программы
		//
		ProgressWidget* progress = new ProgressWidget(m_view);
		progress->showProgress(tr("Exit from Application"), tr("Closing Databse Connections and Remove Temporatry Files."));

		//
		// Закроем текущий проект
		//
		closeCurrentProject();

		//
		// Сохраняем состояния виджетов
		//
		saveViewState();

		//
		// Выходим
		//
		qApp->exit();
	}
}

void ApplicationManager::aboutApplicationSettingsUpdated()
{
	reloadApplicationSettings();
}

void ApplicationManager::aboutProjectChanged()
{
	::updateWindowModified(m_view, true);
}

void ApplicationManager::aboutShowFullscreen()
{
	const char* IS_MAXIMIZED_PROPERTY = "isMaximized";

	if (m_view->isFullScreen()) {
		//
		// Возвращаемся в состояние предшествовавшее полноэкранному режиму
		//
		if (m_view->property(IS_MAXIMIZED_PROPERTY).toBool()) {
			m_view->showMaximized();
		} else {
			m_view->showNormal();
		}
		m_menu->show();
		m_tabs->show();
	} else {
		//
		// Сохраним состояние окна перед переходом в полноэкранный режим
		//
		m_view->setProperty(IS_MAXIMIZED_PROPERTY, m_view->windowState().testFlag(Qt::WindowMaximized));

		//
		// Переходим в полноэкранный режим
		//
		m_tabsWidgets->setCurrentWidget(m_scenarioManager->view());
		m_menu->hide();
		m_tabs->hide();
		m_view->showFullScreen();
	}
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

bool ApplicationManager::saveIfNeeded()
{
	bool success = true;

	//
	// Если какие-то данные изменены
	//
	if (m_view->isWindowModified()) {
		//
		// ... спрашиваем пользователя, хочет ли он сохранить изменения
		//
		int questionResult = QMessageBox::question(m_view, tr("Save project changes?"),
												   tr("Project was modified. Save changes?"),
												   QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No);

		if (questionResult != QMessageBox::Cancel) {
			//
			// ... и сохраняем, если хочет
			//
			if (questionResult == QMessageBox::Yes) {
				aboutSave();
			} else {
				::updateWindowModified(m_view, false);
			}
		} else {
			success = false;
		}
	}

	return success;
}

void ApplicationManager::goToEditCurrentProject()
{
	//
	// Покажем уведомление пользователю
	//
	ProgressWidget progress(m_view);
	progress.showProgress(tr("Loading Scenario"), tr("Please wait. Loading can take few minutes."));

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
	// Загрузить настройки файла
	//
	m_scenarioManager->loadCurrentProjectSettings(ProjectsManager::currentProject().path());
	m_exportManager->loadCurrentProjectSettings(ProjectsManager::currentProject().path());

	//
	// Установим заголовок
	//
	updateWindowTitle();

	//
	// Обновим название текущего проекта, т.к. данные о проекте теперь загружены
	//
	m_projectsManager->setCurrentProjectName(m_scenarioManager->scenarioName());

	//
	// Перейти на вкладку редактирования сценария
	//
	m_tabs->setCurrent(1);

	//
	// Закроем уведомление
	//
	progress.finish();
}

void ApplicationManager::closeCurrentProject()
{
	//
	// Сохраним настройки закрываемого проекта
	//
	m_scenarioManager->saveCurrentProjectSettings(ProjectsManager::currentProject().path());
	m_exportManager->saveCurrentProjectSettings(ProjectsManager::currentProject().path());

	//
	// Очистим все загруженные на текущий момент данные
	//
	DataStorageLayer::StorageFacade::clearStorages();

	//
	// Если использовалась база данных, то удалим старое соединение
	//
	DatabaseLayer::Database::closeCurrentFile();

	//
	// Информируем управляющего проектами, что текущий проект закрыт
	//
	m_projectsManager->closeCurrentProject();
}

void ApplicationManager::initView()
{
	//
	// Настроим меню
	//
	m_menu->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
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
	layout->addWidget(m_tabsWidgets);

	m_view->setLayout(layout);

	//
	// Отключим некоторые действия, которые не могут быть выполнены до момента загрузки проекта
	//
	::disableActionsOnStart();

	//
	// Настроим
	//
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
	saveProject->setShortcut(QKeySequence::Save);
	QAction* saveProjectAs = menu->addAction(tr("Save As..."));
	g_disableOnStartActions << saveProject;
	g_disableOnStartActions << saveProjectAs;

	menu->addSeparator();
	// ... импорт
	QAction* import = menu->addAction(tr("Import..."));
	g_disableOnStartActions << import;
	// ... экспорт
	QAction* exportTo = menu->addAction(tr("Export to..."));
	g_disableOnStartActions << exportTo;
	// ... предварительный просмотр
	QAction* printPreview = menu->addAction(tr("Print Preview"));
	printPreview->setShortcut(QKeySequence(Qt::Key_F12));
	g_disableOnStartActions << printPreview;

	//
	// Настроим соединения
	//
	connect(createNewProject, SIGNAL(triggered()), this, SLOT(aboutCreateNew()));
	connect(openProject, SIGNAL(triggered()), this, SLOT(aboutLoad()));
	connect(saveProject, SIGNAL(triggered()), this, SLOT(aboutSave()));
	connect(saveProjectAs, SIGNAL(triggered()), this, SLOT(aboutSaveAs()));
	connect(import, SIGNAL(triggered()), this, SLOT(aboutImport()));
	connect(exportTo, SIGNAL(triggered()), this, SLOT(aboutExportTo()));
	connect(printPreview, SIGNAL(triggered()), this, SLOT(aboutPrintPreview()));

	return menu;
}

void ApplicationManager::initConnections()
{
	connect(m_view, SIGNAL(wantToClose()), this, SLOT(aboutExit()));

	connect(m_menu, SIGNAL(clicked()), m_menu, SLOT(showMenu()));
	connect(m_tabs, SIGNAL(currentChanged(int)), m_tabsWidgets, SLOT(setCurrentIndex(int)));

	connect(m_projectsManager, SIGNAL(recentProjectsUpdated()), this, SLOT(aboutUpdateProjectsList()));
	connect(m_projectsManager, SIGNAL(remoteProjectsUpdated()), this, SLOT(aboutUpdateProjectsList()));

	connect(m_startUpManager, SIGNAL(loginRequested(QString,QString)), m_synchronizationManager, SLOT(aboutLogin(QString,QString)));
	connect(m_startUpManager, SIGNAL(logoutRequested()), m_synchronizationManager, SLOT(aboutLogout()));
	connect(m_startUpManager, SIGNAL(createProjectRequested()), this, SLOT(aboutCreateNew()));
	connect(m_startUpManager, SIGNAL(openProjectRequested()), this, SLOT(aboutLoad()));
	connect(m_startUpManager, SIGNAL(refreshProjectsRequested()), m_projectsManager, SLOT(refreshProjects()));
	connect(m_startUpManager, SIGNAL(refreshProjectsRequested()), m_synchronizationManager, SLOT(aboutLoadProjects()));
	connect(m_startUpManager, SIGNAL(openRecentProjectRequested(QModelIndex)), this, SLOT(aboutLoadFromRecent(QModelIndex)));
	connect(m_startUpManager, SIGNAL(openRemoteProjectRequested(QModelIndex)), this, SLOT(aboutLoadFromRemote(QModelIndex)));

	connect(m_scenarioManager, SIGNAL(showFullscreen()), this, SLOT(aboutShowFullscreen()));

	connect(m_charactersManager, SIGNAL(characterNameChanged(QString,QString)),
			m_scenarioManager, SLOT(aboutCharacterNameChanged(QString,QString)));
	connect(m_charactersManager, SIGNAL(refreshCharacters()),
			m_scenarioManager, SLOT(aboutRefreshCharacters()));

	connect(m_locationsManager, SIGNAL(locationNameChanged(QString,QString)),
			m_scenarioManager, SLOT(aboutLocationNameChanged(QString,QString)));
	connect(m_locationsManager, SIGNAL(refreshLocations()),
			m_scenarioManager, SLOT(aboutRefreshLocations()));

	connect(m_settingsManager, SIGNAL(applicationSettingsUpdated()),
			this, SLOT(aboutApplicationSettingsUpdated()));
	connect(m_settingsManager, SIGNAL(scenarioEditSettingsUpdated()),
			m_scenarioManager, SLOT(aboutTextEditSettingsUpdated()));
	connect(m_settingsManager, SIGNAL(navigatorSettingsUpdated()),
			m_scenarioManager, SLOT(aboutNavigatorSettingsUpdated()));
	connect(m_settingsManager, SIGNAL(chronometrySettingsUpdated()),
			m_scenarioManager, SLOT(aboutChronometrySettingsUpdated()));
	connect(m_settingsManager, SIGNAL(countersSettingsUpdated()),
			m_scenarioManager, SLOT(aboutCountersSettingsUpdated()));

	connect(m_exportManager, SIGNAL(scenarioNameChanged(QString)),
			m_scenarioManager, SLOT(aboutScenarioNameChanged(QString)));

	connect(m_scenarioManager, SIGNAL(scenarioChanged()), this, SLOT(aboutProjectChanged()));
	connect(m_charactersManager, SIGNAL(characterChanged()), this, SLOT(aboutProjectChanged()));
	connect(m_locationsManager, SIGNAL(locationChanged()), this, SLOT(aboutProjectChanged()));
	connect(m_exportManager, SIGNAL(scenarioTitleListDataChanged()), this, SLOT(aboutProjectChanged()));

	connect(m_synchronizationManager, SIGNAL(loginAccepted(QString)),
			m_startUpManager, SLOT(aboutUserLogged(QString)));
	connect(m_synchronizationManager, SIGNAL(loginNotAccepted(QString,QString,QString)),
			m_startUpManager, SLOT(aboutRetryLogin(QString,QString,QString)));
	connect(m_synchronizationManager, SIGNAL(logoutAccepted()),
			m_startUpManager, SLOT(aboutUserUnlogged()));
	connect(m_synchronizationManager, SIGNAL(remoteProjectsLoaded(QString)),
			m_projectsManager, SLOT(setRemoteProjects(QString)));
}

void ApplicationManager::initStyleSheet()
{
	//
	// Загрузим стиль
	//
	QFile styleSheetFile(":/Interface/UI/style.qss");
	styleSheetFile.open(QIODevice::ReadOnly);
	QString styleSheet = styleSheetFile.readAll();
	styleSheetFile.close();

	//
	// Установим стиль на главный виджет приложения
	//
	m_view->setStyleSheet(styleSheet);

	//
	// Настраиваем виджеты
	//
	m_menu->setProperty("inTopPanel", true);
	m_menu->setProperty("topPanelTopBordered", true);
	m_menu->setProperty("topPanelRightBordered", true);
}

void ApplicationManager::reloadApplicationSettings()
{
	//
	// Внешний вид приложения
	//
	bool useDarkTheme =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/use-dark-theme",
				DataStorageLayer::SettingsStorage::ApplicationSettings)
			.toInt();

	{
		//
		// Настраиваем палитру и стилевые надстройки в зависимости от темы
		//
		QPalette palette = QStyleFactory::create("Fusion")->standardPalette();
		QString styleSheet;

		if (useDarkTheme) {
			palette.setColor(QPalette::Window, QColor("#26282a"));
			palette.setColor(QPalette::Base, QColor("#404040"));
			palette.setColor(QPalette::Disabled, QPalette::Base, QColor("#333333"));
			palette.setColor(QPalette::AlternateBase, QColor(53,53,53));

			palette.setColor(QPalette::Highlight, QColor("#2b78da"));
			palette.setColor(QPalette::HighlightedText, QColor(255,255,255));

			palette.setColor(QPalette::WindowText, QColor("#EBEBEB"));
			palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#a1a1a1"));
			palette.setColor(QPalette::Text, QColor("#EBEBEB"));
			palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#a1a1a1"));

			palette.setColor(QPalette::Button, QColor(53,53,53));
			palette.setColor(QPalette::Disabled, QPalette::Button, QColor("#1b1e21"));
			palette.setColor(QPalette::ButtonText, QColor(255,255,255));
			palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#a1a1a1"));

			palette.setColor(QPalette::Link, QColor("#2b78da"));

			//
			// Фон неактивного пункта меню
			//
			palette.setColor(QPalette::Light, QColor("#404040"));
			//
			//
			//
			palette.setColor(QPalette::Midlight, QColor("#696765"));
			palette.setColor(QPalette::Shadow, QColor("#1c2023"));
		} else {
			//
			// Светлой темой как раз является стандартная палитра стиля без стилевых надстроек
			//
			palette = QStyleFactory::create("Fusion")->standardPalette();

			palette.setColor(QPalette::Highlight, QColor("#2b78da"));
			palette.setColor(QPalette::HighlightedText, QColor(255,255,255));

			palette.setColor(QPalette::Link, QColor("#2b78da"));

			palette.setColor(QPalette::Shadow, palette.midlight().color());
		}

		//
		// Для всплывающей используем универсальный стиль
		//
		styleSheet += "QToolTip { color: palette(window-text); background-color: palette(window); border: 1px solid palette(highlight); } "
					 ;

		//
		// Применяем тему
		//
		qApp->setPalette(palette);
		qApp->setStyleSheet(styleSheet);
	}

	//
	// Автосохранение
	//
	bool autosave =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/autosave",
				DataStorageLayer::SettingsStorage::ApplicationSettings)
			.toInt();
	int autosaveInterval =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/autosave-interval",
				DataStorageLayer::SettingsStorage::ApplicationSettings)
			.toInt();

	m_autosaveTimer.stop();
	m_autosaveTimer.disconnect();
	if (autosave) {
		connect(&m_autosaveTimer, SIGNAL(timeout()), this, SLOT(aboutSave()));
		m_autosaveTimer.start(autosaveInterval * 60 * 1000); // Переводим минуты в миллисекунды
	}

	//
	// Создание резервных копий
	//
	bool saveBackups =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/save-backups",
				DataStorageLayer::SettingsStorage::ApplicationSettings)
			.toInt();
	const QString saveBackupsFolder =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/save-backups-folder",
				DataStorageLayer::SettingsStorage::ApplicationSettings);
	m_backupHelper.setIsActive(saveBackups);
	m_backupHelper.setBackupDir(saveBackupsFolder);
}

void ApplicationManager::updateWindowTitle()
{
	QString projectFileName = ProjectsManager::currentProject().path();
	projectFileName = projectFileName.split(QDir::separator()).last();
#ifdef Q_OS_MAC
	m_view->setWindowTitle(projectFileName);
#else
	m_view->setWindowTitle(tr("%1[*] - Scenarist").arg(projectFileName));
#endif
}
