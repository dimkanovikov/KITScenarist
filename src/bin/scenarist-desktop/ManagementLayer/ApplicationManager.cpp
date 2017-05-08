#include "ApplicationManager.h"

#include "StartUp/StartUpManager.h"
#include "Research/ResearchManager.h"
#include "Scenario/ScenarioCardsManager.h"
#include "Scenario/ScenarioManager.h"
#include "Statistics/StatisticsManager.h"
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
#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>

#include <3rd_party/Widgets/SideBar/SideBar.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.h>

#include <UserInterfaceLayer/ApplicationView.h>
#include <UserInterfaceLayer/Project/AddProjectDialog.h>
#include <UserInterfaceLayer/Project/ShareDialog.h>

#include <QApplication>
#include <QComboBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QProcess>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardItemModel>
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
using UserInterface::ShareDialog;

namespace {
    /**
     * @brief Номера пунктов меню
     */
    /** @{ */
    const int PRINT_PREVIEW_MENU_INDEX = 7;
    const int TWO_PANEL_MODE_MENU_INDEX = 9;
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
    const int SETTINGS_TAB_INDEX = 5;
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
}

ApplicationManager::~ApplicationManager()
{
    delete m_view;
    m_view = 0;
}

void ApplicationManager::exec(const QString& _fileToOpen)
{
    reloadApplicationSettings();
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
    m_startUpManager->setProgressLoginLabel(true);
    if (!m_synchronizationManager->autoLogin()) {
        m_startUpManager->setProgressLoginLabel(false);
    }
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
        AddProjectDialog dlg(m_view);
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
                    // Если задан, то создаём новый проект в заданном файле
                    //
                    const QString newProjectPath = dlg.projectFilePath();
                    const QString importFilePath = dlg.importFilePath();
                    createNewLocalProject(newProjectPath, importFilePath);
                    break;
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
        if (!newProjectFilePath.endsWith(PROJECT_FILE_EXTENSION)) {
            newProjectFilePath.append(PROJECT_FILE_EXTENSION);
        }

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
            goToEditCurrentProject();
            //
            // ... и импортируем, если надо
            //
            if (!_importFilePath.isEmpty()) {
                m_importManager->importScenario(m_scenarioManager->scenario(), _importFilePath);
            }
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
                    tr("Can't write to file. Maybe it opened in other application. Please, close it and retry.");
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
    // Переключаемся на работу с новым проектом
    //
    const bool isRemote = false;
    if (m_projectsManager->setCurrentProject(newProjectId, isRemote)) {
        //
        // ... перейдём к редактированию
        //
        goToEditCurrentProject();
        //
        // ... и импортируем, если надо
        //
        if (!_importFilePath.isEmpty()) {
            m_importManager->importScenario(m_scenarioManager->scenario(), _importFilePath);
        }
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
                m_view->setWindowModified(true);

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
    // Если какие-то данные изменены
    //
    if (m_view->isWindowModified()) {
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
            ::updateWindowModified(m_view, false);

            //
            // Если необходимо создадим резервную копию закрываемого файла
            //
            QString baseBackupName = "";
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
                                                   tr("Can't write you changes to project. There is some internal database error: %1 "
                                                      "Please check that file is exists and you have permissions to write in it. Retry to save?")
                                                   .arg(DatabaseLayer::Database::lastError()),
                                                   QDialogButtonBox::Yes | QDialogButtonBox::No, QDialogButtonBox::Yes);
                //
                // ... пробуем повторно открыть базу данных и записать в неё изменения
                //
                if (messageResult == QDialogButtonBox::Yes) {
                    DatabaseLayer::Database::setCurrentFile(DatabaseLayer::Database::currentFile());
                    aboutSave();
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
                            tr("Can't write you changes to project located at <b>%1</b> becourse file isn't exist. "
                               "Please move file back and retry to save. Retry to save?")
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
        ::updateWindowModified(m_view, false);
    }
}

void ApplicationManager::loadCurrentProjectSettings(const QString& _projectPath)
{
    //
    // Восстановим используемые модули
    //

    const int lastModuleLeft =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                QString("projects/%1/last-active-module_left").arg(_projectPath),
                DataStorageLayer::SettingsStorage::ApplicationSettings,
                QString::number(RESEARCH_TAB_INDEX)
                ).toInt();
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
    const Project project = m_projectsManager->project(_index, IS_REMOTE);
    const QString newName =
            QLightBoxInputDialog::getText(m_view, tr("Change project name"),
                tr("Enter new name for project"), project.name());
    if (!newName.isEmpty()) {
        m_synchronizationManager->updateProjectName(project.id(), newName);
        m_projectsManager->setCurrentProjectName(newName);
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
            m_synchronizationManager->removeProject(project.id());
        }
    }
    //
    // А если нет, то только отписаться от него
    //
    else {
        if (QLightBoxMessage::question(m_view, QString::null, tr("Are you sure to remove your subscription to project <b>%1</b>?").arg(project.name()))
            == QDialogButtonBox::Yes) {
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
    bool isActiveInternet = m_synchronizationManager->isInternetConnectionActive();
    bool isRemoteProject = m_projectsManager->isCurrentProjectValid() &&
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
            iconPath = ":/Graphics/Icons/Indicator/synced.png";
            indicatorText = tr("Project synchronized");
        } else {
            //
            // Локальный проект
            //
            iconPath = ":/Graphics/Icons/Indicator/connected.png";
        }
        waveColor = QColor(0, 255, 0, 40);
    } else {
        //
        // Если интернета нет, то всегда показываем значок отключенного интернета
        //
        iconPath = ":/Graphics/Icons/Indicator/disconnected.png";
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
            if (m_startUpManager->isOnLoginDialog()) {
                m_startUpManager->retryLastAction(_error);
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
                m_startUpManager->setProgressLoginLabel(false);
            }
            break;
        }

        //
        // Проблемы с вводом логина и пароля
        //
        case Sync::IncorrectLoginError:
        case Sync::IncorrectPasswordError: {
            error = tr("Incorrect username or password.");
            m_startUpManager->setProgressLoginLabel(false);
            m_startUpManager->retryLogin(error);
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
            error = tr("Can't correct load all data from service. "
                       "Please check your internet connection quality and refresh synchronization.\n\n"
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
            reactivateIcon = QIcon(":/Graphics/Icons/Editing/refresh.png");
            break;
        }

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

        case Sync::StorageSizeFinished: {
            title = tr("Sync not available");
            error = tr("You have exhausted all available for the use of space on a server.\n\n"
                       "Project didn't synchronized.");
            break;
        }

        //
        // Такой email уже зарегистрирован
        //
        case Sync::EmailAlreadyRegisteredError: {
            error = tr("Email already exist");
            m_startUpManager->retrySignUp(error);
            break;
        }

        //
        // Слишком слабый пароль
        //
        case Sync::WeakPasswordError: {
            error = tr("Password too weak");
            m_startUpManager->showPasswordError(error);
            break;
        }

        //
        // Старый пароль некорректен
        //
        case Sync::IncorrectOldPasswordError: {
            error = tr("Invalid old password");
            m_startUpManager->showPasswordError(error);
            break;
        }

        //
        // Неверный код валидации
        //
        case Sync::IncorrectValidationCodeError: {
            error = tr("Wrong validation code");
            m_startUpManager->retryVerify(error);
            break;
        }

        //
        // Неверный email для восстановления пароля
        //
        case Sync::EmailNotRegisteredError: {
            error = tr("Wrong email");
            m_startUpManager->retryLogin(error);
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
            m_tabs->addIndicator(QIcon(":/Graphics/Icons/Indicator/unsynced.png"));
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
    m_importManager->importScenario(m_scenarioManager->scenario(), m_scenarioManager->cursorPosition());
}

void ApplicationManager::aboutExport()
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
        QLightBoxProgress progress(m_view);
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
    if (isProjectLoaded()) {
        ::updateWindowModified(m_view, true);
        m_statisticsManager->scenarioTextChanged();
    }
}

void ApplicationManager::aboutShowFullscreen()
{
    const char* IS_MAXIMIZED_PROPERTY = "isMaximized";

    if (m_view->isFullScreen()) {
        //
        // Возвращаемся в состояние предшествовавшее полноэкранному режиму
        //
        m_menu->show();
        m_tabs->show();
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
            foreach (const QString parameter, parameters) {
                const QStringList paramaterDetails = parameter.split("=");
                if (paramaterDetails.first() == "position") {
                    cursorPosition = paramaterDetails.last().toInt();
                }
            }

            if (cursorPosition != INVALID_CURSOR_POSITION) {
                if (m_tabsSecondary->isVisible() &&
                    m_tabs->currentTab() == STATISTICS_TAB_INDEX) {
                    m_tabsSecondary->setCurrentTab(SCENARIO_TAB_INDEX);
                } else {
                    m_tabs->setCurrentTab(SCENARIO_TAB_INDEX);
                }
                //
                // Выполняем события, чтобы пропустить первую прокрутку текста, после запуска
                // приложения к последнему рабочему месту в сценарии
                //
                QApplication::processEvents();
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
    bool success = true;

    //
    // Если какие-то данные изменены
    //
    if (m_view->isWindowModified()) {
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
    QLightBoxProgress progress(m_view);
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

    //
    // Активируем вкладки
    //
    ::enableActionsOnProjectOpen();

    //
    // Настроим индикатор
    //
    if (m_projectsManager->currentProject().isRemote()) {
        setSyncIndicator();
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
    // Загрузить данные из файла
    // Делать это нужно после того, как все данные синхронизировались
    //
    m_researchManager->loadCurrentProject();
    m_statisticsManager->loadCurrentProject();

    //
    // После того, как все данные загружены и синхронизированы, сохраняем проект
    //
    if (m_projectsManager->currentProject().isRemote()) {
        m_view->setWindowModified(true);
        aboutSave();
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
    loadCurrentProjectSettings(ProjectsManager::currentProject().path());

    //
    // Обновим название текущего проекта, т.к. данные о проекте теперь загружены
    //
    updateWindowTitle();

    //
    // Обновим информацию о последнем изменении
    //
    aboutUpdateLastChangeInfo();

    //
    // Закроем уведомление
    //
    progress.finish();
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

        //
        // Перейти на стартовую вкладку
        //
        m_tabs->setCurrentTab(0);
    }
}

bool ApplicationManager::isProjectLoaded() const
{
    return m_projectsManager->isCurrentProjectValid();
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
    m_tabs->addTab(tr("Start"), QIcon(":/Graphics/Icons/start.png"));
    g_disableOnStartActions << m_tabs->addTab(tr("Research"), QIcon(":/Graphics/Icons/research.png"));
    g_disableOnStartActions << m_tabs->addTab(tr("Cards"), QIcon(":/Graphics/Icons/cards.png"));
    g_disableOnStartActions << m_tabs->addTab(tr("Scenario"), QIcon(":/Graphics/Icons/script.png"));
    g_disableOnStartActions << m_tabs->addTab(tr("Statistics"), QIcon(":/Graphics/Icons/statistics.png"));
    m_tabs->addTab(tr("Settings"), QIcon(":/Graphics/Icons/settings.png"));
    //
    // ... вспомогательную
    //
    m_tabsSecondary->setCompactMode(true);
    m_tabsSecondary->addTab(tr("Start"), QIcon(":/Graphics/Icons/start.png"));
    g_disableOnStartActions << m_tabsSecondary->addTab(tr("Research"), QIcon(":/Graphics/Icons/research.png"));
    g_disableOnStartActions << m_tabsSecondary->addTab(tr("Cards"), QIcon(":/Graphics/Icons/cards.png"));
    g_disableOnStartActions << m_tabsSecondary->addTab(tr("Scenario"), QIcon(":/Graphics/Icons/script.png"));
    g_disableOnStartActions << m_tabsSecondary->addTab(tr("Statistics"), QIcon(":/Graphics/Icons/statistics.png"));
    m_tabsSecondary->addTab(tr("Settings"), QIcon(":/Graphics/Icons/settings.png"));
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
#ifdef Q_OS_MAC
    QMenuBar *menuBar = new QMenuBar(0);
    QMenu* menu = menuBar->addMenu(tr("File"));
#else
    QMenu* menu = new QMenu(m_menu);
#endif
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

    menu->addSeparator();
    QAction* twoPanelMode = menu->addAction(tr("Two Panel Mode"));
    twoPanelMode->setCheckable(true);
    twoPanelMode->setShortcut(QKeySequence(Qt::Key_F2));

    //
    // Настроим соединения
    //
    connect(createNewProject, SIGNAL(triggered()), this, SLOT(aboutCreateNew()));
    connect(openProject, SIGNAL(triggered()), this, SLOT(aboutLoad()));
    connect(saveProject, SIGNAL(triggered()), this, SLOT(aboutSave()));
    connect(saveProjectAs, SIGNAL(triggered()), this, SLOT(aboutSaveAs()));
    connect(import, SIGNAL(triggered()), this, SLOT(aboutImport()));
    connect(exportTo, SIGNAL(triggered()), this, SLOT(aboutExport()));
    connect(printPreview, SIGNAL(triggered()), this, SLOT(aboutPrintPreview()));
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

    connect(m_menu, SIGNAL(clicked()), m_menu, SLOT(showMenu()));
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

    connect(m_projectsManager, SIGNAL(recentProjectsUpdated()), this, SLOT(aboutUpdateProjectsList()));
    connect(m_projectsManager, SIGNAL(remoteProjectsUpdated()), this, SLOT(aboutUpdateProjectsList()));
    connect(m_startUpManager, &StartUpManager::loginRequested,
            m_synchronizationManager, &SynchronizationManager::login);
    connect(m_startUpManager, &StartUpManager::signUpRequested,
            m_synchronizationManager, &SynchronizationManager::signUp);
    connect(m_startUpManager, &StartUpManager::verifyRequested,
            m_synchronizationManager, &SynchronizationManager::verify);
    connect(m_startUpManager, &StartUpManager::restoreRequested,
            m_synchronizationManager, &SynchronizationManager::restorePassword);
    connect(m_startUpManager, &StartUpManager::logoutRequested,
            m_synchronizationManager, &SynchronizationManager::logout);
    connect(m_startUpManager, &StartUpManager::renewSubscriptionRequested,
            m_synchronizationManager, &SynchronizationManager::renewSubscription);
    connect(m_startUpManager, &StartUpManager::userNameChangeRequested,
            m_synchronizationManager, &SynchronizationManager::changeUserName);
    connect(m_startUpManager, &StartUpManager::getSubscriptionInfoRequested,
            m_synchronizationManager, &SynchronizationManager::loadSubscriptionInfo);
    connect(m_startUpManager, &StartUpManager::passwordChangeRequested,
            m_synchronizationManager, &SynchronizationManager::changePassword);
    //
    connect(m_startUpManager, &StartUpManager::createProjectRequested, this, &ApplicationManager::aboutCreateNew);
    connect(m_startUpManager, &StartUpManager::openProjectRequested, [=] { aboutLoad(); });
    connect(m_startUpManager, &StartUpManager::helpRequested, this, &ApplicationManager::aboutShowHelp);
    connect(m_startUpManager, &StartUpManager::refreshProjectsRequested, m_projectsManager, &ProjectsManager::refreshProjects);
    connect(m_startUpManager, &StartUpManager::refreshProjectsRequested, m_synchronizationManager, &SynchronizationManager::loadProjects);
    connect(m_startUpManager, &StartUpManager::openRecentProjectRequested, this, &ApplicationManager::aboutLoadFromRecent);
    connect(m_startUpManager, &StartUpManager::hideRecentProjectRequested, this, &ApplicationManager::hideLocalProject);
    connect(m_startUpManager, &StartUpManager::openRemoteProjectRequested, this, &ApplicationManager::aboutLoadFromRemote);
    connect(m_startUpManager, &StartUpManager::editRemoteProjectRequested, this, &ApplicationManager::editRemoteProjectName);
    connect(m_startUpManager, &StartUpManager::removeRemoteProjectRequested, this, &ApplicationManager::removeRemoteProject);
    connect(m_startUpManager, &StartUpManager::shareRemoteProjectRequested, this, &ApplicationManager::shareRemoteProject);
    connect(m_startUpManager, &StartUpManager::unshareRemoteProjectRequested, this, &ApplicationManager::unshareRemoteProject);

    connect(m_researchManager, &ResearchManager::scenarioNameChanged, this, &ApplicationManager::updateWindowTitle);
    connect(m_researchManager, &ResearchManager::characterNameChanged, m_scenarioManager, &ScenarioManager::aboutCharacterNameChanged);
    connect(m_researchManager, &ResearchManager::refreshCharacters, m_scenarioManager, &ScenarioManager::aboutRefreshCharacters);
    connect(m_researchManager, &ResearchManager::locationNameChanged, m_scenarioManager, &ScenarioManager::aboutLocationNameChanged);
    connect(m_researchManager, &ResearchManager::refreshLocations, m_scenarioManager, &ScenarioManager::aboutRefreshLocations);

    connect(m_scenarioManager, &ScenarioManager::showFullscreen, this, &ApplicationManager::aboutShowFullscreen);
    connect(m_scenarioManager, &ScenarioManager::updateScenarioRequest, this, &ApplicationManager::aboutUpdateLastChangeInfo);
    connect(m_scenarioManager, &ScenarioManager::updateScenarioRequest, m_synchronizationManager, &SynchronizationManager::aboutWorkSyncScenario);
    connect(m_scenarioManager, &ScenarioManager::updateScenarioRequest, m_synchronizationManager, &SynchronizationManager::aboutWorkSyncData);
    connect(m_scenarioManager, &ScenarioManager::updateCursorsRequest, m_synchronizationManager, &SynchronizationManager::aboutUpdateCursors);

    connect(m_statisticsManager, SIGNAL(needNewExportedScenario()), this, SLOT(aboutPrepareScenarioForStatistics()));
    connect(m_statisticsManager, &StatisticsManager::linkActivated, this, &ApplicationManager::aboutInnerLinkActivated);

    connect(m_settingsManager, SIGNAL(applicationSettingsUpdated()),
            this, SLOT(aboutApplicationSettingsUpdated()));
    connect(m_settingsManager, &SettingsManager::scenarioEditSettingsUpdated,
            m_researchManager, &ResearchManager::updateSettings);
    connect(m_settingsManager, &SettingsManager::cardsSettingsUpdated,
            m_scenarioManager, &ScenarioManager::aboutCardsSettingsUpdated);
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
    connect(m_exportManager, SIGNAL(scenarioTitleListDataChanged()), this, SLOT(aboutProjectChanged()));

    connect(m_synchronizationManager, SIGNAL(applyPatchRequested(QString,bool)),
            m_scenarioManager, SLOT(aboutApplyPatch(QString,bool)));
    connect(m_synchronizationManager, SIGNAL(applyPatchesRequested(QList<QString>,bool)),
            m_scenarioManager, SLOT(aboutApplyPatches(QList<QString>,bool)));
    connect(m_synchronizationManager, SIGNAL(cursorsUpdated(QMap<QString,int>,bool)),
            m_scenarioManager, SLOT(aboutCursorsUpdated(QMap<QString,int>,bool)));
    connect(m_synchronizationManager, &SynchronizationManager::cursorsUpdated, [this] (const QMap<QString, int>& _cursors, bool _isDraft) {
        if (_isDraft == m_scenarioManager->workModeIsDraft()) {
            m_tabs->setIndicatorMenu(QVector<QString>::fromList(_cursors.keys()));
        }
    });
    connect(m_synchronizationManager, SIGNAL(syncClosedWithError(int,QString)),
            this, SLOT(aboutSyncClosedWithError(int,QString)));
    connect(m_synchronizationManager, &SynchronizationManager::networkStatusChanged,
            this, &ApplicationManager::setSyncIndicator);

    connect(m_synchronizationManager, &SynchronizationManager::loginAccepted,
            m_startUpManager, &StartUpManager::completeLogin);
    connect(m_synchronizationManager, &SynchronizationManager::signUpFinished,
            m_startUpManager, &StartUpManager::userAfterSignUp);
    connect(m_synchronizationManager, &SynchronizationManager::verified,
            m_startUpManager, &StartUpManager::userAfterSignUp);
    connect(m_synchronizationManager, &SynchronizationManager::passwordRestored,
            m_startUpManager, &StartUpManager::userPassRestored);
    connect(m_synchronizationManager, &SynchronizationManager::logoutFinished,
            m_startUpManager, &StartUpManager::completeLogout);
    connect(m_synchronizationManager, &SynchronizationManager::passwordChanged,
            m_startUpManager, &StartUpManager::passwordChanged);
    connect(m_synchronizationManager, &SynchronizationManager::subscriptionInfoLoaded,
            m_startUpManager, &StartUpManager::setSubscriptionInfo);
    connect(m_synchronizationManager, &SynchronizationManager::subscriptionInfoLoaded,
            m_projectsManager, &ProjectsManager::setRemoteProjectsSyncAvailable);
    connect(m_synchronizationManager, &SynchronizationManager::syncClosedWithError,
            this, &ApplicationManager::aboutSyncClosedWithError);
    connect(m_synchronizationManager, &SynchronizationManager::projectsLoaded,
            m_projectsManager, &ProjectsManager::setRemoteProjects);
    connect(m_synchronizationManager, &SynchronizationManager::logoutFinished,
            m_tabs, &SideTabBar::removeIndicator);

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
        QFile styleSheetFile(":/Interface/UI/style-desktop.qss");
        styleSheetFile.open(QIODevice::ReadOnly);
        QString styleSheet = styleSheetFile.readAll();
        styleSheetFile.close();
        styleSheet.replace("_THEME_POSTFIX", useDarkTheme ? "-dark" : "");
        m_view->setStyleSheet(styleSheet);
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
    m_menu->menu()->actions().value(TWO_PANEL_MODE_MENU_INDEX)->setChecked(twoPanelsMode);
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
}

void ApplicationManager::updateWindowTitle()
{
    //
    // Обновим название текущего проекта, если он локальный
    //
    if (m_projectsManager->currentProject().isLocal()) {
        m_projectsManager->setCurrentProjectName(m_researchManager->scenarioName());
    }

    const QString projectFileName =
            QString("%1 [%2]")
            .arg(ProjectsManager::currentProject().name())
            .arg(m_projectsManager->currentProject().isLocal() ? tr("local") : tr("in cloud"));
#ifdef Q_OS_MAC
    m_view->setWindowTitle(projectFileName);
#else
    m_view->setWindowTitle(tr("%1[*] - KIT Scenarist").arg(projectFileName));
#endif
}
