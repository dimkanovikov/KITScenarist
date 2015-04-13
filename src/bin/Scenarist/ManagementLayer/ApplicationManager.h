#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <3rd_party/Helpers/BackupHelper.h>

#include <QObject>
#include <QTimer>

class SideTabBar;
class QStackedWidget;
class QToolButton;
class QMenu;

namespace UserInterface {
	class ApplicationView;
}

namespace ManagementLayer
{
	class ProjectsManager;
	class StartUpManager;
	class ScenarioManager;
	class CharactersManager;
	class LocationsManager;
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

	private slots:
		/**
		 * @brief Обновить списки проектов на стартовой странице
		 */
		void aboutUpdateProjectsList();

		/**
		 * @brief Создать новый
		 */
		void aboutCreateNew();

		/**
		 * @brief Сохранить как...
		 */
		void aboutSaveAs();

		/**
		 * @brief Сохранить в файл
		 */
		void aboutSave();

		/**
		 * @brief Загрузить
		 *
		 * Если имя файла не задано, то будет вызван диалог выбора файла
		 */
		void aboutLoad(const QString& _fileName = QString());

		/**
		 * @brief Загрузить из списка
		 */
		/** @{ */
		void aboutLoadFromRecent(const QModelIndex& _projectIndex);
		void aboutLoadFromRemote(const QModelIndex& _projectIndex);
		/** @} */

		/**
		 * @brief Импортировать документ
		 */
		void aboutImport();

		/**
		 * @brief Экспортировать документ
		 */
		void aboutExportTo();

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
		 * @brief Если проект был изменён, но не сохранён предложить пользователю сохранить его
		 *
		 * Возвращает true, если пользователь хочет (Да) или не хочет (Нет) сохранять
		 * и false, если пользователь передумал (Отмена)
		 */
		bool saveIfNeeded();

		/**
		 * @brief Настроить текущий проект для редактирования
		 */
		void goToEditCurrentProject();

		/**
		 * @brief Закрыть текущий проект
		 */
		void closeCurrentProject();

	private:
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
		UserInterface::ApplicationView* m_view;

		/**
		 * @brief Меню приложения
		 */
		QToolButton* m_menu;

		/**
		 * @brief Панель вкладок
		 */
		SideTabBar* m_tabs;

		/**
		 * @brief Виджеты вкладок
		 */
		QStackedWidget* m_tabsWidgets;

		/**
		 * @brief Управляющий проектами
		 */
		ProjectsManager* m_projectsManager;

		/**
		 * @brief Управляющий стартовой страницей
		 */
		StartUpManager* m_startUpManager;

		/**
		 * @brief Управляющий сценарием
		 */
		ScenarioManager* m_scenarioManager;

		/**
		 * @brief Управляющий персонажами
		 */
		CharactersManager* m_charactersManager;

		/**
		 * @brief Управляющий локациями
		 */
		LocationsManager* m_locationsManager;

		/**
		 * @brief Управляющий настройками
		 */
		SettingsManager* m_settingsManager;

		/**
		 * @brief Управляющий импортом
		 */
		ImportManager* m_importManager;

		/**
		 * @brief Управляющий экспортом
		 */
		ExportManager* m_exportManager;

		/**
		 * @brief Управляющий синхронизацией
		 */
		SynchronizationManager* m_synchronizationManager;

		/**
		 * @brief Таймер автосохранения
		 */
		QTimer m_autosaveTimer;

		/**
		 * @brief Помощник резервного копирования
		 */
		BackupHelper m_backupHelper;
	};
}

#endif // APPLICATIONMANAGER_H
