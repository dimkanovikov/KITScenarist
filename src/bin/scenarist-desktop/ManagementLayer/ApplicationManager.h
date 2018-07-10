#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <3rd_party/Helpers/BackupHelper.h>

#include <QObject>
#include <QTimer>

class FlatButton;
class SideTabBar;
class QLabel;
class QSplitter;
class QStackedWidget;
class QToolButton;
class QMenu;
class QMenuBar;

namespace UserInterface {
    class ApplicationView;
    class MenuView;
}

namespace ManagementLayer
{
    class ProjectsManager;
    class MenuManager;
    class StartUpManager;
    class ResearchManager;
    class ScenarioManager;
    class StatisticsManager;
    class ToolsManager;
    class SettingsManager;
    class ExportManager;
    class ImportManager;
    class SynchronizationManager;


    /**
     * @brief Управляющий приложением
     */
    class ApplicationManager : public QObject
    {
        Q_OBJECT

    public:
        explicit ApplicationManager(QObject *parent = 0);
        ~ApplicationManager();

    public slots:
        /**
         * @brief Запуск приложения
         */
        void exec(const QString& _fileToOpen);

        /**
         * @brief Открыть файл
         */
        void openFile(const QString& _fileToOpen);

        /**
         * @brief Сделать работу необходимую при старте
         * проверка отчётов об ошибках
         * проверка выхода новых версий
         * автризация
         */
        void makeStartUpChecks();

    private slots:
        /**
         * @brief Обновить списки проектов на стартовой странице
         */
        /** @{ */
        void aboutUpdateProjectsLists();
        void updateRecentProjectsList();
        void updateRemoteProjectsList();
        /** @} */


        /**
         * @brief Создать новый
         */
        /** @{ */
        void aboutCreateNew();
        void createNewLocalProject(const QString& _filePath, const QString& _importFilePath);
        void createNewRemoteProject(const QString& _projectName, const QString& _importFilePath);
        /** @} */

        /**
         * @brief Сохранить как...
         */
        void aboutSaveAs();

        /**
         * @brief Сохранить в файл
         */
        void aboutSave();

        /**
         * @brief Сохранить версию сценария
         */
        void aboutSaveVersion();

        /**
         * @brief Сохранить настройки текущего проекта
         */
        void saveCurrentProjectSettings(const QString& _projectPath);

        /**
         * @brief Загрузить
         *
         * Если имя файла не задано, то будет вызван диалог выбора файла
         */
        void aboutLoad(const QString& _fileName = QString());

        /**
         * @brief Загрузить настройки текущего проекта
         */
        void loadCurrentProjectSettings(const QString& _projectPath);

        /**
         * @brief Открыть справку
         */
        void aboutShowHelp();

        /**
         * @brief Загрузить выбранный из списка локальный проект
         */
        void aboutLoadFromRecent(const QModelIndex& _projectIndex);

        /**
         * @brief Скрыть проект из списка локальных
         */
        void hideLocalProject(const QModelIndex& _index);

        /**
         * @brief Перенести локальный проект в облако
         */
        void moveLocalProjectToCloud(const QModelIndex& _index);

        /**
         * @brief Загрузить выбранный проект из облака
         */
        void aboutLoadFromRemote(const QModelIndex& _projectIndex);

        /**
         * @brief Сменить имя проекта из облака
         */
        void editRemoteProjectName(const QModelIndex& _index);

        /**
         * @brief Удалить проект из облака
         */
        void removeRemoteProject(const QModelIndex& _index);

        /**
         * @brief Открыть доступ к проекту из облака
         */
        void shareRemoteProject(const QModelIndex& _index);

        /**
         * @brief Закрыть доступ к проекту из облака
         */
        void unshareRemoteProject(const QModelIndex& _index, const QString& _userEmail);

        /**
         * @brief Отобразить индикатор активной синхронизации
         */
        void setSyncIndicator();

        /**
         * @brief Обновить информацию о последнем изменении в индикаторе синхронизации
         */
        void aboutUpdateLastChangeInfo();

        /**
         * @brief Синхронизация остановленна с ошибкой
         */
        void aboutSyncClosedWithError(int _errorCode, const QString& _error = QString::null);

        /**
         * @brief Импортировать документ
         */
        void aboutImport();

        /**
         * @brief Экспортировать документ
         */
        void aboutExport();

        /**
         * @brief Предварительный просмотр и печать
         */
        void aboutPrintPreview();

        /**
         * @brief Закрыть приложение
         */
        void aboutExit();

        /**
         * @brief Обновлены общие настройки приложения
         */
        void aboutApplicationSettingsUpdated();

        /**
         * @brief Проект был изменён
         */
        void aboutProjectChanged();

        /**
         * @brief Включить/выключить полноэкранный режим
         */
        void aboutShowFullscreen();

        /**
         * @brief Подготовить документ для подсчёта статистики
         */
        void aboutPrepareScenarioForStatistics();

        /**
         * @brief Активирована внутренняя ссылка приложения
         */
        void aboutInnerLinkActivated(const QUrl& _url);

    protected:
        /**
         * @brief Переопределяем, для перехвата события простоя приложения
         */
        bool event(QEvent* _event);

    private:
        /**
         * @brief Загрузить состояние окна
         */
        void loadViewState();

        /**
         * @brief Сохранить состояние окна
         */
        void saveViewState();

        /**
         * @brief Изменился индекс текущей вкладки
         */
        void currentTabIndexChanged();

        /**
         * @brief Если проект был изменён, но не сохранён предложить пользователю сохранить его
         *
         * Возвращает true, если пользователь хочет (Да) или не хочет (Нет) сохранять
         * и false, если пользователь передумал (Отмена)
         */
        bool saveIfNeeded();

        /**
         * @brief Настроить текущий проект для редактирования
         */
        void goToEditCurrentProject(const QString& _importFilePath = QString());

        /**
         * @brief Закрыть текущий проект
         */
        void closeCurrentProject();

        /**
         * @brief Загружен ли проект в данный момент
         */
        bool isProjectLoaded() const;

    private:
        /**
         * @brief Настроить контроллеры
         */
        void initControllers();

        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Сформировать меню
         */
        QMenu* createMenu();

        /**
         * @brief Настроить соединения
         */
        void initConnections();

        /**
         * @brief Настроить внешний вид
         */
        void initStyleSheet();

        /**
         * @brief Перезагрузить настройки приложения
         */
        void reloadApplicationSettings();

        /**
         * @brief Обновить заголовок окна
         */
        void updateWindowTitle();

    private:
        /**
         * @brief Главное окно приложения
         */
        UserInterface::ApplicationView* m_view = nullptr;

        /**
         * @brief Кнопка вызова меню приложения
         */
        FlatButton* m_menu = nullptr;

#ifdef Q_OS_MAC
        /**
         * @brief Меню в Mac'е наверху
         */
        QMenuBar* m_menuBar = nullptr;

        /**
         * @brief Пункт меню "Правка", зависит от открытой в данный момент вкладки
         */
        QMenu* m_editMenu = nullptr;
#endif

        /**
         * @brief Заглушка для верха правой панели
         */
        QLabel* m_menuSecondary = nullptr;

        /**
         * @brief Панель вкладок
         */
        SideTabBar* m_tabs = nullptr;

        /**
         * @brief Дополнительная панель вкладок
         */
        SideTabBar* m_tabsSecondary = nullptr;

        /**
         * @brief Виджеты вкладок
         */
        QStackedWidget* m_tabsWidgets = nullptr;

        /**
         * @brief Дополнительная панель для виджетов вкладок
         */
        QStackedWidget* m_tabsWidgetsSecondary = nullptr;

        /**
         * @brief Разделитель основных и дополнительных элементов управления
         */
        QSplitter* m_splitter = nullptr;

        /**
         * @brief Управляющий проектами
         */
        ProjectsManager* m_projectsManager = nullptr;

        /**
         * @brief Управляющий меню
         */
        MenuManager* m_menuManager = nullptr;

        /**
         * @brief Управляющий стартовой страницей
         */
        StartUpManager* m_startUpManager = nullptr;

        /**
         * @brief Управляющий страницей разработки
         */
        ResearchManager* m_researchManager = nullptr;

        /**
         * @brief Управляющий сценарием
         */
        ScenarioManager* m_scenarioManager = nullptr;

        /**
         * @brief Управляющий статистикой
         */
        StatisticsManager* m_statisticsManager = nullptr;

        /**
         * @brief Управляющий инструментами
         */
        ToolsManager* m_toolsManager = nullptr;

        /**
         * @brief Управляющий настройками
         */
        SettingsManager* m_settingsManager = nullptr;

        /**
         * @brief Управляющий импортом
         */
        ImportManager* m_importManager = nullptr;

        /**
         * @brief Управляющий экспортом
         */
        ExportManager* m_exportManager = nullptr;

        /**
         * @brief Управляющий синхронизацией
         */
        SynchronizationManager* m_synchronizationManager = nullptr;

        /**
         * @brief Таймер автосохранения
         */
        QTimer m_autosaveTimer;

        /**
         * @brief Помощник резервного копирования
         */
        BackupHelper m_backupHelper;

        /**
         * @brief Состояние приложения в данный момент
         */
        enum class ApplicationState {
            /**
             * @brief Инициализация после запуска
             */
            Initializing,
            /**
             * @brief Загрузка проекта
             */
            ProjectLoading,
            /**
             * @brief Рабочее состояние
             */
            Working,
            /**
             * @brief Импортирование
             */
            Importing
        } m_state = ApplicationState::Initializing;
    };
}

#endif // APPLICATIONMANAGER_H
