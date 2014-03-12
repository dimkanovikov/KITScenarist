#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <QObject>

class SideTabBar;
class QStackedLayout;
class QToolButton;
class QMenu;

namespace ManagementLayer
{
	class StartUpManager;
	class ScenarioManager;
	class CharactersManager;
	class LocationsManager;
	class SettingsManager;


	/**
	 * @brief Управляющий приложением
	 */
	class ApplicationManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ApplicationManager(QObject *parent = 0);
		~ApplicationManager();

		/**
		 * @brief Запуск приложения
		 */
		void exec();

	private slots:
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
		 * @brief Экспортировать в PDF
		 */
		void aboutExportToPdf();

		/**
		 * @brief Экспортировать в RTF
		 */
		void aboutExportToRtf();

	private:
		/**
		 * @brief Если проект был изменён, но не сохранён предложить пользователю сохранить его
		 */
		void saveIfNeeded();

		/**
		 * @brief Сохранить текущий проект в недавно используемых
		 */
		void saveCurrentProjectInRecent();

		/**
		 * @brief Настроить текущий проект для редактирования
		 */
		void goToEditCurrentProject();

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

	private:
		/**
		 * @brief Главное окно приложения
		 */
		QWidget* m_view;

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
		QStackedLayout* m_tabsWidgets;

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
	};
}

#endif // APPLICATIONMANAGER_H
