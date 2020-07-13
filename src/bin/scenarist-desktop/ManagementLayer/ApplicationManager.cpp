#include "ApplicationManager.h"
#include "MenuManager.h"
#include "StartUp/StartUpManager.h"
#include "Research/ResearchManager.h"
#include "Scenario/ScenarioCardsManager.h"
#include "Scenario/ScenarioManager.h"
#include "Statistics/StatisticsManager.h"
#include "Tools/ToolsManager.h"
#include "Settings/SettingsManager.h"
#include "Import/ImportManager.h"
#include "Export/ExportManager.h"

#include <ManagementLayer/Project/ProjectsManager.h>
#include <ManagementLayer/Synchronization/SynchronizationManager.h>
#include <ManagementLayer/Synchronization/Sync.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/Export/PdfExporter.h>

#include <Domain/ScenarioChange.h>

#include <DataLayer/Database/Database.h>
#include <DataLayer/DataStorageLayer/DatabaseHistoryStorage.h>
#include <DataLayer/DataStorageLayer/ScenarioChangeStorage.h>
#include <DataLayer/DataStorageLayer/ScriptVersionStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>

#include <UserInterfaceLayer/Application/ApplicationView.h>
#include <UserInterfaceLayer/Application/MenuView.h>
#include <UserInterfaceLayer/Project/AddProjectDialog.h>
#include <UserInterfaceLayer/Project/ProjectVersionDialog.h>
#include <UserInterfaceLayer/Project/ShareDialog.h>
#include <UserInterfaceLayer/ScenarioNavigator/ScenarioNavigatorItemDelegate.h>

#include <3rd_party/Helpers/RunOnce.h>
#include <3rd_party/Helpers/TextUtils.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/SideBar/SideBar.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.h>
#include <3rd_party/Widgets/WAF/Animation/Animation.h>

#include <QApplication>
#include <QComboBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QProcess>
#include <QScreen>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QStorageInfo>
#include <QStyle>
#include <QStyleFactory>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

#include <QtConcurrentRun>

#include <functional>

using namespace ManagementLayer;
using UserInterface::ApplicationView;
using UserInterface::AddProjectDialog;
using UserInterface::MenuView;
using UserInterface::ShareDialog;

namespace {
    /**
     * @brief Номера пунктов меню
     */
    /** @{ */
    const int kStartNewVersionMenuIndex = 5;
    const int kImportMenuIndex = 6;
    const int kExportMenuIndex = 7;
    const int kTwoPanelModeMenuIndex = 9;
    /** @} */

    /**
     * @brief Номера вкладок
     */
    /** @{ */
    const int STARTUP_TAB_INDEX = 0;
    const int RESEARCH_TAB_INDEX = 1;
    const int SCENARIO_CARDS_TAB_INDEX = 2;
    const int SCENARIO_TAB_INDEX = 3;
    const int STATISTICS_TAB_INDEX = 4;
    const int TOOLS_TAB_INDEX = 5;
    const int SETTINGS_TAB_INDEX = 6;
    /** @} */

    /**
     * @brief Расширения файлов проекта
     */
    const QString kProjectFleExtension = ".kitsp"; // kit scenarist project

    /**
     * @brief Старый вордовский формат не поддерживается
     */
    const QString kMsDocExtension = ".doc";

    /**
     * @brief Суффикс "изменено" для заголовка окна добавляемый в маке
     */
#ifdef Q_OS_MAC
    const char* MAC_CHANGED_SUFFIX =
            QT_TRANSLATE_NOOP("ManagementLayer::ApplicationManager", " - changed");
#endif

    /**
     * @brief Флаги доступности синхронизации
     */
    /** @{ */
    const bool SYNC_AVAILABLE = true;
    const bool SYNC_UNAVAILABLE = false;
    /** @} */

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
            projectsFolderPath = ProjectsManager::defaultLocation();
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
        if (!ProjectsManager::currentProject().isWritable()) {
            return;
        }

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

    /**
     * @brief Получить язык для подстановки в ссылки на сайте
     */
    static QString urlLanguage() {
        switch (QLocale().language()) {
            case QLocale::Russian:
            case QLocale::Ukrainian:
            case QLocale::Kazakh: {
                return QString();
            }

            default: {
                return "en/";
            }
        }
    }
}


ApplicationManager::ApplicationManager(QObject *parent) :
    QObject(parent),
    m_view(new ApplicationView),
    m_menu(new FlatButton(m_view)),
    m_menuSecondary(new QLabel(m_view)),
    m_tabs(new SideTabBar(m_view)),
    m_tabsSecondary(new SideTabBar(m_view)),
    m_tabsWidgets(new QStackedWidget(m_view)),
    m_tabsWidgetsSecondary(new QStackedWidget(m_view)),
    m_splitter(new QSplitter(m_view)),
    m_projectsManager(new ProjectsManager(this)),
    m_menuManager(new MenuManager(this, m_view)),
    m_startUpManager(new StartUpManager(this, m_view)),
    m_researchManager(new ResearchManager(this, m_view)),
    m_scenarioManager(new ScenarioManager(this, m_view)),
    m_statisticsManager(new StatisticsManager(this, m_view)),
    m_toolsManager(new ToolsManager(this, m_view)),
    m_settingsManager(new SettingsManager(this, m_view)),
    m_importManager(new ImportManager(this, m_view)),
    m_exportManager(new ExportManager(this, m_view)),
    m_synchronizationManager(new SynchronizationManager(this, m_view))
{
    initControllers();
    initView();
    initConnections();
    initStyleSheet();

    aboutUpdateProjectsLists();
}

ApplicationManager::~ApplicationManager()
{
    m_view->deleteLater();

#ifdef Q_OS_MAC
    m_menuBar->deleteLater();
#endif
}

void ApplicationManager::exec(const QString& _fileToOpen)
{
    //
    // Настроим приложение
    //
    reloadApplicationSettings();
    loadViewState();

    //
    // Если были авторизованы покажем информацию из кэша
    //
    m_synchronizationManager->fakeLogin();

    //
    // Покажем приложение
    //
    m_view->show();

    //
    // При необходимости откроем проект поданный в качестве аргументов
    //
    if (!_fileToOpen.isEmpty()) {
        aboutLoad(_fileToOpen);
    }

    //
    // Переводим состояние приложение в рабочий режим
    //
    m_state = ApplicationState::Working;
}

void ApplicationManager::openFile(const QString &_fileToOpen)
{
    aboutLoad(_fileToOpen);
}

void ApplicationManager::makeStartUpChecks()
{
    //
    // Работаем с отчётами об ошибке
    //
    m_startUpManager->checkCrashReports();

    //
    // Проверяем обновления
    //
    m_startUpManager->checkNewVersion();

    //
    // И авторизуемся
    //
    m_menuManager->setProgressLoginLabel(true);
    if (!m_synchronizationManager->autoLogin()) {
        m_menuManager->setProgressLoginLabel(false);
    }
}

void ApplicationManager::aboutUpdateProjectsLists()
{
    updateRecentProjectsList();
    updateRemoteProjectsList();
}

void ApplicationManager::updateRecentProjectsList()
{
    m_startUpManager->setRecentProjects(m_projectsManager->recentProjects());
}

void ApplicationManager::updateRemoteProjectsList()
{
    m_startUpManager->setRemoteProjects(m_projectsManager->remoteProjects());
}

void ApplicationManager::aboutCreateNew()
{
    //
    // Спросить у пользователя хочет ли он сохранить проект
    //
    if (saveIfNeeded()) {
        AddProjectDialog dlg(m_view, m_synchronizationManager->isInternetConnectionActive());
        dlg.setIsRemoteAvailable(m_synchronizationManager->isLogged(),
                                 m_synchronizationManager->isSubscriptionActive(),
                                 !m_startUpManager->isOnLocalProjectsTab());

        while (dlg.exec() != AddProjectDialog::Rejected) {
            //
            // Если пользователь добавляет локальный проект
            //
            if (dlg.isLocal()) {
                //
                // Путь к файлу проекта должен быть обязательно задан
                //
                if (!dlg.projectFilePath().isEmpty()) {
                    //
                    // Старый вордовский формат не поддерживаем
                    //
                    if (dlg.importFilePath().toLower().endsWith(kMsDocExtension)) {
                        QLightBoxMessage::critical(&dlg, tr("File format not supported"),
                            tr("Microsoft <b>DOC</b> files are not supported. You need save it to <b>DOCX</b> file and reimport."));
                    }
                    //
                    // Если всё в порядке
                    //
                    else {
                        //
                        // ... то создаём новый проект в заданном файле
                        //
                        const QString newProjectPath = dlg.projectFilePath();
                        const QString importFilePath = dlg.importFilePath();
                        createNewLocalProject(newProjectPath, importFilePath);
                        break;
                    }
                }
            }
            //
            // Проект в облаке
            //
            else {
                if (!dlg.projectName().isEmpty()) {
                    //
                    // Если задано имя, то создаём новый проект в облаке
                    //
                    const QString newProjectName = dlg.projectName();
                    const QString importFilePath = dlg.importFilePath();
                    createNewRemoteProject(newProjectName, importFilePath);
                    break;
                }
            }
        }
    }
}

void ApplicationManager::createNewLocalProject(const QString& _filePath, const QString& _importFilePath)
{
    QString newProjectFilePath = _filePath;

    //
    // Если файл выбран
    //
    if (!newProjectFilePath.isEmpty()) {
        //
        // ... закроем текущий проект
        //
        closeCurrentProject();

        //
        // ... установим расширение, если не задано
        //
        if (!newProjectFilePath.endsWith(kProjectFleExtension)) {
            newProjectFilePath.append(kProjectFleExtension);
        }

        //
        // ... папки, в которую пользователь хочет писать может и не быть,
        //     создадим на всякий случай, чтобы его не мучать
        //
        QDir::root().mkpath(QFileInfo(newProjectFilePath).absolutePath());

        //
        // ... проверяем, можем ли мы писать в выбранный файл
        //
        QFile file(newProjectFilePath);
        const bool canWrite = file.open(QIODevice::WriteOnly);
        file.close();

        //
        // Если возможна запись в файл
        //
        if (canWrite) {
            //
            // ... если файл существовал, удалим его для удаления данных в нём
            //
            if (QFile::exists(newProjectFilePath)) {
                QFile::remove(newProjectFilePath);
            }
            //
            // ... создаём новую базу данных в файле и делаем её текущим проектом
            //
            m_projectsManager->setCurrentProject(newProjectFilePath);
            //
            // ... сохраняем путь
            //
            saveProjectsFolderPath(newProjectFilePath);
            //
            // ... перейдём к редактированию
            //
            goToEditCurrentProject(_importFilePath);
        }
        //
        // Если невозможно записать в файл, предупреждаем пользователя и отваливаемся
        //
        else {
            const QFileInfo fileInfo(newProjectFilePath);

            //
            // ... предупреждаем
            //
            QString errorMessage;
            if (!fileInfo.dir().exists()) {
                errorMessage =
                    tr("You try to create project in nonexistent folder <b>%1</b>. Please, choose other location for new project.")
                    .arg(fileInfo.dir().absolutePath());
            } else if (fileInfo.exists()) {
                errorMessage =
                    tr("Can't write to file. Maybe it is opened by another application. Please close it and retry.");
            } else {
                errorMessage =
                    tr("Can't write to file. Check permissions to write in choosed folder. Please, choose other folder.");
            }
            QLightBoxMessage::critical(m_view, tr("Create project error"), errorMessage);
            //
            // ... и перезапускаем создание проекта
            //
            QTimer::singleShot(0, this, &ApplicationManager::aboutCreateNew);
        }
    }
}

void ApplicationManager::createNewRemoteProject(const QString& _projectName, const QString& _importFilePath)
{
    //
    // Закроем текущий проект
    //
    closeCurrentProject();

    //
    // Создаём новый проект в облаке
    //
    const int newProjectId = m_synchronizationManager->createProject(_projectName);

    //
    // Проверяем удалось ли создать проект
    //
    if (newProjectId == Project::kInvalidId) {
        return;
    }

    //
    // Переключаемся на работу с новым проектом
    //
    const bool isRemote = false;
    if (m_projectsManager->setCurrentProject(newProjectId, isRemote)) {
        //
        // ... перейдём к редактированию
        //
        goToEditCurrentProject(_importFilePath);
    }
    //
    // Если переключиться не удалось, сообщаем пользователю об ошибке
    //
    else {
        QLightBoxMessage::critical(
            m_view,
            tr("Can't open project file"),
            DatabaseLayer::Database::openFileError());

        //
        // ... и перезапускаем создание проекта
        //
        QTimer::singleShot(0, this, &ApplicationManager::aboutCreateNew);
    }
}

void ApplicationManager::aboutSaveAs()
{
    //
    // Освобождаем очередь событий, чтобы диалог сохранения успел открыться до
    // следующей проверки соединения. В противном случае диалог закрывается на уровне ОС
    //
    QApplication::processEvents();

    //
    // Изначально высвечивается текущее имя проекта
    //
    QString projectPath = ProjectsManager::currentProject().path();
    const Project& currentProject = ProjectsManager::currentProject();
    if (currentProject.isRemote()) {
        //
        // Для удаленных проектов используем имя проекта + id проекта
        // и сохраняем в папку вновь создаваемых проектов
        //
        projectPath = projectsFolderPath() + QDir::separator()
                      + QString("%1 [%2]%3").arg(currentProject.name())
                                            .arg(currentProject.id())
                                            .arg(kProjectFleExtension);
    }

    //
    // Получим имя файла для сохранения
    //
    QString saveAsProjectFileName =
            QFileDialog::getSaveFileName(
                m_view,
                tr("Choose file to save project"),
                projectPath,
                tr ("Scenarist project files (*%1)").arg(kProjectFleExtension)
                );

    //
    // Если файл выбран
    //
    if (!saveAsProjectFileName.isEmpty()) {
        //
        // ... установим расширение, если не задано
        //
        if (!saveAsProjectFileName.endsWith(kProjectFleExtension)) {
            saveAsProjectFileName.append(kProjectFleExtension);
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
            if (QFile::copy(ProjectsManager::currentProject().path(), saveAsProjectFileName)) {
                //
                // ... отключаем индикатор соединения, если мы работали с облаком
                //
                if (m_projectsManager->currentProject().isRemote()) {
                    m_tabs->removeIndicator();
                }

                //
                // ... переключаемся на использование другого файла
                //
                DatabaseLayer::Database::setCurrentFile(saveAsProjectFileName);
                m_projectsManager->setCurrentProject(saveAsProjectFileName);

                //
                // ... сохраняем изменения
                //
                aboutSave();
                updateWindowModified(m_view, true);

                //
                // ... обновим заголовок
                //
                updateWindowTitle();
            } else {
                QLightBoxMessage::critical(m_view, tr("Saving error"),
                    tr("Can't save project as <b>%1</b>.<br/> Please check permissions and retry.")
                    .arg(saveAsProjectFileName));
            }
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
    // Избегаем рекурсии
    //
    const auto canRun = RunOnce::tryRun(Q_FUNC_INFO);
    if (!canRun) {
        return;
    }

    //
    // Сохраняем только, если приложение находится в рабочем состоянии
    //
    if (m_state != ApplicationState::Working) {
        return;
    }

    //
    // Если какие-то данные изменены
    //
    if (m_view->isWindowModified()) {
//        //
//        // Перед сохранением проверяем достаточно ли места на диске, если нет, то уведомляем пользователя
//        //
//        const QStorageInfo storageInfo(DatabaseLayer::Database::currentFile());
//        if (storageInfo.istorageInfo.bytesAvailable()/1000/1000 < 50) {
//            QLightBoxMessage::warning(
//                        m_view,
//                        tr("Possible save error"),
//                        tr("You have less than 50 megabytes of free disk space. This can lead to problems "
//                           "with saving the project. We recommend that you free up more space "
//                           "and check whether the project is saved correctly."));
//        }

        //
        // Управляющие должны сохранить несохранённые данные
        //
        DatabaseLayer::Database::transaction();
        m_researchManager->saveResearch();
        m_scenarioManager->saveCurrentProject();
        DatabaseLayer::Database::commit();

        //
        // Обновим информацию о последнем изменении
        //
        aboutUpdateLastChangeInfo();

        //
        // Если всё успешно сохранилось
        //
        if (!DatabaseLayer::Database::hasError()) {
            //
            // Изменим статус окна на сохранение изменений
            //
            updateWindowModified(m_view, false);

            //
            // Если необходимо создадим резервную копию закрываемого файла
            //
            QString baseBackupName;
            const Project& currentProject = ProjectsManager::currentProject();
            if (currentProject.isRemote()) {
                //
                // Для удаленных проектов имя бекапа - имя проекта + id проекта
                // В случае, если имя удаленного проекта изменилось, то бэкапы со старым именем останутся навсегда
                //
                baseBackupName = QString("%1 [%2]").arg(currentProject.name()).arg(currentProject.id());
            }
            QtConcurrent::run(&m_backupHelper, &BackupHelper::saveBackup, ProjectsManager::currentProject().path(), baseBackupName);
        }
        //
        // А если ошибка сохранения, то делаем дополнительные проверки и работаем с пользователем
        //
        else {
            //
            // Если файл, в который мы пробуем сохранять изменения существует
            //
            if (QFile::exists(DatabaseLayer::Database::currentFile())) {
                //
                // ... то у нас случилась какая-то внутренняя ошибка базы данных
                //
                const QDialogButtonBox::StandardButton messageResult =
                        QLightBoxMessage::critical(m_view, tr("Saving error"),
                                                   tr("Can't write your changes to the project. There is a internal database error: %1 "
                                                      "Please check, if this file exists and if you have permissions to write. Retry (to save)?")
                                                   .arg(DatabaseLayer::Database::lastError()),
                                                   QDialogButtonBox::Yes | QDialogButtonBox::No, QDialogButtonBox::Yes);
                //
                // ... пробуем повторно открыть базу данных и записать в неё изменения
                //
                if (messageResult == QDialogButtonBox::Yes) {
                    DatabaseLayer::Database::setCurrentFile(DatabaseLayer::Database::currentFile());
                    QTimer::singleShot(0, this, &ApplicationManager::aboutSave);
                }
            }
            //
            // Файла с базой данных не найдено
            //
            else {
                //
                // ... возможно файл был на флешке, а она отошла, или файл был переименован во время работы программы
                //
                const QDialogButtonBox::StandardButton messageResult =
                        QLightBoxMessage::critical(m_view, tr("Saving error"),
                            tr("Can't write your changes to project located at <b>%1</b>, because the file doesn't exist. "
                               "Please move the file back and retry saving. Retry saving")
                                .arg(DatabaseLayer::Database::currentFile()),
                            QDialogButtonBox::Yes | QDialogButtonBox::No, QDialogButtonBox::Yes);
                //
                // ... пробуем повторно сохранить изменения в базу данных
                //
                if (messageResult == QDialogButtonBox::Yes) {
                    aboutSave();
                }
            }
        }
    }

    //
    // Для проекта из облака синхронизируем данные
    //
    if (m_projectsManager->currentProject().isRemote()) {
        m_synchronizationManager->aboutWorkSyncScenario();
        m_synchronizationManager->aboutWorkSyncData();
    }
}

void ApplicationManager::aboutStartNewVersion()
{
    UserInterface::ProjectVersionDialog versionDialog(m_view);
    versionDialog.setPreviousVersions(DataStorageLayer::StorageFacade::scriptVersionStorage()->all());
    if (versionDialog.exec() == QLightBoxDialog::Accepted) {
        //
        // Сперва сохраним текст сценария
        //
        m_scenarioManager->saveCurrentProject();
        //
        // А уж потом положим версию в базу данных
        //
        DataStorageLayer::StorageFacade::scriptVersionStorage()->storeScriptVersion(
            DataStorageLayer::StorageFacade::userName(), versionDialog.versionDateTime(), versionDialog.versionColor(),
            versionDialog.versionName(), versionDialog.versionDescription(), m_scenarioManager->scenario()->save());
        //
        // И обнови заголовок окна, чтобы отразить в нём новую версию
        //
        updateWindowTitle();
    }
}

void ApplicationManager::saveCurrentProjectSettings(const QString& _projectPath)
{
    //
    // Сохраним используемые модули
    //

    int tabIndexToSave = m_tabs->currentTab();
    if (tabIndexToSave == STARTUP_TAB_INDEX) {
        tabIndexToSave = m_tabs->prevCurrentTab();
    }
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
        QString("projects/%1/last-active-module_left").arg(_projectPath),
        QString::number(tabIndexToSave),
        DataStorageLayer::SettingsStorage::ApplicationSettings
        );

    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
        QString("projects/%1/last-active-module_right").arg(_projectPath),
        QString::number(m_tabsSecondary->currentTab()),
        DataStorageLayer::SettingsStorage::ApplicationSettings
        );
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
                        tr ("Scenarist project files (*%1)").arg(kProjectFleExtension)
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
            bool canOpenProject = m_projectsManager->setCurrentProject(loadProjectFileName);
            if (!canOpenProject) {
                //
                // ... если переключиться не удалось, сообщаем пользователю об ошибке,
                //     но даём возможность игнорировать её и всё равно попробовать открыть файл
                //
                if (QLightBoxMessage::critical(m_view, tr("Can't open project file"),
                        DatabaseLayer::Database::openFileError() + "\n\n" + tr("Ignore and try to open project?"),
                        QDialogButtonBox::No | QDialogButtonBox::Open)
                    == QDialogButtonBox::Open) {
                    const bool isLocal = true;
                    const bool forced = true;
                    canOpenProject = m_projectsManager->setCurrentProject(loadProjectFileName, isLocal, forced);
                }
            }

            if (canOpenProject) {
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

        //
        // Изменим статус окна на сохранение изменений
        //
        updateWindowModified(m_view, false);
    }
}

void ApplicationManager::loadCurrentProjectSettings(const QString& _projectPath)
{
    //
    // Восстановим используемые модули
    //

    int lastModuleLeft =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                QString("projects/%1/last-active-module_left").arg(_projectPath),
                DataStorageLayer::SettingsStorage::ApplicationSettings,
                QString::number(RESEARCH_TAB_INDEX)
                ).toInt();
    //
    // ... если последней используемой была стартовая страница или настройки,
    //     покажем разработку
    //
    if (lastModuleLeft == STARTUP_TAB_INDEX
        || lastModuleLeft == SETTINGS_TAB_INDEX) {
        lastModuleLeft = RESEARCH_TAB_INDEX;
    }
    m_tabs->setCurrentTab(lastModuleLeft);

    const int lastModuleRight =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                QString("projects/%1/last-active-module_right").arg(_projectPath),
                DataStorageLayer::SettingsStorage::ApplicationSettings,
                QString::number(SCENARIO_TAB_INDEX)
                ).toInt();
    m_tabsSecondary->setCurrentTab(lastModuleRight);
}

void ApplicationManager::aboutShowHelp()
{
    const QString url = QString("https://kitscenarist.ru/%1help/").arg(urlLanguage());
    QDesktopServices::openUrl(QUrl(url));
}

void ApplicationManager::aboutShowCrowdfinding()
{
    QDesktopServices::openUrl(QUrl("https://boomstarter.ru/projects/dimkanovikov/kit_stsenarist_-_programma_dlya_sozdaniya_istoriy"));
}

void ApplicationManager::aboutLoadFromRecent(const QModelIndex& _projectIndex)
{
    //
    // Если индекс поломан, ничего не делаем
    // Так может случиться, если в момент открытия проекта список проектов перезагрузился
    //
    if (!_projectIndex.isValid()) {
        return;
    }

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
        bool canOpenProject = m_projectsManager->setCurrentProject(_projectIndex);
        if (!canOpenProject) {
            //
            // ... если переключиться не удалось, сообщаем пользователю об ошибке,
            //     но даём возможность игнорировать её и всё равно попробовать открыть файл
            //
            if (QLightBoxMessage::critical(m_view, tr("Can't open project file"),
                    DatabaseLayer::Database::openFileError() + "\n\n" + tr("Ignore and try to open project?"),
                    QDialogButtonBox::No | QDialogButtonBox::Open)
                == QDialogButtonBox::Open) {
                const bool isLocal = true;
                const bool forced = true;
                canOpenProject = m_projectsManager->setCurrentProject(_projectIndex, isLocal, forced);
            }
        }

        if (canOpenProject) {
            //
            // ... если файл доступен только для чтения, уведомим об этом пользователя
            //
            if (!ProjectsManager::currentProject().isWritable()) {
                QLightBoxMessage::information(m_view, tr("A file will be opened in read-only mode"),
                    tr("If you want to edit a file, please check it's permissions for your account."));
            }

            //
            // ... перейдём к редактированию
            //
            goToEditCurrentProject();
        }
    }
}

void ApplicationManager::hideLocalProject(const QModelIndex& _index)
{
    const QString question = tr("Are you sure to hide project <b>%1</b> from recent?")
                             .arg(m_projectsManager->project(_index).name());
    if (QLightBoxMessage::question(m_view, QString::null, question)
        == QDialogButtonBox::Yes) {
        m_projectsManager->hideProjectFromLocal(_index);
    }
}

void ApplicationManager::moveLocalProjectToCloud(const QModelIndex& _index)
{
    if (!_index.isValid()) {
        return;
    }

    if (!saveIfNeeded()) {
        return;
    }

    if (!m_synchronizationManager->isLogged()) {
        QLightBoxMessage::warning(m_view, tr("Moving project to the cloud failed"),
            tr("For moving projects to the cloud you should be logged in the KIT Scenarist cloud service."));
        return;
    }

    if (!m_synchronizationManager->isSubscriptionActive()) {
        QLightBoxMessage::warning(m_view, tr("Moving project to the cloud failed"),
            tr("For moving projects to the cloud your subscription in the KIT Scenarist cloud service should be active."));
        return;
    }

    //
    // Получим путь до файла и название проекта
    //
    const Project project = m_projectsManager->project(_index);
    createNewRemoteProject(project.name(), project.path());
}

void ApplicationManager::aboutLoadFromRemote(const QModelIndex& _projectIndex)
{
    //
    // Если индекс поломан, ничего не делаем
    // Так может случиться, если в момент открытия проекта список проектов перезагрузился
    //
    if (!_projectIndex.isValid()) {
        return;
    }

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
        if (m_projectsManager->setCurrentProject(_projectIndex, isRemote)) {
            //
            // ... перейдём к редактированию
            //
            goToEditCurrentProject();
        }
        //
        // Если переключиться не удалось, сообщаем пользователю об ошибке
        //
        else {
            QLightBoxMessage::critical(
                m_view,
                tr("Can't open project file"),
                DatabaseLayer::Database::openFileError());
        }
    }
}

void ApplicationManager::editRemoteProjectName(const QModelIndex& _index)
{
    const bool IS_REMOTE = false;
    Project& project = m_projectsManager->project(_index, IS_REMOTE);
    const QString newName =
            QLightBoxInputDialog::getText(m_view, tr("Change project name"),
                tr("Enter new name for project"), project.name());
    //
    // Если пользователь действительно хочет переименовать проект
    //
    if (!newName.isEmpty()) {
        //
        // ... если проект, который нужно переименовать сейчас открыт, то закроем его
        //
        if (project == m_projectsManager->currentProject()) {
            closeCurrentProject();
        }
        //
        // ... переименуем
        //     это действие приводит к перезагрузке списка проектов, так что более делать ничего не надо
        //
        m_synchronizationManager->updateProjectName(project.id(), newName);
        //
        // ... и обновим список проектов
        //
        m_startUpManager->setRemoteProjects(m_projectsManager->remoteProjects());
    }
}

void ApplicationManager::removeRemoteProject(const QModelIndex& _index)
{
    const bool IS_REMOTE = false;
    const Project project = m_projectsManager->project(_index, IS_REMOTE);

    //
    // Если пользователь является владельцем файла, то он может его удалить
    //
    if (project.isUserOwner()) {
        if (QLightBoxMessage::question(m_view, QString::null, tr("Are you sure to remove project <b>%1</b>?").arg(project.name()))
            == QDialogButtonBox::Yes) {
            //
            // Если в данный момент открыт проект, который пользователь хочет удалить, закрываем его
            //
            if (project == m_projectsManager->currentProject()) {
                closeCurrentProject();
            }
            m_synchronizationManager->removeProject(project.id());
        }
    }
    //
    // А если нет, то только отписаться от него
    //
    else {
        if (QLightBoxMessage::question(m_view, QString::null, tr("Are you sure to remove your subscription to project <b>%1</b>?").arg(project.name()))
            == QDialogButtonBox::Yes) {
            //
            // Если в данный момент открыт проект, от которого пользователь хочет отписаться, закрываем его
            //
            if (project == m_projectsManager->currentProject()) {
                closeCurrentProject();
            }
            m_synchronizationManager->unshareProject(project.id());
        }
    }
}

void ApplicationManager::shareRemoteProject(const QModelIndex& _index)
{
    const bool IS_REMOTE = false;
    const Project project = m_projectsManager->project(_index, IS_REMOTE);
    ShareDialog dlg(m_view);
    if (dlg.exec() == ShareDialog::Accepted) {
        m_synchronizationManager->shareProject(project.id(), dlg.email(), dlg.role());
    }
}

void ApplicationManager::unshareRemoteProject(const QModelIndex& _index, const QString& _userEmail)
{
    const bool IS_REMOTE = false;
    const Project project = m_projectsManager->project(_index, IS_REMOTE);
    if (QLightBoxMessage::question(m_view, QString::null, tr("Are you sure to remove subscription of user <b>%1</b> to project <b>%2</b>?")
                                   .arg(_userEmail)
                                   .arg(project.name()))
        == QDialogButtonBox::Yes) {
        m_synchronizationManager->unshareProject(project.id(), _userEmail);
    }
}

void ApplicationManager::setSyncIndicator()
{
    const bool isActiveInternet = m_synchronizationManager->isInternetConnectionActive();
    const bool isRemoteProject = m_projectsManager->isCurrentProjectValid() &&
            m_projectsManager->currentProject().isRemote();

    QString iconPath;
    QString indicatorTitle;
    QString indicatorText;
    QColor waveColor;
    if (isActiveInternet){
        //
        // Если интернет есть, надо понять, с каким типом проекта работает пользователь
        //
        indicatorTitle = tr("Connection active");
        if (isRemoteProject) {
            //
            // Облачный проект
            //
            iconPath = ":/Graphics/Iconset/Indicator/synced.png";
            indicatorText = tr("Project synchronized");
        } else {
            //
            // Локальный проект
            //
            iconPath = ":/Graphics/Iconset/Indicator/connected.png";
        }
        waveColor = QColor(0, 255, 0, 40);
    } else {
        //
        // Если интернета нет, то всегда показываем значок отключенного интернета
        //
        iconPath = ":/Graphics/Iconset/Indicator/disconnected.png";
        indicatorTitle = tr("Connection inactive");
        indicatorText = isRemoteProject ? tr("Project didn't synchronized") : QString::null;
        waveColor = QColor(255, 0, 0, 40);
    }
    m_tabs->addIndicator(QIcon(iconPath));
    m_tabs->setIndicatorTitle(indicatorTitle);
    m_tabs->setIndicatorText(indicatorText);
    m_tabs->setIndicatorActionIcon(QIcon());
    m_tabs->makeIndicatorWave(waveColor);
}

void ApplicationManager::aboutUpdateLastChangeInfo()
{
    //
    // Берём последнее изменение базы данных
    //
    const auto databaseChange = DataStorageLayer::StorageFacade::databaseHistoryStorage()->last();
    QDateTime changeDatetime = QDateTime::fromString(databaseChange.value("datetime"), "yyyy-MM-dd hh:mm:ss");
    QString changeUserName = databaseChange.value("username");

    //
    // Берём последнее изменение сценария
    //
    const auto* scenarioChange = DataStorageLayer::StorageFacade::scenarioChangeStorage()->last();
    if (scenarioChange != nullptr) {
        //
        // Ищем наиболее позднее изменение
        //
        if (changeDatetime < scenarioChange->datetime()) {
            changeDatetime = scenarioChange->datetime();
            changeUserName = scenarioChange->user();
        }
    }

    //
    // Время хранится в UTC поэтому убираем отступ от него
    //
    changeDatetime.setUtcOffset(0);

    //
    // Индицируем последнее изменение
    //
    QString lastChange =
        QString("%1: %2 %3")
        .arg(tr("Modified"))
        .arg(changeDatetime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss"))
        .arg(changeUserName);
    m_tabs->setIndicatorFooterText(lastChange);
}

void ApplicationManager::aboutSyncClosedWithError(int _errorCode, const QString& _error)
{
    QString title;
    QString error = _error;
    QIcon reactivateIcon;
    bool disableSyncForCurrentProject = false;
    bool isCriticalError = true;
    switch (_errorCode) {
        //
        // Нет связи с интернетом
        //
        case Sync::NetworkError: {
            //
            // Если ошибка пришла от окна акторизации или смены пароля, покажем её в нём
            //
            if (m_menuManager->isOnLoginDialog()) {
                m_menuManager->retryLastAction(_error);
            }
            //
            // А если ошибка пришла в момент работы с облаком, то покажем её в индикаторе
            //
            else {
                title = tr("Network error");
                error += "\n\n";
                error += tr("Project didn't synchronized.");

                //
                // Нет интернета в момент автологина. Текст о соединении
                //
                m_menuManager->setProgressLoginLabel(false);
            }
            break;
        }

        //
        // Проблемы с вводом логина и пароля
        //
        case Sync::IncorrectLoginError:
        case Sync::IncorrectPasswordError: {
            //
            // Если пользователь не был авторизован, то прокинем
            // сообщение об ошибке в диалог авторизации
            //
            if (!m_synchronizationManager->isLogged()) {
                error = tr("Incorrect username or password.");
                m_menuManager->setProgressLoginLabel(false);
                m_menuManager->retryLogin(error);
            }
            //
            // В противном случае, разавторизуем и покажем окно авторизации
            // с сообщением о том, что пароль изменился
            //
            else {
                const QString email = m_menuManager->userEmail();
                m_synchronizationManager->logout();
                error = tr("Saved password is incorrect. Looks like you changed the password. Please, enter the new password.");
                m_menuManager->showLoginDialog(email, error);
            }
            break;
        }

        //
        // Закончилась подписка
        //
        case Sync::SubscriptionEndedError: {
            title = tr("Subscription ended");
            error = tr("Buyed subscription period is finished.\n\n"
                       "Project didn't synchronized.");
            disableSyncForCurrentProject = true;
            break;
        }

        //
        // Не задан ключ сессии
        // NOTE: Такая проблема может возникать при проблемах с провайдером,
        //		 когда данные портятся на каком-либо из узлов связи
        //
        case Sync::NoSessionKeyError: {
            title = tr("Network Error");
            error = tr("Can't load all data from service correctly. "
                       "Please check your internet connection and refresh synchronization.\n\n"
                       "Project didn't synchronized.");
            break;
        }

        //
        // Сессия закрыта
        //
        case Sync::SessionClosedError: {
            //
            // Переходим в автономный режим с возможностью переавторизации
            //
            title = tr("Session closed");
            error = tr("New session for you account started at other device.\n\n"
                       "Project didn't synchronized.");
            reactivateIcon = QIcon(":/Graphics/Iconset/refresh.svg");
            break;
        }

        //
        // Пользователь пытается открыть доступ к проекту самому себе
        //
        case Sync::DisallowToShareSelf: {
            title = tr("Share error");
            error = tr("You can't share project with yourself.");
            isCriticalError = false;
            break;
        }

        //
        // Проект недоступен
        //
        case Sync::ProjectUnavailableError: {
            title = tr("Project not available");
            error = tr("Current project is not available for syncronization now, because project's owner subscription is ended.\n\n"
                       "Project didn't synchronized.");
            disableSyncForCurrentProject = true;
            break;
        }

        //
        // Доступ к проекту закрыт
        //
        case Sync::AccessToProjectClosed: {
            title = tr("Project not available");
            error = tr("Current project is not available for syncronization now, because project's owner closed access to project for you.\n\n"
                       "Project didn't synchronized.");
            disableSyncForCurrentProject = true;
            break;
        }

        //
        // Закончилось доступное меcто в облаке
        //
        case Sync::StorageSizeFinished: {
            title = tr("Sync not available");
            error = tr("You have exhausted all available for the use of space on a server.\n\n"
                       "Project didn't synchronized.");
            disableSyncForCurrentProject = true;
            break;
        }

        //
        // Используется устаревшая версия программы
        //
        case Sync::AppVersionOutdated: {
            title = tr("Project sync not available");
            error = tr("You have outdated version of the application. Please install latest app version for restoring access to the project.\n\n"
                       "Project didn't synchronized.");
            disableSyncForCurrentProject = true;
            break;
        }

        //
        // Такой email уже зарегистрирован
        //
        case Sync::EmailAlreadyRegisteredError: {
            error = tr("Email already exist");
            m_menuManager->retrySignUp(error);
            break;
        }

        //
        // Слишком слабый пароль
        //
        case Sync::WeakPasswordError: {
            error = tr("Password too weak");
            m_menuManager->showPasswordError(error);
            break;
        }

        //
        // Старый пароль некорректен
        //
        case Sync::IncorrectOldPasswordError: {
            error = tr("Invalid old password");
            m_menuManager->showPasswordError(error);
            break;
        }

        //
        // Неверный код валидации
        //
        case Sync::IncorrectValidationCodeError: {
            error = tr("Wrong validation code");
            m_menuManager->retryVerify(error);
            break;
        }

        //
        // Неверный email для восстановления пароля
        //
        case Sync::EmailNotRegisteredError: {
            error = tr("Wrong email");
            m_menuManager->retryLogin(error);
            break;
        }

        //
        // Остальное
        //
        default: {
            title = tr("Unknown Error");
            break;
        }
    }

    //
    // Для критичных ошибок
    //
    if (isCriticalError) {
        //
        // Сигнализируем об ошибке
        // Если не залогинены, то значок не показываем
        // Если пропал интернет, то значок сам покажется при необходимости
        //
        if (m_synchronizationManager->isInternetConnectionActive()
                && m_synchronizationManager->isLogged()) {
            m_tabs->addIndicator(QIcon(":/Graphics/Iconset/Indicator/unsynced.png"));
            m_tabs->setIndicatorTitle(title);
            m_tabs->setIndicatorText(error);
            m_tabs->setIndicatorActionIcon(reactivateIcon);
            m_tabs->makeIndicatorWave(QColor(255, 0, 0, 40));
        }

        //
        // Если необходимо отключаем синхронизацию для текущего проекта
        //
        if (disableSyncForCurrentProject) {
            m_projectsManager->setCurrentProjectSyncAvailable(SYNC_UNAVAILABLE);
        }
    }
    //
    // Для некритичных ошибок просто покажем сообщение с ошибкой
    //
    else {
        QLightBoxMessage::warning(m_view, title, error);
    }
}

void ApplicationManager::aboutImport()
{
    m_state = ApplicationState::Importing;
    m_importManager->importScenario(m_scenarioManager->scenario(), m_scenarioManager->cursorPosition());
    m_researchManager->loadScenarioData();

    m_state = ApplicationState::Working;
}

void ApplicationManager::aboutExport()
{
    m_exportManager->exportScenario(m_scenarioManager->scenario(), m_researchManager->scenarioData());
}

void ApplicationManager::printPreviewScript()
{
    m_exportManager->printPreview(m_scenarioManager->scenario(), m_researchManager->scenarioData(),
                                  ManagementLayer::ExportType::Script);
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
        QLightBoxProgress progress(m_view);
        progress.showProgress(tr("Exit from Application"), tr("Closing Database Connections and remove temporary files."));

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
    if (isProjectLoaded()) {
        updateWindowModified(m_view, true);
        m_statisticsManager->scenarioTextChanged();
    }
}

void ApplicationManager::aboutShowFullscreen()
{
    const char* IS_MAXIMIZED_PROPERTY = "isMaximized";

    const bool useZenMode =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "scenario-editor/hide-panels-in-fullscreen",
                DataStorageLayer::SettingsStorage::ApplicationSettings)
            .toInt();

    if (m_view->isFullScreen()) {
        //
        // Возвращаемся в состояние предшествовавшее полноэкранному режиму
        //
        m_menu->show();
        m_tabs->show();
        //
        if (useZenMode) {
            m_scenarioManager->setZenMode(false);
        }
        //
        if (m_view->property(IS_MAXIMIZED_PROPERTY).toBool()) {
            m_view->showMaximized();
        } else {
            m_view->showNormal();
        }
    } else {
        //
        // Сохраним состояние окна перед переходом в полноэкранный режим
        //
        m_view->setProperty(IS_MAXIMIZED_PROPERTY, m_view->windowState().testFlag(Qt::WindowMaximized));

        //
        // Переходим в полноэкранный режим
        //
        m_menu->hide();
        m_tabs->hide();
        //
        if (useZenMode) {
            m_scenarioManager->setZenMode(true);
        }
        //
        m_view->showFullScreen();
    }
}

void ApplicationManager::aboutPrepareScenarioForStatistics()
{
    m_statisticsManager->setExportedScenario(m_scenarioManager->scenario()->document());
}

void ApplicationManager::aboutInnerLinkActivated(const QUrl& _url)
{
    if (_url.scheme() == "inapp") {
        if (_url.host() == "scenario") {
            const QStringList parameters = _url.query().split("&");
            const int INVALID_CURSOR_POSITION = -1;
            int cursorPosition = INVALID_CURSOR_POSITION;
            int fromTabIndex = RESEARCH_TAB_INDEX;
            foreach (const QString parameter, parameters) {
                const QStringList paramaterDetails = parameter.split("=");
                if (paramaterDetails.first() == "position") {
                    cursorPosition = paramaterDetails.last().toInt();
                }
                if (paramaterDetails.first() == "from") {
                    const QString from = paramaterDetails.last();
                    if (from == "research") {
                        fromTabIndex = RESEARCH_TAB_INDEX;
                    } else if (from == "cards") {
                        fromTabIndex = SCENARIO_CARDS_TAB_INDEX;
                    } else if (from == "statistics") {
                        fromTabIndex = STATISTICS_TAB_INDEX;
                    }
                }
            }

            if (cursorPosition != INVALID_CURSOR_POSITION) {
                if (m_tabsSecondary->isVisible()) {
                    if (m_tabs->currentTab() == fromTabIndex) {
                        m_tabsSecondary->setCurrentTab(SCENARIO_TAB_INDEX);
                    } else {
                        m_tabs->setCurrentTab(SCENARIO_TAB_INDEX);
                    }
                } else {
                    m_tabs->setCurrentTab(SCENARIO_TAB_INDEX);
                }
                //
                // Выполняем события, чтобы пропустить первую прокрутку текста, после запуска
                // приложения к последнему рабочему месту в сценарии
                //
                m_scenarioManager->setCursorPosition(cursorPosition);
            }
        }
    }
}

bool ApplicationManager::event(QEvent* _event)
{
    bool result = false;
    if (_event->type() == QEvent::User) {
        if (m_autosaveTimer.isActive()) {
            aboutSave();
        }
        _event->accept();
        result = true;
    } else {
        result = QObject::event(_event);
    }

    return result;
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

    //
    // Для всех полос прокрутки добавляем функциональность - при наведении они расширяются
    //
    m_view->initScrollBarsWidthChanges();

    //
    // Загружаем состояние контроллеров
    //
    m_scenarioManager->loadViewState();
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
                if (m_tabsSecondary->isVisible()
                    && m_tabsSecondary->currentTab() == m_tabs->currentTab()) {
                    m_tabs->setCurrentTab(m_tabsSecondary->prevCurrentTab());
                }
            }

            //
            // Функция для определения виджета для отображения по индексу вкладки
            //

            auto widgetForTab =
                [=] (int _index) {
                QWidget* result = 0;
                switch (_index) {
                    case STARTUP_TAB_INDEX: result = m_startUpManager->view(); break;
                    case RESEARCH_TAB_INDEX: result = m_researchManager->view(); break;
                    case SCENARIO_CARDS_TAB_INDEX: result = m_scenarioManager->cardsView(); break;
                    case SCENARIO_TAB_INDEX: result = m_scenarioManager->view(); break;
                    case STATISTICS_TAB_INDEX: result = m_statisticsManager->view(); break;
                    case TOOLS_TAB_INDEX: result = m_toolsManager->view(); break;
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

#ifdef Q_OS_MAC
                m_editMenu->clear();
                switch (m_tabs->currentTab()) {
                    case SCENARIO_TAB_INDEX: {
                        m_scenarioManager->buildScriptEditMenu(m_editMenu);
                        break;
                    }

                    default: {
                        m_startUpManager->buildEditMenu(m_editMenu);
                        break;
                    }
                }
#endif
            }
            //
            if (m_tabsSecondary->isVisible()) {
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
    if (!m_view->isWindowModified()) {
        return true;
    }

    //
    // Если какие-то данные изменены
    //

    bool success = true;

    int questionResult = QDialogButtonBox::Cancel;

    //
    // ... если работаем с проектом из облака, сохраняем без вопросов
    //
    if (m_projectsManager->currentProject().isRemote()) {
        questionResult = QDialogButtonBox::Yes;
    }
    //
    // ... для локальных проектов спрашиваем пользователя, хочет ли он сохранить изменения
    //
    else {
        questionResult =
                QLightBoxMessage::question(m_view, tr("Save project changes?"),
                    tr("Project was modified. Save changes?"),
                    QDialogButtonBox::Cancel | QDialogButtonBox::Yes | QDialogButtonBox::No);
    }

    if (questionResult != QDialogButtonBox::Cancel) {
        //
        // ... и сохраняем, если хочет
        //
        if (questionResult == QDialogButtonBox::Yes) {
            aboutSave();
        } else {
            updateWindowModified(m_view, false);
        }
    } else {
        success = false;
    }

    return success;
}

void ApplicationManager::goToEditCurrentProject(const QString& _importFilePath)
{
    m_state = ApplicationState::ProjectLoading;

    //
    // Покажем уведомление пользователю
    //
    QLightBoxProgress progress(m_view);
    progress.showProgress(tr("Loading Scenario"), tr("Please wait. Loading can take few minutes."));

    //
    // Установим заголовок
    //
    updateWindowTitle();

    //
    // Активируем вкладки
    //
    ::enableActionsOnProjectOpen();
    m_menuManager->enableProjectActions();

    //
    // Настроим режим работы со сценарием
    //
    const bool isCommentOnly = ProjectsManager::currentProject().isCommentOnly();
    m_menuManager->setMenuItemEnabled(kStartNewVersionMenuIndex, !isCommentOnly);
    m_menuManager->setMenuItemEnabled(kImportMenuIndex, !isCommentOnly);
    m_menuManager->setMenuItemEnabled(kExportMenuIndex, !isCommentOnly);
    m_researchManager->setCommentOnly(isCommentOnly);
    m_scenarioManager->setCommentOnly(isCommentOnly);

    //
    // Если открываемый файл доступен только для чтения, то блокируем изменения, но оставляем возможность экспорта
    //
    if (!ProjectsManager::currentProject().isWritable()) {
        m_menuManager->setMenuItemEnabled(kStartNewVersionMenuIndex, false);
        m_menuManager->setMenuItemEnabled(kImportMenuIndex, false);
        m_researchManager->setCommentOnly(true);
        m_scenarioManager->setCommentOnly(true);
    }

    //
    // Настроим индикатор
    //
    if (m_projectsManager->currentProject().isRemote()) {
        //
        // Сбросим состояние возможности синхронизации проекта,
        // т.к. она могла измениться, например была продлена подписка
        //
        m_projectsManager->setCurrentProjectSyncAvailable(SYNC_AVAILABLE);

        setSyncIndicator();
        m_synchronizationManager->prepareToFullSynchronization();
    }

    //
    // FIXME: Сделать загрузку сценария  сразу в БД, это заодно позволит избавиться
    //		  и от необходимости сохранять проект после синхронизации
    //
    // Загружаем текст сценария
    // Это нужно делать перед синхронизацией текста
    //
    m_scenarioManager->loadCurrentProject();

    //
    // Синхронизируем проекты из облака
    //
    if (m_projectsManager->currentProject().isRemote()) {
        progress.setProgressText(QString::null, tr("Sync scenario with cloud service."));
        m_synchronizationManager->aboutFullSyncScenario();
        m_synchronizationManager->aboutFullSyncData();
    }

    //
    // FIXME: Если были изменения связанные с текстом сценария перестраиваем карточки
    //        т.к. там нет пока синхронизации
    //
    m_scenarioManager->rebuildCardsFromScript();

    //
    // Загрузить данные из файла
    // Делать это нужно после того, как все данные синхронизировались
    //
    m_researchManager->loadCurrentProject();
    m_statisticsManager->loadCurrentProject();

    //
    // Затем импортируем данные из указанного файла, если необходимо
    //
    if (!_importFilePath.isEmpty()) {
        progress.setProgressText(tr("Import"), tr("Please wait. Import can take few minutes."));
        m_importManager->importScenario(m_scenarioManager->scenario(), _importFilePath);
        m_researchManager->loadScenarioData();
    }

    //
    // Запускаем обработку изменений сценария
    //
    m_scenarioManager->startChangesHandling();

    //
    // Загрузить настройки файла
    // Порядок загрузки важен - сначала настройки каждого модуля, потом активные вкладки
    //
    m_researchManager->loadCurrentProjectSettings(ProjectsManager::currentProject().path());
    m_scenarioManager->loadCurrentProjectSettings(ProjectsManager::currentProject().path());
    m_exportManager->loadCurrentProjectSettings(ProjectsManager::currentProject().path());
    m_toolsManager->loadCurrentProjectSettings();
    loadCurrentProjectSettings(ProjectsManager::currentProject().path());

    //
    // Обновим название текущего проекта, т.к. данные о проекте теперь загружены
    //
    updateWindowTitle();

    //
    // Установим параметры между менеджерами
    //
    m_scenarioManager->setScriptHeader(m_researchManager->scriptHeader());
    m_scenarioManager->setScriptFooter(m_researchManager->scriptFooter());
    m_scenarioManager->setSceneNumbersPrefix(m_researchManager->sceneNumbersPrefix());
    m_scenarioManager->setSceneStartNumber(m_researchManager->sceneStartNumber());

    //
    // Обновим информацию о последнем изменении
    //
    aboutUpdateLastChangeInfo();

    //
    // Закроем уведомление
    //
    QApplication::sendPostedEvents();
    QApplication::processEvents();
    progress.finish();

    m_state = ApplicationState::Working;

    //
    // После того, как все данные загружены и синхронизированы, сохраняем проект
    //
    if (m_projectsManager->currentProject().isRemote()) {
        updateWindowModified(m_view, true);
        aboutSave();
    }
}

void ApplicationManager::closeCurrentProject()
{
    if (isProjectLoaded()) {
        //
        // Сохраним настройки закрываемого проекта
        //
        m_researchManager->saveCurrentProjectSettings(ProjectsManager::currentProject().path());
        m_scenarioManager->saveCurrentProjectSettings(ProjectsManager::currentProject().path());
        m_exportManager->saveCurrentProjectSettings(ProjectsManager::currentProject().path());
        saveCurrentProjectSettings(ProjectsManager::currentProject().path());

        //
        // Закроем проект управляющими
        //
        m_researchManager->closeCurrentProject();
        m_scenarioManager->closeCurrentProject();

        //
        // Удалим всю информацию о курсорах соавтора
        //
        m_tabs->clearIndicatorMenu();
        m_scenarioManager->clearAdditionalCursors();

        //
        // Информируем управляющего проектами, что текущий проект закрыт
        //
        m_projectsManager->closeCurrentProject();

        //
        // Отключим некоторые действия, которые не могут быть выполнены до момента загрузки проекта
        //
        ::disableActionsOnStart();
        m_menuManager->disableProjectActions();

        //
        // Перейти на стартовую вкладку
        //
        m_tabs->setCurrentTab(STARTUP_TAB_INDEX);
        m_tabsSecondary->setCurrentTab(SETTINGS_TAB_INDEX);
    }

    //
    // Очистим все загруженные на текущий момент данные
    //
    DataStorageLayer::StorageFacade::clearStorages();

    //
    // Если использовалась база данных, то удалим старое соединение
    //
    DatabaseLayer::Database::closeCurrentFile();

    updateWindowModified(m_view, false);
    updateWindowTitle();
}

bool ApplicationManager::isProjectLoaded() const
{
    return m_projectsManager->isCurrentProjectValid();
}

void ApplicationManager::initControllers()
{
    m_exportManager->setResearchModel(m_researchManager->model());
}

void ApplicationManager::initView()
{
    //
    // Настроим меню
    //
    m_menu->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_menu->setIcons(QIcon(":/Graphics/Iconset/menu.svg"));
    m_menu->setText(tr("Menu"));
    m_menuManager->setMenu(createMenu());
    m_menuSecondary->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

#ifdef Q_OS_MAC
    //
    // Добавляем в маке возможность открытия ещё одного окна приложения
    //
    QMenu* menu = new QMenu(m_view);
    QAction* openNewWindow = menu->addAction(tr("New window"));
    connect(openNewWindow, &QAction::triggered, [=] {
        QString appPath = QApplication::applicationFilePath();
        appPath = appPath.split(".app").first();
        appPath += ".app";
        QProcess::startDetached("open", {"-na", appPath});
    });
    menu->setAsDockMenu();
#endif

    //
    // Настроим боковую панель
    //
    // ... основную
    //
    m_tabs->addTab(tr("Start"), QIcon(":/Graphics/Iconset/apps.svg"));
    g_disableOnStartActions << m_tabs->addTab(tr("Research"), QIcon(":/Graphics/Iconset/sitemap.svg"));
    g_disableOnStartActions << m_tabs->addTab(tr("Cards"), QIcon(":/Graphics/Iconset/arrange-bring-to-front.svg"));
    g_disableOnStartActions << m_tabs->addTab(tr("Scenario"), QIcon(":/Graphics/Iconset/file-document-box.svg"));
    g_disableOnStartActions << m_tabs->addTab(tr("Statistics"), QIcon(":/Graphics/Iconset/chart-areaspline.svg"));
    g_disableOnStartActions << m_tabs->addTab(tr("Tools"), QIcon(":/Graphics/Iconset/wrench.svg"));
    m_tabs->addTab(tr("Settings"), QIcon(":/Graphics/Iconset/settings.svg"));
    //
    // ... вспомогательную
    //
    m_tabsSecondary->setCompactMode(true);
    m_tabsSecondary->addTab(tr("Start"), QIcon(":/Graphics/Iconset/apps.svg"));
    g_disableOnStartActions << m_tabsSecondary->addTab(tr("Research"), QIcon(":/Graphics/Iconset/sitemap.svg"));
    g_disableOnStartActions << m_tabsSecondary->addTab(tr("Cards"), QIcon(":/Graphics/Iconset/arrange-bring-to-front.svg"));
    g_disableOnStartActions << m_tabsSecondary->addTab(tr("Scenario"), QIcon(":/Graphics/Iconset/file-document-box.svg"));
    g_disableOnStartActions << m_tabsSecondary->addTab(tr("Statistics"), QIcon(":/Graphics/Iconset/chart-areaspline.svg"));
    g_disableOnStartActions << m_tabsSecondary->addTab(tr("Tools"), QIcon(":/Graphics/Iconset/wrench.svg"));
    m_tabsSecondary->addTab(tr("Settings"), QIcon(":/Graphics/Iconset/settings.svg"));
    m_tabsSecondary->setCurrentTab(SETTINGS_TAB_INDEX);

    //
    // Настроим виджеты соответствующие первоначальным активным вкладкам
    //
    m_tabsWidgets->setObjectName("tabsWidgets");
    m_tabsWidgets->addWidget(m_startUpManager->view());
    m_tabsWidgets->addWidget(m_researchManager->view());
    m_tabsWidgets->addWidget(m_scenarioManager->cardsView());
    m_tabsWidgets->addWidget(m_scenarioManager->view());
    m_tabsWidgets->addWidget(m_statisticsManager->view());
    m_tabsWidgets->addWidget(m_toolsManager->view());
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
    m_menuManager->disableProjectActions();

    //
    // Отключим на маленьких экранах некоторые возможности
    //
    QScreen* screen = QApplication::primaryScreen();
    if (screen->availableSize().width() < 1360) {
        m_menuManager->setMenuItemEnabled(kTwoPanelModeMenuIndex, false);
        m_settingsManager->disableTwoPanelsMode();
        if (screen->availableSize().width() < 1024) {
            m_settingsManager->disableCompactMode();
        }
    }
}

QMenu* ApplicationManager::createMenu()
{
    //
    // Сформируем меню
    //
#ifdef Q_OS_MAC
    m_menuBar = new QMenuBar(nullptr);
    QMenu* menu = m_menuBar->addMenu(tr("File"));
    m_editMenu = m_menuBar->addMenu(tr("Edit"));
    m_startUpManager->buildEditMenu(m_editMenu);
#else
    QMenu* menu = new QMenu(m_view);
#endif
    QAction* createNewProject = menu->addAction(tr("New"));
    QAction* openProject = menu->addAction(tr("Open"));
    QAction* saveProject = menu->addAction(tr("Save"));
    saveProject->setShortcut(QKeySequence::Save);
    m_view->addAction(saveProject);
    QAction* saveProjectAs = menu->addAction(tr("Save as..."));

    menu->addSeparator();
    // ... начать новую версию
    QAction* newVersion = menu->addAction(tr("New script version..."));
    // ... импорт
    QAction* importTo = menu->addAction(tr("Import..."));
    // ... экспорт
    QAction* exportTo = menu->addAction(tr("Export to..."));
    // ... предварительный просмотр
    QAction* printPreview = menu->addAction(tr("Print preview script"));
    printPreview->setShortcut(QKeySequence(Qt::Key_F12));
    m_view->addAction(printPreview);

    menu->addSeparator();
    QAction* twoPanelMode = menu->addAction(tr("Two panel mode"));
    twoPanelMode->setCheckable(true);
    twoPanelMode->setShortcut(QKeySequence(Qt::Key_F2));
    m_view->addAction(twoPanelMode);

    //
    // Настроим соединения
    //
    connect(createNewProject, &QAction::triggered, this, &ApplicationManager::aboutCreateNew);
    connect(openProject, &QAction::triggered, this, [this] { aboutLoad(); });
    connect(saveProject, &QAction::triggered, this, &ApplicationManager::aboutSave);
    connect(newVersion, &QAction::triggered, this, &ApplicationManager::aboutStartNewVersion);
    connect(saveProjectAs, &QAction::triggered, this, &ApplicationManager::aboutSaveAs);
    connect(importTo, &QAction::triggered, this, &ApplicationManager::aboutImport);
    connect(exportTo, &QAction::triggered, this, &ApplicationManager::aboutExport);
    connect(printPreview, &QAction::triggered, this, &ApplicationManager::printPreviewScript);
    connect(twoPanelMode, &QAction::triggered, m_settingsManager, &SettingsManager::setUseTwoPanelMode);

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

    connect(m_menu, &FlatButton::clicked, m_menuManager, &MenuManager::showMenu);

    connect(m_tabs, &SideTabBar::currentChanged, this, &ApplicationManager::currentTabIndexChanged);
    connect(m_tabsSecondary, &SideTabBar::currentChanged, this, &ApplicationManager::currentTabIndexChanged);
    //
    // Переавторизуемся
    //
    connect(m_tabs, &SideTabBar::indicatorActionClicked, [=] {
        m_synchronizationManager->restartSession();
        setSyncIndicator();
    });
    connect(m_tabs, &SideTabBar::indicatorMenuClicked, m_scenarioManager, &ScenarioManager::scrollToAdditionalCursor);

    connect(m_projectsManager, &ProjectsManager::recentProjectsUpdated, this, &ApplicationManager::updateRecentProjectsList);
    connect(m_projectsManager, &ProjectsManager::remoteProjectsUpdated, this, &ApplicationManager::updateRemoteProjectsList);
    connect(m_projectsManager, &ProjectsManager::recentProjectNameChanged, m_startUpManager, &StartUpManager::setRecentProjectName);
    connect(m_projectsManager, &ProjectsManager::remoteProjectNameChanged, m_startUpManager, &StartUpManager::setRemoteProjectName);

    connect(m_menuManager, &MenuManager::loginRequested, m_synchronizationManager, &SynchronizationManager::login);
    connect(m_menuManager, &MenuManager::signUpRequested, m_synchronizationManager, &SynchronizationManager::signUp);
    connect(m_menuManager, &MenuManager::verifyRequested, m_synchronizationManager, &SynchronizationManager::verify);
    connect(m_menuManager, &MenuManager::restoreRequested, m_synchronizationManager, &SynchronizationManager::restorePassword);
    connect(m_menuManager, &MenuManager::logoutRequested, m_synchronizationManager, &SynchronizationManager::logout);
    connect(m_menuManager, &MenuManager::renewSubscriptionRequested, m_synchronizationManager, &SynchronizationManager::renewSubscription);
    connect(m_menuManager, &MenuManager::userNameChangeRequested, m_synchronizationManager, &SynchronizationManager::changeUserName);
    connect(m_menuManager, &MenuManager::getSubscriptionInfoRequested, m_synchronizationManager, &SynchronizationManager::loadSubscriptionInfo);
    connect(m_menuManager, &MenuManager::passwordChangeRequested, m_synchronizationManager, &SynchronizationManager::changePassword);
    connect(m_menuManager, &MenuManager::updateRequested, m_startUpManager, &StartUpManager::showUpdateDialog);

    connect(m_startUpManager, &StartUpManager::createProjectRequested, this, &ApplicationManager::aboutCreateNew);
    connect(m_startUpManager, &StartUpManager::openProjectRequested, [=] { aboutLoad(); });
    connect(m_startUpManager, &StartUpManager::helpRequested, this, &ApplicationManager::aboutShowHelp);
    connect(m_startUpManager, &StartUpManager::crowdfindingJoinRequested, this, &ApplicationManager::aboutShowCrowdfinding);
    connect(m_startUpManager, &StartUpManager::refreshProjectsRequested, m_projectsManager, &ProjectsManager::refreshProjects);
    connect(m_startUpManager, &StartUpManager::refreshProjectsRequested, m_synchronizationManager, &SynchronizationManager::loadProjects);
    connect(m_startUpManager, &StartUpManager::openRecentProjectRequested, this, &ApplicationManager::aboutLoadFromRecent);
    connect(m_startUpManager, &StartUpManager::hideRecentProjectRequested, this, &ApplicationManager::hideLocalProject);
    connect(m_startUpManager, &StartUpManager::moveToCloudRecentProjectRequested, this, &ApplicationManager::moveLocalProjectToCloud);
    connect(m_startUpManager, &StartUpManager::openRemoteProjectRequested, this, &ApplicationManager::aboutLoadFromRemote);
    connect(m_startUpManager, &StartUpManager::editRemoteProjectRequested, this, &ApplicationManager::editRemoteProjectName);
    connect(m_startUpManager, &StartUpManager::removeRemoteProjectRequested, this, &ApplicationManager::removeRemoteProject);
    connect(m_startUpManager, &StartUpManager::shareRemoteProjectRequested, this, &ApplicationManager::shareRemoteProject);
    connect(m_startUpManager, &StartUpManager::unshareRemoteProjectRequested, this, &ApplicationManager::unshareRemoteProject);
    connect(m_startUpManager, &StartUpManager::updatePublished, m_menuManager, &MenuManager::showUpdateButton);

    connect(m_researchManager, &ResearchManager::scriptNameChanged, this, &ApplicationManager::updateWindowTitle);
    connect(m_researchManager, &ResearchManager::scriptHeaderChanged, m_scenarioManager, &ScenarioManager::setScriptHeader);
    connect(m_researchManager, &ResearchManager::scriptFooterChanged, m_scenarioManager, &ScenarioManager::setScriptFooter);
    connect(m_researchManager, &ResearchManager::sceneNumbersPrefixChanged, m_scenarioManager, &ScenarioManager::setSceneNumbersPrefix);
    connect(m_researchManager, &ResearchManager::sceneStartNumberChanged, m_scenarioManager, &ScenarioManager::setSceneStartNumber);
    connect(m_researchManager, &ResearchManager::versionsChanged, this, &ApplicationManager::updateWindowTitle);
    connect(m_researchManager, &ResearchManager::characterNameChanged, m_scenarioManager, &ScenarioManager::aboutCharacterNameChanged);
    connect(m_researchManager, &ResearchManager::refreshCharacters, m_scenarioManager, &ScenarioManager::aboutRefreshCharacters);
    connect(m_researchManager, &ResearchManager::locationNameChanged, m_scenarioManager, &ScenarioManager::aboutLocationNameChanged);
    connect(m_researchManager, &ResearchManager::refreshLocations, m_scenarioManager, &ScenarioManager::aboutRefreshLocations);
    connect(m_researchManager, &ResearchManager::addScriptVersionRequested, this, &ApplicationManager::aboutStartNewVersion);

    connect(m_scenarioManager, &ScenarioManager::showFullscreen, this, &ApplicationManager::aboutShowFullscreen);
    connect(m_scenarioManager, &ScenarioManager::updateScenarioRequest, this, &ApplicationManager::aboutUpdateLastChangeInfo);
    connect(m_scenarioManager, &ScenarioManager::updateScenarioRequest, m_synchronizationManager, &SynchronizationManager::aboutWorkSyncScenario);
    connect(m_scenarioManager, &ScenarioManager::updateScenarioRequest, m_synchronizationManager, &SynchronizationManager::aboutWorkSyncData);
    connect(m_scenarioManager, &ScenarioManager::updateCursorsRequest, m_synchronizationManager, &SynchronizationManager::aboutUpdateCursors);
    connect(m_scenarioManager, &ScenarioManager::linkActivated, this, &ApplicationManager::aboutInnerLinkActivated);
    connect(m_scenarioManager, &ScenarioManager::scriptFixedScenesChanged, m_researchManager, &ResearchManager::setSceneStartNumberEnabled);

    connect(m_statisticsManager, SIGNAL(needNewExportedScenario()), this, SLOT(aboutPrepareScenarioForStatistics()));
    connect(m_statisticsManager, &StatisticsManager::linkActivated, this, &ApplicationManager::aboutInnerLinkActivated);

    connect(m_settingsManager, &SettingsManager::applicationSettingsUpdated,
            this, &ApplicationManager::aboutApplicationSettingsUpdated);
    connect(m_settingsManager, &SettingsManager::researchSettingsUpdated,
            m_researchManager, &ResearchManager::updateSettings);
    connect(m_settingsManager, &SettingsManager::scenarioEditSettingsUpdated,
            m_researchManager, &ResearchManager::updateSettings);
    connect(m_settingsManager, &SettingsManager::cardsSettingsUpdated,
            m_scenarioManager, &ScenarioManager::aboutCardsSettingsUpdated);
    connect(m_settingsManager, &SettingsManager::scenarioEditSettingsUpdated,
            m_scenarioManager, &ScenarioManager::aboutTextEditSettingsUpdated);
    connect(m_settingsManager, &SettingsManager::navigatorSettingsUpdated,
            m_scenarioManager, &ScenarioManager::aboutNavigatorSettingsUpdated);
    connect(m_settingsManager, &SettingsManager::chronometrySettingsUpdated,
            m_scenarioManager, &ScenarioManager::aboutChronometrySettingsUpdated);
    connect(m_settingsManager, &SettingsManager::countersSettingsUpdated,
            m_scenarioManager, &ScenarioManager::aboutCountersSettingsUpdated);
    connect(m_settingsManager, &SettingsManager::scenarioEditSettingsUpdated, m_toolsManager, &ToolsManager::reloadTextEditSettings);

    connect(m_toolsManager, &ToolsManager::applyScriptRequested, m_scenarioManager, &ScenarioManager::setScriptXml);

    connect(m_researchManager, SIGNAL(researchChanged()), this, SLOT(aboutProjectChanged()));
    connect(m_scenarioManager, SIGNAL(scenarioChanged()), this, SLOT(aboutProjectChanged()));
    connect(m_exportManager, SIGNAL(scenarioTitleListDataChanged()), this, SLOT(aboutProjectChanged()));

    connect(m_synchronizationManager, &SynchronizationManager::syncClosedWithError, this, &ApplicationManager::aboutSyncClosedWithError);
    connect(m_synchronizationManager, &SynchronizationManager::networkStatusChanged, this, &ApplicationManager::setSyncIndicator);
    connect(m_synchronizationManager, &SynchronizationManager::logoutFinished, m_tabs, &SideTabBar::removeIndicator);

    connect(m_synchronizationManager, &SynchronizationManager::applyPatchRequested, m_scenarioManager, &ScenarioManager::aboutApplyPatch);
    connect(m_synchronizationManager, &SynchronizationManager::applyPatchesRequested, m_scenarioManager, &ScenarioManager::aboutApplyPatches);
    connect(m_synchronizationManager, &SynchronizationManager::cursorsUpdated, m_scenarioManager, &ScenarioManager::aboutCursorsUpdated);
    connect(m_synchronizationManager, &SynchronizationManager::cursorsUpdated, [this] (const QMap<QString, int>& _cursors, bool _isDraft) {
        if (_isDraft == m_scenarioManager->workModeIsDraft()) {
            m_tabs->setIndicatorMenu(_cursors.keys().toVector());
        }
    });

    connect(m_synchronizationManager, &SynchronizationManager::loginAccepted, m_menuManager, &MenuManager::completeLogin);
    connect(m_synchronizationManager, &SynchronizationManager::loginAccepted, [this] { m_startUpManager->setRemoteProjectsVisible(true); });
    connect(m_synchronizationManager, &SynchronizationManager::signUpFinished, m_menuManager, &MenuManager::userAfterSignUp);
    connect(m_synchronizationManager, &SynchronizationManager::verified, m_menuManager, &MenuManager::userAfterSignUp);
    connect(m_synchronizationManager, &SynchronizationManager::passwordRestored, m_menuManager, &MenuManager::userPassRestored);
    connect(m_synchronizationManager, &SynchronizationManager::logoutFinished, m_menuManager, &MenuManager::completeLogout);
    connect(m_synchronizationManager, &SynchronizationManager::logoutFinished, [this] { m_startUpManager->setRemoteProjectsVisible(false); });
    connect(m_synchronizationManager, &SynchronizationManager::passwordChanged, m_menuManager, &MenuManager::passwordChanged);
    connect(m_synchronizationManager, &SynchronizationManager::subscriptionInfoLoaded, m_menuManager, &MenuManager::setSubscriptionInfo);
    connect(m_synchronizationManager, &SynchronizationManager::subscriptionInfoLoaded, m_projectsManager, &ProjectsManager::setRemoteProjectsSyncAvailable);
    connect(m_synchronizationManager, &SynchronizationManager::projectsLoaded, m_projectsManager, &ProjectsManager::setRemoteProjects);

    //
    // Когда пользователь вышел из своего аккаунта, закрываем текущий проект, если он из облака
    //
    connect(m_synchronizationManager, &SynchronizationManager::logoutFinished, [=] {
        if (m_projectsManager->currentProject().isRemote()) {
            closeCurrentProject();
        }
    });
}

void ApplicationManager::initStyleSheet()
{
    //
    // Загрузим стиль
    //
    QFile styleSheetFile(
                QString(":/Interface/UI/style-desktop%2.qss")
                .arg(QLocale().textDirection() == Qt::RightToLeft ? "-rtl" : ""));
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
    m_menu->setProperty("topPanelLeftBordered", true);
    //
    m_menuSecondary->setProperty("inTopPanel", true);
    m_menuSecondary->setProperty("topPanelTopBordered", true);
    m_menuSecondary->setProperty("topPanelRightBordered", true);
    m_menuSecondary->setProperty("topPanelLeftBordered", true);
}

void ApplicationManager::reloadApplicationSettings()
{
    //
    // Установить используемый приложением шрифт
    //
    QFont font("Roboto Regular");
    font.setPixelSize(12);
    QApplication::setFont(font);

    //
    // Внешний вид приложения
    //
    const bool useDarkTheme =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/use-dark-theme",
                DataStorageLayer::SettingsStorage::ApplicationSettings)
            .toInt();
    m_view->setUseDarkTheme(useDarkTheme);
    {
        //
        // Настраиваем палитру и стилевые надстройки в зависимости от темы
        //
        QPalette palette = QStyleFactory::create("Fusion")->standardPalette();

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
            palette.setColor(QPalette::Midlight, QColor("#424140"));
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
        // Применяем палитру
        //
        qApp->setPalette(palette);

        //
        // Чтобы все цветовые изменения подхватились, нужно заново переустановить стиль
        //
        const bool useCompactMode =
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "application/compact-mode",
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                .toInt();
        QFile styleSheetFile(
                    QString(":/Interface/UI/style-desktop%1%2.qss")
                    .arg(useCompactMode ? "-compact" : "")
                    .arg(QLocale().textDirection() == Qt::RightToLeft ? "-rtl" : ""));
        styleSheetFile.open(QIODevice::ReadOnly);
        QString styleSheet = styleSheetFile.readAll();
        styleSheetFile.close();
        styleSheet.replace("_THEME_POSTFIX", useDarkTheme ? "-dark" : "");
        m_view->setStyleSheet(styleSheet);

        //
        // Настроим боковую панель в зависимости от необходимости быть компактным
        //
        m_menu->setIcons(QIcon(useCompactMode ? ":/Graphics/Iconset/menu.svg" : ""));
        m_tabs->setCompactMode(useCompactMode);
    }

    //
    // Автосохранение
    //
    const bool autosave =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/autosave",
                DataStorageLayer::SettingsStorage::ApplicationSettings)
            .toInt();
    const int autosaveInterval =
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
    m_menuManager->menu()->actions().value(kTwoPanelModeMenuIndex)->setChecked(twoPanelsMode);
    //
    // Если не применять этот хак, то в редакторе сценария пропадает курсор
    // Возникает, только когда редактор сценария был на экране, при отключении второй панели
    //
    if (!twoPanelsMode
        && m_tabsWidgetsSecondary->currentWidget() == m_scenarioManager->view()) {
        m_tabsWidgets->insertWidget(SCENARIO_TAB_INDEX, m_scenarioManager->view());
    }
    m_menuSecondary->setVisible(twoPanelsMode);
    m_tabsSecondary->setVisible(twoPanelsMode);
    m_tabsWidgetsSecondary->setVisible(twoPanelsMode);
    m_splitter->handle(1)->setEnabled(twoPanelsMode);
    m_splitter->setHandleWidth(twoPanelsMode ? 1 : 0);

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
    const bool showCardsModule =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/modules/cards",
                DataStorageLayer::SettingsStorage::ApplicationSettings)
            .toInt();
    m_tabs->tab(SCENARIO_CARDS_TAB_INDEX)->setVisible(showCardsModule);
    m_tabsSecondary->tab(SCENARIO_CARDS_TAB_INDEX)->setVisible(showCardsModule);
    //
    const bool showScenarioModule =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/modules/scenario",
                DataStorageLayer::SettingsStorage::ApplicationSettings)
            .toInt();
    m_tabs->tab(SCENARIO_TAB_INDEX)->setVisible(showScenarioModule);
    m_tabsSecondary->tab(SCENARIO_TAB_INDEX)->setVisible(showScenarioModule);
    //
    const bool showStatisticsModule =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/modules/statistics",
                DataStorageLayer::SettingsStorage::ApplicationSettings)
            .toInt();
    m_tabs->tab(STATISTICS_TAB_INDEX)->setVisible(showStatisticsModule);
    m_tabsSecondary->tab(STATISTICS_TAB_INDEX)->setVisible(showStatisticsModule);
    //
    const bool showToolsModule =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/modules/tools",
                DataStorageLayer::SettingsStorage::ApplicationSettings)
            .toInt();
    m_tabs->tab(TOOLS_TAB_INDEX)->setVisible(showToolsModule);
    m_tabsSecondary->tab(TOOLS_TAB_INDEX)->setVisible(showToolsModule);

    //
    // Отключим на маленьких экранах некоторые возможности
    //
    QScreen* screen = QApplication::primaryScreen();
    if (screen->availableSize().width() < 1360) {
        m_menuManager->menu()->actions()[kTwoPanelModeMenuIndex]->setEnabled(false);
        m_menuManager->menu()->actions()[kTwoPanelModeMenuIndex]->setVisible(false);
        m_settingsManager->disableTwoPanelsMode();
        if (screen->availableSize().width() < 1024) {
            m_settingsManager->disableCompactMode();
        }
    }
}

void ApplicationManager::updateWindowTitle()
{
    if (!m_projectsManager->isCurrentProjectValid()) {
        m_view->setWindowTitle(tr("KIT Scenarist"));
        return;
    }

    //
    // Обновим название текущего проекта, если он локальный
    //
    if (m_projectsManager->currentProject().isLocal()) {
        m_projectsManager->setCurrentProjectName(m_researchManager->scenarioName());
    }

    const QString projectFileName =
            QString("%1 - %2 %3")
            .arg(ProjectsManager::currentProject().name())
            .arg(DataStorageLayer::StorageFacade::scriptVersionStorage()->currentVersionName())
            .arg(TextUtils::directedText((m_projectsManager->currentProject().isLocal()
                                          ? tr("on local computer")
                                          : tr("in cloud")),
                                         '[', ']'));
#ifdef Q_OS_MAC
    m_view->setWindowTitle(projectFileName);
#else
    m_view->setWindowTitle(tr("%1[*] - KIT Scenarist").arg(projectFileName));
#endif
}
