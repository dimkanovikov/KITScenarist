#ifndef SCENARIOMANAGER_H
#define SCENARIOMANAGER_H

#include <QObject>

class QComboBox;
class QLabel;

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

	public slots:
		/**
		 * @brief Изменено имя персонажа
		 */
		void aboutCharacterNameChanged(const QString& _oldName, const QString& _newName);

		/**
		 * @brief Пересоздать персонажей
		 */
		void refreshCharacters();

		/**
		 * @brief Изменено название локации
		 */
		void aboutLocationNameChanged(const QString& _oldName, const QString& _newName);

		/**
		 * @brief Пересоздать локации
		 */
		void refreshLocations();

	private slots:
		/**
		 * @brief Обновить хронометраж
		 */
		void aboutUpdateDuration(int _cursorPosition);

	private:
		void initData();

		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

	private:
		/**
		 * @brief Представление сценария
		 */
		QWidget* m_view;

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
