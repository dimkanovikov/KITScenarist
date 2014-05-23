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
#include <QStackedWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolButton>
#include <QMenu>
#include <QStyle>
#include <QStyleFactory>
#include <QVBoxLayout>

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

	/**
	 * @brief Получить путь к папке экспортируемых файлов
	 */
	static QString exportFolderPath() {
		QString exportFolderPath =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/export-files",
					DataStorageLayer::SettingsStorage::ApplicationSettings);
		if (exportFolderPath.isEmpty()) {
			exportFolderPath = QDir::homePath();
		}
		return exportFolderPath;
	}

	/**
	 * @brief Сохранить путь к папке экспортируемых файлов
	 */
	static void saveExportFolderPath(const QString& _path) {
		DataStorageLayer::StorageFacade::settingsStorage()->setValue(
					"application/export-files",
					QFileInfo(_path).absoluteDir().absolutePath(),
					DataStorageLayer::SettingsStorage::ApplicationSettings);
	}
}


ApplicationManager::ApplicationManager(QObject *parent) :
	QObject(parent),
	m_view(new ApplicationView),
	m_menu(new QToolButton(m_view)),
	m_tabs(new SideTabBar(m_view)),
	m_tabsWidgets(new QStackedWidget),
	m_startUpManager(new StartUpManager(this, m_view)),
	m_scenarioManager(new ScenarioManager(this, m_view)),
	m_charactersManager(new CharactersManager(this, m_view)),
	m_locationsManager(new LocationsManager(this, m_view)),
	m_settingsManager(new SettingsManager(this, m_view))
{
	initView();
	initConnections();
	initStyleSheet();
	reloadApplicationSettings();
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
			// ... очистим все загруженные на текущий момент данные
			//
			DataStorageLayer::StorageFacade::clearStorages();

			//
			// ... переключаемся на работу с выбранным файлом
			//
			DatabaseLayer::Database::setCurrentFile(loadProjectFileName);

			//
			// ... сохраняем путь
			//
			saveProjectsFolderPath(loadProjectFileName);

			//
			// ... перейдём к редактированию
			//
			goToEditCurrentProject();
		}
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
				exportFolderPath(),
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

		//
		// ... сохраним путь
		//
		saveExportFolderPath(exportFileName);
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
				exportFolderPath(),
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

		//
		// ... сохраним путь
		//
		saveExportFolderPath(exportFileName);
	}
}

void ApplicationManager::aboutPrintPreview()
{
	BusinessLogic::PdfExporter exporter;
	exporter.printPreview(m_scenarioManager->scenario()->document());
}

void ApplicationManager::aboutExit()
{
    //
    // Сохраняем, если необходимо
    //
	if (saveIfNeeded()) {
		//
		// Ожидаем завершения всех операций с БД
		//
		DataStorageLayer::StorageFacade::waitWhileSave();

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
				m_view->setWindowModified(false);
			}
		} else {
			success = false;
		}
	}

	return success;
}

void ApplicationManager::saveCurrentProjectInRecent()
{
	//
	// Сохраним текущий проект в недавно использованых
	//
	m_startUpManager->addRecentFile(DatabaseLayer::Database::currentFile(), m_scenarioManager->scenarioName());
}

void ApplicationManager::goToEditCurrentProject()
{
	//
	// Перейти на вкладку редактирования сценария
	//
	m_tabs->setCurrent(1);

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

	// ... экспорт
	QMenu* exportMenu = new QMenu(tr("Export to..."), m_menu);
	QAction* exportToPdf = exportMenu->addAction(tr("PDF"));
	QAction* exportToRtf = exportMenu->addAction(tr("RTF"));
	g_disableOnStartActions << menu->addMenu(exportMenu);

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
	connect(exportToPdf, SIGNAL(triggered()), this, SLOT(aboutExportToPdf()));
	connect(exportToRtf, SIGNAL(triggered()), this, SLOT(aboutExportToRtf()));
	connect(printPreview, SIGNAL(triggered()), this, SLOT(aboutPrintPreview()));

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
			palette.setColor(QPalette::Window, QColor("#282D31"));
			palette.setColor(QPalette::Base, QColor("#404040"));
			palette.setColor(QPalette::Disabled, QPalette::Base, QColor("#333333"));
			palette.setColor(QPalette::AlternateBase, QColor(53,53,53));

			palette.setColor(QPalette::Highlight, QColor(142,45,197));
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

			palette.setColor(QPalette::ToolTipText, QColor("#EBEBEB"));

			//
			// Фон неактивного пункта меню
			//
			palette.setColor(QPalette::Light, QColor("#404040"));
		} else {
			//
			// Светлой темой как раз является стандартная палитра стиля без стилевых надстроек
			//
			palette = QStyleFactory::create("Fusion")->standardPalette();

			palette.setColor(QPalette::HighlightedText, QColor(255,255,255));

			palette.setColor(QPalette::Link, QColor("#2b78da"));
		}

		//
		// Для всплывающей используем универсальный стиль
		//
		styleSheet = "QToolTip { color: palette(window-text); border: 1px solid palette(highlight); padding: 2px; background-color: palette(window) }";

		//
		// Применяем тему
		//
		qApp->setPalette(palette);
		qApp->setStyleSheet(styleSheet);

		//
		// Обновим лого
		//
		m_startUpManager->aboutUpdateLogo(useDarkTheme);
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
}
