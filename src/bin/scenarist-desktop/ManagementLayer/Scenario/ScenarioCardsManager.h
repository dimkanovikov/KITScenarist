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
	class ScenarioSchemeItemDialog;
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
		void load(BusinessLogic::ScenarioModel* _model, const QString& _xml);

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
		 * @brief Схема карточек изменена
		 */
		void schemeChanged();

		/**
		 * @brief Запрос на добавление элемента
		 */
		void addItemRequest(const QModelIndex& _afterItemIndex, int _type, const QString& _title,
			const QString& _description);

	private:
		/**
		 * @brief Добавить элемент после выбранного
		 */
		void addCard();

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
		 * @brief Диалог добавления элемента
		 */
		UserInterface::ScenarioSchemeItemDialog* m_addItemDialog;

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
