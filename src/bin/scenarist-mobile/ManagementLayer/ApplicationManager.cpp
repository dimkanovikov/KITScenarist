#include "ApplicationManager.h"

#include "Project/ProjectsManager.h"
#include "Menu/MenuManager.h"
#include "StartUp/StartUpManager.h"
#include "Scenario/ScenarioManager.h"
//#include "Characters/CharactersManager.h"
//#include "Locations/LocationsManager.h"
//#include "Statistics/StatisticsManager.h"
//#include "Settings/SettingsManager.h"
//#include "Import/ImportManager.h"
//#include "Export/ExportManager.h"
#include "Synchronization/SynchronizationManager.h"

#include <UserInterfaceLayer/ApplicationView.h>

#include <DataLayer/Database/Database.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/ProgressWidget/ProgressWidget.h>
#include <3rd_party/Helpers/StyleSheetHelper.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>
#include <3rd_party/Widgets/WAF/Animation.h>

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>

using namespace ManagementLayer;
using UserInterface::ApplicationView;

namespace {
	/**
	 * @brief Индексы представлений в общем стэке
	 */
	/** @{ */
	const int PROJECTS_VIEW_INDEX = 0;
	const int PROJECT_TEXT_VIEW_INDEX = 1;
	/** @} */

	/**
	 * @brief Расширение файла проекта сценария
	 */
	const QString PROJECT_FILE_EXTENSION = ".kitsp"; // kit scenarist project
	const bool SYNC_UNAVAILABLE = false;

	/**
	 * @brief Обновить состояние изменённости текущего проекта
	 */
	static void updateWindowModified(QWidget* _widget, bool _modified) {
		_widget->setWindowModified(_modified);
	}
}


ApplicationManager::ApplicationManager(QObject *parent) :
	QObject(parent),
	m_view(new ApplicationView),
	m_projectsManager(new ProjectsManager(this)),
	m_menuManager(new MenuManager(this, m_view)),
	m_startUpManager(new StartUpManager(this, m_view)),
	m_scenarioManager(new ScenarioManager(this, m_view)),
	m_synchronizationManager(new SynchronizationManager(this, m_view))
	/*,
	m_charactersManager(new CharactersManager(this, m_view)),
	m_locationsManager(new LocationsManager(this, m_view)),
	m_statisticsManager(new StatisticsManager(this, m_view)),
	m_settingsManager(new SettingsManager(this, m_view)),
	m_importManager(new ImportManager(this, m_view)),
	m_exportManager(new ExportManager(this, m_view))*/
{
	initView();
	initConnections();
	initStyleSheet();

	aboutUpdateProjectsList();

//	reloadApplicationSettings();

	QTimer::singleShot(0, m_synchronizationManager, &SynchronizationManager::login);
}

ApplicationManager::~ApplicationManager()
{
	delete m_view;
	m_view = 0;
}

void ApplicationManager::exec()
{
//	loadViewState();
	m_view->show();
}

void ApplicationManager::aboutUpdateProjectsList()
{
	m_startUpManager->setRecentProjects(m_projectsManager->recentProjects());
	m_startUpManager->setRemoteProjects(m_projectsManager->remoteProjects());
}

void ApplicationManager::createNewProject(const QString& _projectName)
{
	//
	// Спросить у пользователя хочет ли он сохранить проект
	//
	if (saveIfNeeded()) {
		//
		// Получим имя файла для нового проекта
		//
        const QDir projectsDir(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).last());
		if (!projectsDir.exists()) {
			QDir::root().mkpath(projectsDir.absolutePath());
		}
		QString newProjectFileName = projectsDir.absoluteFilePath(_projectName);

		//
		// Если файл выбран
		//
		if (!newProjectFileName.isEmpty()) {
			//
			// ... закроем текущий проект
			//
			closeCurrentProject();

			//
			// ... если файл с таким именем существует, добавляем суффикс в название нового проекта
			//
			if (QFile::exists(newProjectFileName + PROJECT_FILE_EXTENSION)) {
				int copyIndex = 0;
				QString newProjectFileNameCorrected;
				do {
					++copyIndex;
					newProjectFileNameCorrected =
						QString("%1%2%3").arg(newProjectFileName).arg(copyIndex).arg(PROJECT_FILE_EXTENSION);
				} while (QFile::exists(newProjectFileNameCorrected));
				newProjectFileName = newProjectFileNameCorrected;
			} else {
				newProjectFileName += PROJECT_FILE_EXTENSION;
			}

			//
			// ... создаём новую базу данных в файле и делаем её текущим проектом
			//
			m_projectsManager->setCurrentProject(newProjectFileName);

			//
			// ... перейдём к редактированию
			//
			goToEditCurrentProject();
		}
	}
}

void ApplicationManager::saveProject()
{
	//
	// Делаем это вручную, т.к. приложение не всегда корректно закрывается
	//
	QSettings().sync();

	//
	// Если какие-то данные изменены
	//
	if (m_view->isWindowModified()) {
		//
		// Управляющие должны сохранить несохранённые данные
		//
		DatabaseLayer::Database::transaction();
		m_scenarioManager->saveCurrentProject();
//		m_charactersManager->saveCharacters();
//		m_locationsManager->saveLocations();
		DatabaseLayer::Database::commit();

//		//
//		// Для проекта из облака отправляем данные на сервер
//		//
//		if (m_projectsManager->currentProject().isRemote()) {
//			m_synchronizationManager->aboutWorkSyncScenario();
//			m_synchronizationManager->aboutWorkSyncData();
//		}

		//
		// Изменим статус окна на сохранение изменений
		//
		::updateWindowModified(m_view, false);
	}
}

//void ApplicationManager::aboutLoad(const QString& _fileName)
//{
//	//
//	// Если нужно сохранить проект
//	//
//	if (saveIfNeeded()) {
//		//
//		// Имя файла для загрузки
//		//
//		QString loadProjectFileName = _fileName;

//		//
//		// Если имя файла не определено, выберем его в диалоге выбора файла
//		//
//		if (loadProjectFileName.isEmpty()) {
//			loadProjectFileName =
//					QFileDialog::getOpenFileName(
//						m_view,
//						tr("Choose project file to open"),
//						projectsFolderPath(),
//						tr ("Scenarist project files (*%1)").arg(PROJECT_FILE_EXTENSION)
//						);
//		}

//		//
//		// Если файл выбран
//		//
//		if (!loadProjectFileName.isEmpty()) {
//			//
//			// ... закроем текущий проект
//			//
//			closeCurrentProject();

//			//
//			// ... переключаемся на работу с выбранным файлом
//			//
//			m_projectsManager->setCurrentProject(loadProjectFileName);

//			//
//			// ... сохраняем путь
//			//
//			saveProjectsFolderPath(loadProjectFileName);

//			//
//			// ... перейдём к редактированию
//			//
//			goToEditCurrentProject();
//		}

//		//
//		// Изменим статус окна на сохранение изменений
//		//
//		::updateWindowModified(m_view, false);
//	}
//}

//void ApplicationManager::aboutShowHelp()
//{
//	QDesktopServices::openUrl(QUrl("https://kitscenarist.ru/help/"));
//}

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

//void ApplicationManager::aboutShowSyncActiveIndicator()
//{
//	m_tabs->addIndicator(QIcon(":/Graphics/Icons/Indicator/connected.png"), tr("Connection active"), tr("Project sinchronized"));
//}

void ApplicationManager::syncClosedWithError(int _errorCode, const QString& _error)
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
			m_menuManager->retryLogin(error);
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

//	//
//	// Сигнализируем об ошибке
//	//
//	m_tabs->addIndicator(QIcon(":/Graphics/Icons/Indicator/disconnected.png"), title, error);

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
			m_menuManager->userLogged();
			m_startUpManager->showRemoteProjects();
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

//void ApplicationManager::aboutImport()
//{
//	m_importManager->importScenario(m_scenarioManager->scenario(), m_scenarioManager->cursorPosition());
//}

//void ApplicationManager::aboutExportTo()
//{
//	m_exportManager->exportScenario(m_scenarioManager->scenario());
//}

//void ApplicationManager::aboutPrintPreview()
//{
//	m_exportManager->printPreviewScenario(m_scenarioManager->scenario());
//}

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
		progress.showProgress(tr("Exit from Application"), tr("Closing Database Connections and Remove Temporatry Files."));

		//
		// Закроем текущий проект
		//
		closeCurrentProject();

		//
		// Сохраняем состояния виджетов
		//
//		saveViewState();

		//
		// Выходим
		//
		progress.close();
		QApplication::processEvents();
		QApplication::quit();
	}
}

//void ApplicationManager::aboutApplicationSettingsUpdated()
//{
//	reloadApplicationSettings();
//}

void ApplicationManager::aboutProjectChanged()
{
	::updateWindowModified(m_view, true);

//	m_statisticsManager->scenarioTextChanged();
}

//void ApplicationManager::aboutShowFullscreen()
//{
//	const char* IS_MAXIMIZED_PROPERTY = "isMaximized";

//	if (m_view->isFullScreen()) {
//		//
//		// Возвращаемся в состояние предшествовавшее полноэкранному режиму
//		//
//		if (m_view->property(IS_MAXIMIZED_PROPERTY).toBool()) {
//			m_view->showMaximized();
//		} else {
//			m_view->showNormal();
//		}
//		m_menu->show();
//		m_tabs->show();
//	} else {
//		//
//		// Сохраним состояние окна перед переходом в полноэкранный режим
//		//
//		m_view->setProperty(IS_MAXIMIZED_PROPERTY, m_view->windowState().testFlag(Qt::WindowMaximized));

//		//
//		// Переходим в полноэкранный режим
//		//
//		m_tabsWidgets->setCurrentWidget(m_scenarioManager->view());
//		m_menu->hide();
//		m_tabs->hide();
//		m_view->showFullScreen();
//	}
//}

//void ApplicationManager::aboutPrepareScenarioForStatistics()
//{
//	m_statisticsManager->setExportedScenario(m_scenarioManager->scenario()->document());
//}

//void ApplicationManager::loadViewState()
//{
//	DataStorageLayer::StorageFacade::settingsStorage()->loadApplicationStateAndGeometry(m_view);
//}

//void ApplicationManager::saveViewState()
//{
//	DataStorageLayer::StorageFacade::settingsStorage()->saveApplicationStateAndGeometry(m_view);
//}

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
				saveProject();
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
	m_scenarioManager->setCommentOnly(isCommentOnly);
//	m_charactersManager->setCommentOnly(isCommentOnly);
//	m_locationsManager->setCommentOnly(isCommentOnly);

//	//
//	// Настроим индикатор
//	//
//	if (m_projectsManager->currentProject().isRemote()) {
//		if (m_projectsManager->currentProject().isSyncAvailable()) {
//			aboutShowSyncActiveIndicator();
//		} else {
//			aboutSyncClosedWithError(201);
//		}
//	} else {
//		m_tabs->removeIndicator();
//	}

	//
	// Загрузить данные из файла
	//
	m_scenarioManager->loadCurrentProject();
//	m_charactersManager->loadCurrentProject();
//	m_locationsManager->loadCurrentProject();
//	m_statisticsManager->loadCurrentProject();

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
		saveProject();
	}

	//
	// Запускаем обработку изменений сценария
	//
	m_scenarioManager->startChangesHandling();

	//
	// Загрузить настройки файла
	//
	m_scenarioManager->loadCurrentProjectSettings(ProjectsManager::currentProject().path());
//	m_exportManager->loadCurrentProjectSettings(ProjectsManager::currentProject().path());

	//
	// Обновим название текущего проекта, т.к. данные о проекте теперь загружены
	//
	m_projectsManager->setCurrentProjectName(m_scenarioManager->scenarioName());

	//
	// Активируем меню для проекта
	//
	m_menuManager->showProjectSubmenu(ProjectsManager::currentProject().displayName());

	//
	// Перейти на вкладку редактирования сценария
	//
	m_view->setCurrentView(PROJECT_TEXT_VIEW_INDEX);

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
//	m_exportManager->saveCurrentProjectSettings(ProjectsManager::currentProject().path());

	//
	// Закроем проект управляющими
	//
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
	m_menuManager->hideProjectSubmenu();
}

void ApplicationManager::initView()
{
	//
	// Настроим представления
	//
	m_view->addView(m_startUpManager->toolbar(), m_startUpManager->view());
	m_view->addView(m_scenarioManager->toolbar(), m_scenarioManager->view());
}

void ApplicationManager::initConnections()
{
	connect(m_view, &ApplicationView::menuClicked, m_menuManager, &MenuManager::showMenu);

	connect(m_menuManager, &MenuManager::loginRequested, m_synchronizationManager, &SynchronizationManager::aboutLogin);
	connect(m_menuManager, &MenuManager::logoutRequested, m_synchronizationManager, &SynchronizationManager::aboutLogout);
	connect(m_menuManager, &MenuManager::projectsRequested, [=](){
		m_view->setCurrentView(PROJECTS_VIEW_INDEX);
	});
	connect(m_menuManager, &MenuManager::projectSaveRequested, this, &ApplicationManager::saveProject);
	connect(m_menuManager, &MenuManager::projectTextRequested, [=](){
		m_view->setCurrentView(PROJECT_TEXT_VIEW_INDEX);
	});

	connect(m_view, &ApplicationView::wantToClose, this, &ApplicationManager::aboutExit);

	connect(m_projectsManager, &ProjectsManager::recentProjectsUpdated, this, &ApplicationManager::aboutUpdateProjectsList);
	connect(m_projectsManager, &ProjectsManager::remoteProjectsUpdated, this, &ApplicationManager::aboutUpdateProjectsList);

	connect(m_startUpManager, &StartUpManager::createProjectRequested, this, &ApplicationManager::createNewProject);
	connect(m_startUpManager, &StartUpManager::openRecentProjectRequested, this, &ApplicationManager::aboutLoadFromRecent);
	connect(m_startUpManager, &StartUpManager::openRemoteProjectRequested, this, &ApplicationManager::aboutLoadFromRemote);

	connect(m_scenarioManager, &ScenarioManager::scenarioChangesSaved, m_synchronizationManager, &SynchronizationManager::aboutWorkSyncScenario);
	connect(m_scenarioManager, &ScenarioManager::scenarioChangesSaved, m_synchronizationManager, &SynchronizationManager::aboutWorkSyncData);
	connect(m_scenarioManager, &ScenarioManager::cursorPositionUpdated, m_synchronizationManager, &SynchronizationManager::aboutUpdateCursors);

//	connect(m_charactersManager, SIGNAL(characterNameChanged(QString,QString)),
//			m_scenarioManager, SLOT(aboutCharacterNameChanged(QString,QString)));
//	connect(m_charactersManager, SIGNAL(refreshCharacters()),
//			m_scenarioManager, SLOT(aboutRefreshCharacters()));

//	connect(m_locationsManager, SIGNAL(locationNameChanged(QString,QString)),
//			m_scenarioManager, SLOT(aboutLocationNameChanged(QString,QString)));
//	connect(m_locationsManager, SIGNAL(refreshLocations()),
//			m_scenarioManager, SLOT(aboutRefreshLocations()));

//	connect(m_statisticsManager, SIGNAL(needNewExportedScenario()), this, SLOT(aboutPrepareScenarioForStatistics()));

//	connect(m_settingsManager, SIGNAL(applicationSettingsUpdated()),
//			this, SLOT(aboutApplicationSettingsUpdated()));
//	connect(m_settingsManager, SIGNAL(scenarioEditSettingsUpdated()),
//			m_scenarioManager, SLOT(aboutTextEditSettingsUpdated()));
//	connect(m_settingsManager, SIGNAL(navigatorSettingsUpdated()),
//			m_scenarioManager, SLOT(aboutNavigatorSettingsUpdated()));
//	connect(m_settingsManager, SIGNAL(chronometrySettingsUpdated()),
//			m_scenarioManager, SLOT(aboutChronometrySettingsUpdated()));
//	connect(m_settingsManager, SIGNAL(countersSettingsUpdated()),
//			m_scenarioManager, SLOT(aboutCountersSettingsUpdated()));

//	connect(m_exportManager, SIGNAL(scenarioNameChanged(QString)),
//			m_scenarioManager, SLOT(aboutScenarioNameChanged(QString)));

	connect(m_scenarioManager, &ScenarioManager::scenarioChanged, this, &ApplicationManager::aboutProjectChanged);
//	connect(m_charactersManager, SIGNAL(characterChanged()), this, SLOT(aboutProjectChanged()));
//	connect(m_locationsManager, SIGNAL(locationChanged()), this, SLOT(aboutProjectChanged()));
//	connect(m_exportManager, SIGNAL(scenarioTitleListDataChanged()), this, SLOT(aboutProjectChanged()));

	connect(m_synchronizationManager, &SynchronizationManager::loginAccepted, [=](){
		//
		// Покажем в меню, под каким логином авторизован пользователь
		//
		m_menuManager->userLogged();

		//
		// Покажем список проектов из облака
		//
		m_startUpManager->showRemoteProjects();
	});
	connect(m_synchronizationManager, &SynchronizationManager::logoutAccepted, [=](){
		//
		// Закрываем проект, если он из облака
		//
		if (m_projectsManager->currentProject().isRemote()) {
			closeCurrentProject();
		}

		//
		// Корректируем меню
		//
		m_menuManager->userUnlogged();

		//
		// Скрываем проекты из облака на экране проектов
		//
		m_startUpManager->hideRemoteProjects();
	});
	connect(m_synchronizationManager, &SynchronizationManager::remoteProjectsLoaded, m_projectsManager, &ProjectsManager::setRemoteProjects);
	connect(m_synchronizationManager, &SynchronizationManager::applyPatchRequested, m_scenarioManager, &ScenarioManager::aboutApplyPatch);
	connect(m_synchronizationManager, &SynchronizationManager::applyPatchesRequested, m_scenarioManager, &ScenarioManager::aboutApplyPatches);
	connect(m_synchronizationManager, &SynchronizationManager::cursorsUpdated, m_scenarioManager, &ScenarioManager::aboutCursorsUpdated);
	connect(m_synchronizationManager, &SynchronizationManager::syncClosedWithError, this, &ApplicationManager::syncClosedWithError);
//	connect(m_synchronizationManager, SIGNAL(syncRestarted()), this, SLOT(aboutShowSyncActiveIndicator()));
}

void ApplicationManager::initStyleSheet()
{
	//
	// Загрузим стиль
	//
	QFile styleSheetFile(":/Interface/UI/style-mobile.qss");
	styleSheetFile.open(QIODevice::ReadOnly);
	QString styleSheet = styleSheetFile.readAll();
	styleSheetFile.close();

	//
	// Установим стиль на главный виджет приложения
	//
	m_view->setStyleSheet(StyleSheetHelper::computeDeviceInpedendentSize(styleSheet));
}

//void ApplicationManager::reloadApplicationSettings()
//{
//	//
//	// Внешний вид приложения
//	//
//	bool useDarkTheme =
//			DataStorageLayer::StorageFacade::settingsStorage()->value(
//				"application/use-dark-theme",
//				DataStorageLayer::SettingsStorage::ApplicationSettings)
//			.toInt();

//	{
//		//
//		// Настраиваем палитру и стилевые надстройки в зависимости от темы
//		//
//		QPalette palette = QStyleFactory::create("Fusion")->standardPalette();
//		QString styleSheet;

//		if (useDarkTheme) {
//			palette.setColor(QPalette::Window, QColor("#26282a"));
//			palette.setColor(QPalette::WindowText, QColor("#ebebeb"));
//			palette.setColor(QPalette::Button, QColor("#414244"));
//			palette.setColor(QPalette::ButtonText, QColor("#ebebeb"));
//			palette.setColor(QPalette::Base, QColor("#404040"));
//			palette.setColor(QPalette::AlternateBase, QColor("#353535"));
//			palette.setColor(QPalette::Text, QColor("#ebebeb"));
//			palette.setColor(QPalette::Highlight, QColor("#2b78da"));
//			palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
//			palette.setColor(QPalette::Light, QColor("#404040"));
//			palette.setColor(QPalette::Midlight, QColor("#696765"));
//			palette.setColor(QPalette::Dark, QColor("#696765"));
//			palette.setColor(QPalette::Shadow, QColor("#1c2023"));

//			palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#a1a1a1"));
//			palette.setColor(QPalette::Disabled, QPalette::Button, QColor("#1b1e21"));
//			palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#a1a1a1"));
//			palette.setColor(QPalette::Disabled, QPalette::Base, QColor("#333333"));
//			palette.setColor(QPalette::Inactive, QPalette::Text, QColor("#bcbdbf"));
//			palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#666769"));
//			palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor("#eeeeee"));
//		} else {
//			palette.setColor(QPalette::Window, QColor("#f6f6f6"));
//			palette.setColor(QPalette::WindowText, QColor("#38393a"));
//			palette.setColor(QPalette::Button, QColor("#e4e4e4"));
//			palette.setColor(QPalette::ButtonText, QColor("#38393a"));
//			palette.setColor(QPalette::Base, QColor("#ffffff"));
//			palette.setColor(QPalette::AlternateBase, QColor("#eeeeee"));
//			palette.setColor(QPalette::Text, QColor("#38393a"));
//			palette.setColor(QPalette::Highlight, QColor("#2b78da"));
//			palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
//			palette.setColor(QPalette::Light, QColor("#ffffff"));
//			palette.setColor(QPalette::Midlight, QColor("#d6d6d6"));
//			palette.setColor(QPalette::Dark, QColor("#bdbebf"));
//			palette.setColor(QPalette::Mid, QColor("#a0a2a4"));
//			palette.setColor(QPalette::Shadow, QColor("#585a5c"));

//			palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#acadaf"));
//			palette.setColor(QPalette::Disabled, QPalette::Button, QColor("#f6f6f6"));
//			palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#acadaf"));
//			palette.setColor(QPalette::Inactive, QPalette::Text, QColor("#595a5c"));
//			palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#acadaf"));
//			palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor("#eeeeee"));
//		}

//		//
//		// Для всплывающей используем универсальный стиль
//		//
//		styleSheet += "QToolTip { color: palette(window-text); background-color: palette(window); border: 1px solid palette(highlight); } "
//					 ;

//		//
//		// Применяем тему
//		//
//		qApp->setPalette(palette);
//		qApp->setStyleSheet(styleSheet);
//	}

//	//
//	// Автосохранение
//	//
//	bool autosave =
//			DataStorageLayer::StorageFacade::settingsStorage()->value(
//				"application/autosave",
//				DataStorageLayer::SettingsStorage::ApplicationSettings)
//			.toInt();
//	int autosaveInterval =
//			DataStorageLayer::StorageFacade::settingsStorage()->value(
//				"application/autosave-interval",
//				DataStorageLayer::SettingsStorage::ApplicationSettings)
//			.toInt();

//	m_autosaveTimer.stop();
//	m_autosaveTimer.disconnect();
//	if (autosave) {
//		connect(&m_autosaveTimer, SIGNAL(timeout()), this, SLOT(aboutSave()));
//		m_autosaveTimer.start(autosaveInterval * 60 * 1000); // Переводим минуты в миллисекунды
//	}

//	//
//	// Создание резервных копий
//	//
//	bool saveBackups =
//			DataStorageLayer::StorageFacade::settingsStorage()->value(
//				"application/save-backups",
//				DataStorageLayer::SettingsStorage::ApplicationSettings)
//			.toInt();
//	const QString saveBackupsFolder =
//			DataStorageLayer::StorageFacade::settingsStorage()->value(
//				"application/save-backups-folder",
//				DataStorageLayer::SettingsStorage::ApplicationSettings);
//	m_backupHelper.setIsActive(saveBackups);
//	m_backupHelper.setBackupDir(saveBackupsFolder);
//}

void ApplicationManager::updateWindowTitle()
{
	QString projectFileName = ProjectsManager::currentProject().path();
	projectFileName = projectFileName.split(QDir::separator()).last();
#ifdef Q_OS_MAC
	m_view->setWindowTitle(projectFileName);
#else
	m_view->setWindowTitle(tr("%1[*] - KIT Scenarist").arg(projectFileName));
#endif
}
