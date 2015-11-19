#include "ApplicationManager.h"

#include "Project/ProjectsManager.h"
#include "StartUp/StartUpManager.h"
#include "Research/ResearchManager.h"
#include "Scenario/ScenarioManager.h"
#include "Characters/CharactersManager.h"
#include "Locations/LocationsManager.h"
#include "Statistics/StatisticsManager.h"
#include "Settings/SettingsManager.h"
#include "Import/ImportManager.h"
#include "Export/ExportManager.h"
#include "Synchronization/SynchronizationManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/Export/PdfExporter.h>

#include <DataLayer/Database/Database.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/SideBar/SideBar.h>
#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <UserInterfaceLayer/ApplicationView.h>

#include <QApplication>
#include <QComboBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QStyle>
#include <QStyleFactory>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

#include <functional>

using namespace ManagementLayer;
using UserInterface::ApplicationView;

namespace {
	/**
	 * @brief Номера вкладок
	 */
	/** @{ */
	const int STARTUP_TAB_INDEX = 0;
	const int RESEARCH_TAB_INDEX = 1;
	const int SCENARIO_TAB_INDEX = 2;
	const int CHARACTERS_TAB_INDEX = 3;
	const int LOCATIONS_TAB_INDEX = 4;
	const int STATISTICS_TAB_INDEX = 5;
	const int SETTINGS_TAB_INDEX = 6;
	/** @} */

	/**
	 * @brief Расширения файлов проекта
	 */
	const QString PROJECT_FILE_EXTENSION = ".kitsp"; // kit scenarist project

	/**
	 * @brief Суффикс "изменено" для заголовка окна добавляемый в маке
	 */
	const char* MAC_CHANGED_SUFFIX =
			QT_TRANSLATE_NOOP("ManagementLayer::ApplicationManager", " - changed");

	/**
	 * @brief Флаг: синхронизация недоступна
	 */
	const bool SYNC_UNAVAILABLE = false;

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
	m_menuSecondary(new QLabel(m_view)),
	m_tabs(new SideTabBar(m_view)),
	m_tabsSecondary(new SideTabBar(m_view)),
	m_tabsWidgets(new QStackedWidget(m_view)),
	m_tabsWidgetsSecondary(new QStackedWidget(m_view)),
	m_splitter(new QSplitter(m_view)),
	m_projectsManager(new ProjectsManager(this)),
	m_startUpManager(new StartUpManager(this, m_view)),
	m_researchManager(new ResearchManager(this, m_view)),
	m_scenarioManager(new ScenarioManager(this, m_view)),
	m_charactersManager(new CharactersManager(this, m_view)),
	m_locationsManager(new LocationsManager(this, m_view)),
	m_statisticsManager(new StatisticsManager(this, m_view)),
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
			m_projectsManager->setCurrentProject(saveAsProjectFileName);

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
		// Управляющие должны сохранить несохранённые данные
		//
		DatabaseLayer::Database::transaction();
		m_researchManager->saveResearch();
		m_scenarioManager->saveCurrentProject();
		m_charactersManager->saveCharacters();
		m_locationsManager->saveLocations();
		DatabaseLayer::Database::commit();

		//
		// Для проекта из облака отправляем данные на сервер
		//
		if (m_projectsManager->currentProject().isRemote()) {
			m_synchronizationManager->aboutWorkSyncScenario();
			m_synchronizationManager->aboutWorkSyncData();
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

void ApplicationManager::aboutShowHelp()
{
	QDesktopServices::openUrl(QUrl("https://kitscenarist.ru/help/"));
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

void ApplicationManager::aboutUserUnlogged()
{
	//
	// Закрываем проект, если он из облака
	//
	if (m_projectsManager->currentProject().isRemote()) {
		closeCurrentProject();
	}
}

void ApplicationManager::aboutShowSyncActiveIndicator()
{
	m_tabs->addIndicator(QIcon(":/Graphics/Icons/Indicator/connected.png"), tr("Connection active"), tr("Project sinchronized"));
}

void ApplicationManager::aboutSyncClosedWithError(int _errorCode, const QString& _error)
{
	bool switchToOfflineMode = false;
	QString title;
	QString error = _error;
	switch (_errorCode) {
		//
		// Нет связи с интернетом
		//
		case 0: {
			title = tr("Network error");
			error = tr("Can't estabilish network connection.\n"
					   "Continue working in offline mode.");
			switchToOfflineMode = true;
			break;
		}

		//
		// Проблемы с вводом логина и пароля
		//
		case 100:
		case 101: {
			error = tr("Incorrect username or password.");
			m_startUpManager->aboutRetryLogin(error);
			break;
		}

		//
		// Закончилась подписка
		//
		case 102: {
			title = tr("Subscription ended");
			error = tr("Buyed subscription period is finished.\n"
					   "Continue working in offline mode.");
			QLightBoxMessage::information(m_view, title, error);
			switchToOfflineMode = true;
			break;
		}

		//
		// Сессия закрыта
		//
		case 104: {
			if (QLightBoxMessage::question(m_view, tr("Session closed"),
					tr("New session for you account started at other device. Restart session?"))
				== QDialogButtonBox::Yes) {
				//
				// Переподключаемся
				//
				QTimer::singleShot(0, m_synchronizationManager, SLOT(login()));
				return;
			} else {
				//
				// Переходим в автономный режим
				//
				title = tr("Session closed");
				error = tr("New session for you account started at other device.\n"
						   "Continue working in offline mode.");
				QLightBoxMessage::information(m_view, title, error);
				switchToOfflineMode = true;
			}
			break;
		}

		//
		// Проект недоступен
		//
		case 201: {
			title = tr("Project not available");
			error = tr("Current project is not available for syncronization now.\n"
					   "Continue working with this project in offline mode.");
			QLightBoxMessage::information(m_view, title, error);
			m_projectsManager->setCurrentProjectSyncAvailable(SYNC_UNAVAILABLE);
			break;
		}

		//
		// Остальное
		//
		default: {
			QLightBoxMessage::warning(m_view, tr("Error"), _error);
			break;
		}
	}

	//
	// Сигнализируем об ошибке
	//
	m_tabs->addIndicator(QIcon(":/Graphics/Icons/Indicator/disconnected.png"), title, error);

	//
	// Если необходимо переключаемся в автономный режим
	//
	if (switchToOfflineMode) {
		const QString loginData =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/user-name",
				DataStorageLayer::SettingsStorage::ApplicationSettings);

		//
		// Если есть закэшированные данные о прошлой авторизации
		//
		if (!loginData.isEmpty()) {
			//
			// Имитируем успешную авторизацию
			//
			m_startUpManager->aboutUserLogged();
			//
			// и загружаем список доступных проектов из кэша
			//
			QByteArray cachedProjectsXml =
					QByteArray::fromBase64(
						DataStorageLayer::StorageFacade::settingsStorage()->value(
							"application/remote-projects",
							DataStorageLayer::SettingsStorage::ApplicationSettings).toUtf8()
						);
			m_projectsManager->setRemoteProjects(cachedProjectsXml);
			//
			// говорим, что все проекты недоступны к синхронизации
			//
			m_projectsManager->setRemoteProjectsSyncUnavailable();
		}
	}
}

void ApplicationManager::aboutImport()
{
	m_importManager->importScenario(m_scenarioManager->scenario(), m_scenarioManager->cursorPosition());
}

void ApplicationManager::aboutExportTo()
{
	m_exportManager->exportScenario(m_scenarioManager->scenario(), m_researchManager->scenarioData());
}

void ApplicationManager::aboutPrintPreview()
{
	m_exportManager->printPreviewScenario(m_scenarioManager->scenario(), m_researchManager->scenarioData());
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
		ProgressWidget progress(m_view);
		progress.showProgress(tr("Exit from Application"), tr("Closing Databse Connections and Remove Temporatry Files."));

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
		progress.close();
		QApplication::processEvents();
		QApplication::quit();
	}
}

void ApplicationManager::aboutApplicationSettingsUpdated()
{
	reloadApplicationSettings();
}

void ApplicationManager::aboutProjectChanged()
{
	::updateWindowModified(m_view, true);

	m_statisticsManager->scenarioTextChanged();
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

void ApplicationManager::aboutPrepareScenarioForStatistics()
{
	m_statisticsManager->setExportedScenario(m_scenarioManager->scenario()->document());
}

void ApplicationManager::loadViewState()
{
	//
	// Загрузим состояние
	//
	DataStorageLayer::StorageFacade::settingsStorage()->loadApplicationStateAndGeometry(m_view);

	//
	// Для всех сплитеров добавляем функциональность - двойной щелчок, разворачивает панели
	//
	m_view->initSplittersRightClick();
}

void ApplicationManager::saveViewState()
{
	DataStorageLayer::StorageFacade::settingsStorage()->saveApplicationStateAndGeometry(m_view);
}

void ApplicationManager::currentTabIndexChanged()
{
	static bool processedNow = false;
	if (!processedNow) {
		processedNow = true;
		if (SideTabBar* sidebar = qobject_cast<SideTabBar*>(sender())) {
			//
			// Если выбрана та вкладка, что открыта во вспомогательной панели,
			// то нужно поменять их местами и наоборот
			//
			if (sidebar == m_tabs) {
				if (m_tabs->currentTab() == m_tabsSecondary->currentTab()) {
					m_tabsSecondary->setCurrentTab(m_tabs->prevCurrentTab());
				}
			} else {
				if (m_tabsSecondary->currentTab() == m_tabs->currentTab()) {
					m_tabs->setCurrentTab(m_tabsSecondary->prevCurrentTab());
				}
			}

			//
			// Функция для определения виджета для отображения по индексу вкладки
			//

			auto widgetForTab =
				[=] (int _index) {
				QWidget* result;
				switch (_index) {
					case STARTUP_TAB_INDEX: result = m_startUpManager->view(); break;
					case RESEARCH_TAB_INDEX: result = m_researchManager->view(); break;
					case SCENARIO_TAB_INDEX: result = m_scenarioManager->view(); break;
					case CHARACTERS_TAB_INDEX: result = m_charactersManager->view(); break;
					case LOCATIONS_TAB_INDEX: result = m_locationsManager->view(); break;
					case STATISTICS_TAB_INDEX: result = m_statisticsManager->view(); break;
					case SETTINGS_TAB_INDEX: result = m_settingsManager->view(); break;
				}
				return result;
			};

			//
			// Установим виджеты в контейнеры
			//
			{
				QWidget* widget = widgetForTab(m_tabs->currentTab());
				m_tabsWidgets->addWidget(widget);
				m_tabsWidgets->setCurrentWidget(widget);
			}
			//
			{
				QWidget* widget = widgetForTab(m_tabsSecondary->currentTab());
				m_tabsWidgetsSecondary->addWidget(widget);
				m_tabsWidgetsSecondary->setCurrentWidget(widget);
			}
		}

		processedNow = false;
	}
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
		int questionResult =
				QLightBoxMessage::question(m_view, tr("Save project changes?"),
					tr("Project was modified. Save changes?"),
					QDialogButtonBox::Cancel | QDialogButtonBox::Yes | QDialogButtonBox::No);

		if (questionResult != QDialogButtonBox::Cancel) {
			//
			// ... и сохраняем, если хочет
			//
			if (questionResult == QDialogButtonBox::Yes) {
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
	// Установим заголовок
	//
	updateWindowTitle();

	//
	// Настроим режим работы со сценарием
	//
	const bool isCommentOnly = ProjectsManager::currentProject().isCommentOnly();
	m_researchManager->setCommentOnly(isCommentOnly);
	m_scenarioManager->setCommentOnly(isCommentOnly);
	m_charactersManager->setCommentOnly(isCommentOnly);
	m_locationsManager->setCommentOnly(isCommentOnly);

	//
	// Активируем вкладки
	//
	::enableActionsOnProjectOpen();

	//
	// Настроим индикатор
	//
	if (m_projectsManager->currentProject().isRemote()) {
		if (m_projectsManager->currentProject().isSyncAvailable()) {
			aboutShowSyncActiveIndicator();
		} else {
			aboutSyncClosedWithError(201);
		}
	} else {
		m_tabs->removeIndicator();
	}

	//
	// Загрузить данные из файла
	//
	m_researchManager->loadCurrentProject();
	m_scenarioManager->loadCurrentProject();
	m_charactersManager->loadCurrentProject();
	m_locationsManager->loadCurrentProject();
	m_statisticsManager->loadCurrentProject();

	//
	// Синхронизируем проекты из облака
	//
	if (m_projectsManager->currentProject().isRemote()) {
		progress.setProgressText(QString::null, tr("Sync scenario with cloud service."));
		m_synchronizationManager->aboutFullSyncScenario();
		m_synchronizationManager->aboutFullSyncData();

		//
		// Принудительно сохраняем текст сценария
		//
		m_view->setWindowModified(true);
		aboutSave();
	}

	//
	// Запускаем обработку изменений сценария
	//
	m_scenarioManager->startChangesHandling();

	//
	// Загрузить настройки файла
	//
	m_scenarioManager->loadCurrentProjectSettings(ProjectsManager::currentProject().path());
	m_exportManager->loadCurrentProjectSettings(ProjectsManager::currentProject().path());

	//
	// Обновим название текущего проекта, т.к. данные о проекте теперь загружены
	//
	m_projectsManager->setCurrentProjectName(m_researchManager->scenarioName());

	//
	// Перейти на вкладку следующую после стартовой
	//
	m_tabs->setCurrentTab(1);

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
	// Закроем проект управляющими
	//
	m_researchManager->closeCurrentProject();
	m_scenarioManager->closeCurrentProject();

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

	//
	// Отключим некоторые действия, которые не могут быть выполнены до момента загрузки проекта
	//
	::disableActionsOnStart();
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
	m_menuSecondary->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	//
	// Настроим боковую панель
	//
	// ... основную
	//
	m_tabs->addTab(tr("Start"), QIcon(":/Graphics/Icons/start.png"));
	g_disableOnStartActions << m_tabs->addTab(tr("Research"), QIcon(":/Graphics/Icons/research.png"));
	g_disableOnStartActions << m_tabs->addTab(tr("Scenario"), QIcon(":/Graphics/Icons/script.png"));
	g_disableOnStartActions << m_tabs->addTab(tr("Characters"), QIcon(":/Graphics/Icons/characters.png"));
	g_disableOnStartActions << m_tabs->addTab(tr("Locations"), QIcon(":/Graphics/Icons/locations.png"));
	g_disableOnStartActions << m_tabs->addTab(tr("Statistics"), QIcon(":/Graphics/Icons/statistics.png"));
	m_tabs->addTab(tr("Settings"), QIcon(":/Graphics/Icons/settings.png"));
	//
	// ... вспомогательную
	//
	m_tabsSecondary->setCompactMode(true);
	m_tabsSecondary->addTab(tr("Start"), QIcon(":/Graphics/Icons/start.png"));
	g_disableOnStartActions << m_tabsSecondary->addTab(tr("Research"), QIcon(":/Graphics/Icons/research.png"));
	g_disableOnStartActions << m_tabsSecondary->addTab(tr("Scenario"), QIcon(":/Graphics/Icons/script.png"));
	g_disableOnStartActions << m_tabsSecondary->addTab(tr("Characters"), QIcon(":/Graphics/Icons/characters.png"));
	g_disableOnStartActions << m_tabsSecondary->addTab(tr("Locations"), QIcon(":/Graphics/Icons/locations.png"));
	g_disableOnStartActions << m_tabsSecondary->addTab(tr("Statistics"), QIcon(":/Graphics/Icons/statistics.png"));
	m_tabsSecondary->addTab(tr("Settings"), QIcon(":/Graphics/Icons/settings.png"));
	m_tabsSecondary->setCurrentTab(SETTINGS_TAB_INDEX);

	//
	// Настроим виджеты соответствующие первоначальным активным вкладкам
	//
	m_tabsWidgets->setObjectName("tabsWidgets");
	m_tabsWidgets->addWidget(m_startUpManager->view());
	m_tabsWidgets->addWidget(m_researchManager->view());
	m_tabsWidgets->addWidget(m_scenarioManager->view());
	m_tabsWidgets->addWidget(m_charactersManager->view());
	m_tabsWidgets->addWidget(m_locationsManager->view());
	m_tabsWidgets->addWidget(m_statisticsManager->view());
	m_tabsWidgetsSecondary->setObjectName("tabsWidgetsSecondary");
	m_tabsWidgetsSecondary->addWidget(m_settingsManager->view());

	//
	// Настроим разделитель панелей
	//
	m_splitter->setObjectName("mainWindowSplitter");
	m_splitter->setHandleWidth(4);
	m_splitter->setOpaqueResize(false);
	m_splitter->addWidget(m_tabsWidgets);
	m_splitter->addWidget(m_tabsWidgetsSecondary);
	m_splitter->setStretchFactor(1, 1);
	m_splitter->setCollapsible(0, false);
	m_splitter->setCollapsible(1, false);

	//
	// Расположим всё на форме
	//
	QVBoxLayout* leftTabsLayout = new QVBoxLayout;
	leftTabsLayout->setContentsMargins(QMargins());
	leftTabsLayout->setSpacing(0);
	leftTabsLayout->addWidget(m_menu);
	leftTabsLayout->addWidget(m_tabs);

	QVBoxLayout* rightTabsLayout = new QVBoxLayout;
	rightTabsLayout->setContentsMargins(QMargins());
	rightTabsLayout->setSpacing(0);
	rightTabsLayout->addWidget(m_menuSecondary);
	rightTabsLayout->addWidget(m_tabsSecondary);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addLayout(leftTabsLayout);
	layout->addWidget(m_splitter);
	layout->addLayout(rightTabsLayout);

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

#ifdef Q_OS_MAC
	//
	// Добавляем действие "Сохранить" в виджет главного окна,
	// чтобы в маке работал шорткат
	//
	m_view->addAction(saveProject);
#endif

	return menu;
}

void ApplicationManager::initConnections()
{
	connect(m_view, SIGNAL(wantToClose()), this, SLOT(aboutExit()));

	connect(m_menu, SIGNAL(clicked()), m_menu, SLOT(showMenu()));
	connect(m_tabs, &SideTabBar::currentChanged, this, &ApplicationManager::currentTabIndexChanged);
	connect(m_tabsSecondary, &SideTabBar::currentChanged, this, &ApplicationManager::currentTabIndexChanged);

	connect(m_projectsManager, SIGNAL(recentProjectsUpdated()), this, SLOT(aboutUpdateProjectsList()));
	connect(m_projectsManager, SIGNAL(remoteProjectsUpdated()), this, SLOT(aboutUpdateProjectsList()));

	connect(m_startUpManager, SIGNAL(loginRequested(QString,QString)), m_synchronizationManager, SLOT(aboutLogin(QString,QString)));
	connect(m_startUpManager, SIGNAL(logoutRequested()), m_synchronizationManager, SLOT(aboutLogout()));
	connect(m_startUpManager, SIGNAL(createProjectRequested()), this, SLOT(aboutCreateNew()));
	connect(m_startUpManager, SIGNAL(openProjectRequested()), this, SLOT(aboutLoad()));
	connect(m_startUpManager, SIGNAL(helpRequested()), this, SLOT(aboutShowHelp()));
	connect(m_startUpManager, SIGNAL(refreshProjectsRequested()), m_projectsManager, SLOT(refreshProjects()));
	connect(m_startUpManager, SIGNAL(refreshProjectsRequested()), m_synchronizationManager, SLOT(aboutLoadProjects()));
	connect(m_startUpManager, SIGNAL(openRecentProjectRequested(QModelIndex)), this, SLOT(aboutLoadFromRecent(QModelIndex)));
	connect(m_startUpManager, SIGNAL(openRemoteProjectRequested(QModelIndex)), this, SLOT(aboutLoadFromRemote(QModelIndex)));

	connect(m_scenarioManager, SIGNAL(showFullscreen()), this, SLOT(aboutShowFullscreen()));
	connect(m_scenarioManager, SIGNAL(scenarioChangesSaved()), m_synchronizationManager, SLOT(aboutWorkSyncScenario()));
	connect(m_scenarioManager, SIGNAL(scenarioChangesSaved()), m_synchronizationManager, SLOT(aboutWorkSyncData()));
	connect(m_scenarioManager, SIGNAL(cursorPositionUpdated(int,bool)), m_synchronizationManager, SLOT(aboutUpdateCursors(int,bool)));

	connect(m_charactersManager, SIGNAL(characterNameChanged(QString,QString)),
			m_scenarioManager, SLOT(aboutCharacterNameChanged(QString,QString)));
	connect(m_charactersManager, SIGNAL(refreshCharacters()),
			m_scenarioManager, SLOT(aboutRefreshCharacters()));

	connect(m_locationsManager, SIGNAL(locationNameChanged(QString,QString)),
			m_scenarioManager, SLOT(aboutLocationNameChanged(QString,QString)));
	connect(m_locationsManager, SIGNAL(refreshLocations()),
			m_scenarioManager, SLOT(aboutRefreshLocations()));

	connect(m_statisticsManager, SIGNAL(needNewExportedScenario()), this, SLOT(aboutPrepareScenarioForStatistics()));

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

	connect(m_researchManager, SIGNAL(researchChanged()), this, SLOT(aboutProjectChanged()));
	connect(m_scenarioManager, SIGNAL(scenarioChanged()), this, SLOT(aboutProjectChanged()));
	connect(m_charactersManager, SIGNAL(characterChanged()), this, SLOT(aboutProjectChanged()));
	connect(m_locationsManager, SIGNAL(locationChanged()), this, SLOT(aboutProjectChanged()));
	connect(m_exportManager, SIGNAL(scenarioTitleListDataChanged()), this, SLOT(aboutProjectChanged()));

	connect(m_synchronizationManager, SIGNAL(loginAccepted()),
			m_startUpManager, SLOT(aboutUserLogged()));
	connect(m_synchronizationManager, SIGNAL(logoutAccepted()),
			m_startUpManager, SLOT(aboutUserUnlogged()));
	connect(m_synchronizationManager, SIGNAL(logoutAccepted()),
			this, SLOT(aboutUserUnlogged()));
	connect(m_synchronizationManager, SIGNAL(remoteProjectsLoaded(QString)),
			m_projectsManager, SLOT(setRemoteProjects(QString)));
	connect(m_synchronizationManager, SIGNAL(applyPatchRequested(QString,bool)),
			m_scenarioManager, SLOT(aboutApplyPatch(QString,bool)));
	connect(m_synchronizationManager, SIGNAL(applyPatchesRequested(QList<QString>,bool)),
			m_scenarioManager, SLOT(aboutApplyPatches(QList<QString>,bool)));
	connect(m_synchronizationManager, SIGNAL(cursorsUpdated(QMap<QString,int>,bool)),
			m_scenarioManager, SLOT(aboutCursorsUpdated(QMap<QString,int>,bool)));
	connect(m_synchronizationManager, SIGNAL(syncClosedWithError(int,QString)),
			this, SLOT(aboutSyncClosedWithError(int,QString)));
	connect(m_synchronizationManager, SIGNAL(syncRestarted()), this, SLOT(aboutShowSyncActiveIndicator()));
}

void ApplicationManager::initStyleSheet()
{
	//
	// Загрузим стиль
	//
	QFile styleSheetFile(":/Interface/UI/style-desktop.qss");
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
	//
	m_menuSecondary->setProperty("inTopPanel", true);
	m_menuSecondary->setProperty("topPanelTopBordered", true);
	m_menuSecondary->setProperty("topPanelRightBordered", true);
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
			palette.setColor(QPalette::WindowText, QColor("#ebebeb"));
			palette.setColor(QPalette::Button, QColor("#414244"));
			palette.setColor(QPalette::ButtonText, QColor("#ebebeb"));
			palette.setColor(QPalette::Base, QColor("#404040"));
			palette.setColor(QPalette::AlternateBase, QColor("#353535"));
			palette.setColor(QPalette::Text, QColor("#ebebeb"));
			palette.setColor(QPalette::Highlight, QColor("#2b78da"));
			palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
			palette.setColor(QPalette::Light, QColor("#404040"));
			palette.setColor(QPalette::Midlight, QColor("#696765"));
			palette.setColor(QPalette::Dark, QColor("#696765"));
			palette.setColor(QPalette::Shadow, QColor("#1c2023"));

			palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#a1a1a1"));
			palette.setColor(QPalette::Disabled, QPalette::Button, QColor("#1b1e21"));
			palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#a1a1a1"));
			palette.setColor(QPalette::Disabled, QPalette::Base, QColor("#333333"));
			palette.setColor(QPalette::Inactive, QPalette::Text, QColor("#bcbdbf"));
			palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#666769"));
			palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor("#eeeeee"));
		} else {
			palette.setColor(QPalette::Window, QColor("#f6f6f6"));
			palette.setColor(QPalette::WindowText, QColor("#38393a"));
			palette.setColor(QPalette::Button, QColor("#e4e4e4"));
			palette.setColor(QPalette::ButtonText, QColor("#38393a"));
			palette.setColor(QPalette::Base, QColor("#ffffff"));
			palette.setColor(QPalette::AlternateBase, QColor("#eeeeee"));
			palette.setColor(QPalette::Text, QColor("#38393a"));
			palette.setColor(QPalette::Highlight, QColor("#2b78da"));
			palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
			palette.setColor(QPalette::Light, QColor("#ffffff"));
			palette.setColor(QPalette::Midlight, QColor("#d6d6d6"));
			palette.setColor(QPalette::Dark, QColor("#bdbebf"));
			palette.setColor(QPalette::Mid, QColor("#a0a2a4"));
			palette.setColor(QPalette::Shadow, QColor("#585a5c"));

			palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#acadaf"));
			palette.setColor(QPalette::Disabled, QPalette::Button, QColor("#f6f6f6"));
			palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#acadaf"));
			palette.setColor(QPalette::Inactive, QPalette::Text, QColor("#595a5c"));
			palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#acadaf"));
			palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor("#eeeeee"));
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

	//
	// Разделение экрана на две панели
	//
	const bool twoPanelsMode =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/two-panel-mode",
				DataStorageLayer::SettingsStorage::ApplicationSettings)
			.toInt();
	m_menuSecondary->setVisible(twoPanelsMode);
	m_tabsSecondary->setVisible(twoPanelsMode);
	m_tabsWidgetsSecondary->setVisible(twoPanelsMode);
	m_splitter->handle(1)->setEnabled(twoPanelsMode);
	m_splitter->setHandleWidth(twoPanelsMode ? 4 : 0);
	m_splitter->setSizes(QList<int>() << 1 << (twoPanelsMode ? 1 : 0));

	//
	// Активация/деактивация модулей модули
	//
	const bool showResearchModule =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/modules/research",
				DataStorageLayer::SettingsStorage::ApplicationSettings)
			.toInt();
	m_tabs->tab(RESEARCH_TAB_INDEX)->setVisible(showResearchModule);
	m_tabsSecondary->tab(RESEARCH_TAB_INDEX)->setVisible(showResearchModule);
	//
	const bool showScenarioModule =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/modules/scenario",
				DataStorageLayer::SettingsStorage::ApplicationSettings)
			.toInt();
	m_tabs->tab(SCENARIO_TAB_INDEX)->setVisible(showScenarioModule);
	m_tabsSecondary->tab(SCENARIO_TAB_INDEX)->setVisible(showScenarioModule);
	//
	const bool showCharactersModule =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/modules/characters",
				DataStorageLayer::SettingsStorage::ApplicationSettings)
			.toInt();
	m_tabs->tab(CHARACTERS_TAB_INDEX)->setVisible(showCharactersModule);
	m_tabsSecondary->tab(CHARACTERS_TAB_INDEX)->setVisible(showCharactersModule);
	//
	const bool showLocationsModule =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/modules/locations",
				DataStorageLayer::SettingsStorage::ApplicationSettings)
			.toInt();
	m_tabs->tab(LOCATIONS_TAB_INDEX)->setVisible(showLocationsModule);
	m_tabsSecondary->tab(LOCATIONS_TAB_INDEX)->setVisible(showLocationsModule);
	//
	const bool showStatisticsModule =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/modules/statistics",
				DataStorageLayer::SettingsStorage::ApplicationSettings)
			.toInt();
	m_tabs->tab(STATISTICS_TAB_INDEX)->setVisible(showStatisticsModule);
	m_tabsSecondary->tab(STATISTICS_TAB_INDEX)->setVisible(showStatisticsModule);
}

void ApplicationManager::updateWindowTitle()
{
	const QString projectFileName = ProjectsManager::currentProject().name();
#ifdef Q_OS_MAC
	m_view->setWindowTitle(projectFileName);
#else
	m_view->setWindowTitle(tr("%1[*] - KIT Scenarist").arg(projectFileName));
#endif
}
