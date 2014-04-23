#ifndef SCENARIOMANAGER_H
#define SCENARIOMANAGER_H

#include <QObject>

class QSplitter;
class QComboBox;
class QLabel;
class QTabBar;
class QStackedWidget;

namespace BusinessLogic {
	class ScenarioDocument;
}

namespace ManagementLayer
{
	class ScenarioNavigatorManager;
	class ScenarioTextEditManager;


	/**
	 * @brief Управляющий сценарием
	 */
	class ScenarioManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ScenarioManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		BusinessLogic::ScenarioDocument* scenario() const;

		/**
		 * @brief Загрузить данные текущего проекта
		 */
		void loadCurrentProject();

		/**
		 * @brief Сохранить данные текущего проекта
		 */
		void saveCurrentProject();

		/**
		 * @brief Загрузить состояние окна
		 */
		void loadViewState();

		/**
		 * @brief Сохранить состояние окна
		 */
		void saveViewState();

	public slots:
		/**
		 * @brief Обновить параметры текстового редактора
		 */
		void aboutTextEditSettingsUpdated();

		/**
		 * @brief Обновить параметры навигатора
		 */
		void aboutNavigatorSettingsUpdated();

		/**
		 * @brief Пересчитать хронометраж
		 */
		void aboutChronometrySettingsUpdated();

		/**
		 * @brief Изменено имя персонажа
		 */
		void aboutCharacterNameChanged(const QString& _oldName, const QString& _newName);

		/**
		 * @brief Пересоздать персонажей
		 */
		void aboutRefreshCharacters();

		/**
		 * @brief Изменено название локации
		 */
		void aboutLocationNameChanged(const QString& _oldName, const QString& _newName);

		/**
		 * @brief Пересоздать локации
		 */
		void aboutRefreshLocations();

	private slots:
		/**
		 * @brief Обновить хронометраж
		 */
		void aboutUpdateDuration(int _cursorPosition);

		/**
		 * @brief Выделить текущую сцену в навигаторе
		 */
		void aboutSelectItemInNavigator(int _cursorPosition);

		/**
		 * @brief Сместить курсор к выбранной сцене
		 */
		void aboutMoveCursorToItem(const QModelIndex& _index);

	private:
		/**
		 * @brief Загрузить данные
		 */
		void initData();

		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		/**
		 * @brief Представление сценария
		 */
		QWidget* m_view;

		/**
		 * @brief Разделитель представленя
		 */
		QSplitter* m_viewSplitter;

		/**
		 * @brief Вкладки редакторов
		 */
		QTabBar* m_viewEditorsTabs;

		/**
		 * @brief Панели инструментов редакторов
		 */
		QStackedWidget* m_viewEditorsToolbars;

		/**
		 * @brief Редакторы
		 */
		QStackedWidget* m_viewEditors;

		/**
		 * @brief Документ сценария
		 */
		BusinessLogic::ScenarioDocument* m_scenario;

		/**
		 * @brief Управляющий навигацией по сценарию
		 */
		ScenarioNavigatorManager* m_navigatorManager;

		/**
		 * @brief Управляющий редактированием сценария
		 */
		ScenarioTextEditManager* m_textEditManager;
	};
}

#endif // SCENARIOMANAGER_H
