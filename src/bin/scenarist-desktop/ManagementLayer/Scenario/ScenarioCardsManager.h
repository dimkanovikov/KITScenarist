#ifndef SCENARIOCARDSMANAGER_H
#define SCENARIOCARDSMANAGER_H

#include <QObject>

namespace Domain {
	class Scenario;
}

namespace BusinessLogic {
    class ScenarioModel;
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
         * @brief Сохранить схему сценария
         */
        QString save() const;

        /**
         * @brief Загрузить заданную схему
         */
        void load(const QString& _xml);

        /**
         * @brief Установить модель документа сценария
         */
        void setModel(BusinessLogic::ScenarioModel* _model);

        /**
         * @brief Очистить данные схемы и модель
         */
        void clear();

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

        /**
         * @brief Модель сценария
         */
        BusinessLogic::ScenarioModel* m_model;
	};
}

#endif // SCENARIOCARDSMANAGER_H
