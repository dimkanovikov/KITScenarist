#ifndef SCENARIOCARDSVIEW_H
#define SCENARIOCARDSVIEW_H

#include <QWidget>

class ActivityEdit;
class FlatButton;
class QLabel;


namespace UserInterface {
	/**
	 * @brief Представление редактора карт
	 */
	class ScenarioCardsView : public QWidget
	{
		Q_OBJECT

	public:
		explicit ScenarioCardsView(QWidget* _parent = 0);

		/**
		 * @brief Очистить схему
		 */
		void clear();

		/**
		 * @brief Загрузить схему из xml-строки
		 */
		void load(const QString& _xml);

		/**
		 * @brief Получить xml-строку текущей схемы
		 */
		QString save() const;

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	signals:
		/**
		 * @brief Схема карточек изменена
		 */
		void schemeChanged();

	private:
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
		 * @brief Редактор карточек
		 */
		ActivityEdit* m_cardsEdit;

		/**
		 * @brief Кнопка добавления карточки
		 */
		FlatButton* m_addCard;

		/**
		 * @brief Кнопка добавления заметки
		 */
		FlatButton* m_addNote;

		/**
		 * @brief Кнопка добавления горизонтальной линии
		 */
		FlatButton* m_addHLine;

		/**
		 * @brief Кнопка добавления вертикальной линии
		 */
		FlatButton* m_addVLine;

		/**
		 * @brief Кнопка упорядочивания по таблице
		 */
		FlatButton* m_sort;

		/**
		 * @brief Кнопка отправки элемента в черновик
		 */
		FlatButton* m_moveToDraft;

		/**
		 * @brief Кнопка отправки элемента в сценарий
		 */
		FlatButton* m_moveToScript;

		/**
		 * @brief Метка, для закрашивания пространства в панели инструментов
		 */
		QLabel* m_toolbarSpacer;
	};
}

#endif // SCENARIOCARDSVIEW_H
