#ifndef SCENARIOCARDSMANAGER_H
#define SCENARIOCARDSMANAGER_H

#include <QObject>

namespace Domain {
	class Scenario;
}

namespace UserInterface {
	class ScenarioCardsView;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий карточками сценария
	 */
	class ScenarioCardsManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ScenarioCardsManager(QObject* _parent, QWidget* _parentWidget);

		QWidget* view() const;

		/**
		 * @brief Загрузить данные текущего проекта
		 */
		void loadCurrentProject();

		/**
		 * @brief Загрузить настройки текущего проекта
		 */
		void loadCurrentProjectSettings(const QString& _projectPath);

		/**
		 * @brief Сохранить данные текущего проекта
		 */
		void saveCurrentProject();

		/**
		 * @brief Сохранить настройки текущего проекта
		 */
		void saveCurrentProjectSettings(const QString& _projectPath);

		/**
		 * @brief Закрыть текущий проект
		 */
		void closeCurrentProject();

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	signals:
		/**
		 * @brief Запрос на формирование черновой схемы по тексту сценария
		 */
		void needDirtyScheme();

		/**
		 * @brief Схема карточек изменена
		 */
		void schemeChanged();

	private:
		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

	private:
		/**
		 * @brief Представление редактора карт
		 */
		UserInterface::ScenarioCardsView* m_view;

		/**
		 * @brief Сценарий
		 */
		Domain::Scenario* m_scenario;
	};
}

#endif // SCENARIOCARDSMANAGER_H
