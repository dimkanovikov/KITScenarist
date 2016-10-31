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
         * @brief Перезагрузить настройки
         */
        void reloadSettings();

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
		void addCardRequest(const QModelIndex& _afterItemIndex, int _type, const QString& _title,
			const QString& _description);

		/**
		 * @brief Запрос на изменение элемента
		 */
		void editCardRequest(const QModelIndex& _index, int _type, const QString& _title,
			const QString& _description);

		/**
		 * @brief Запрос на удаление элемента
		 */
		void removeCardRequest(const QModelIndex& _index);

        /**
         * @brief Изменились цвета карточки
         */
        void cardColorsChanged(const QModelIndex& _index, const QString& _colors);

	private:
		/**
		 * @brief Добавить элемент после выбранного
		 */
		void addCard();

		/**
		 * @brief Изменить карточку
		 */
		void editCard(const QString& _uuid, int _cardType, const QString& _title, const QString& _description);

		/**
		 * @brief Удалить карточку
		 */
		void removeCard(const QString& _uuid);

		/**
		 * @brief Переместить сцену в соответствии с перемещённой карточкой
		 */
        void moveCard(const QString& _parentUuid, const QString& _previousUuid, const QString& _movedUuid);

        /**
         * @brief Изменить цвета карточки
         */
        void changeCardColors(const QString& _uuid, const QString& _colors);

        /**
         * @brief Добавить заметку
         */
        void addNote();

        /**
         * @brief Изменить заметку
         */
        void editNote(const QString& _text);

        /**
         * @brief Добавить текст связи
         */
        void addFlowText();

        /**
         * @brief Изменить текст связи
         */
        void editFlowText(const QString& _text);

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
