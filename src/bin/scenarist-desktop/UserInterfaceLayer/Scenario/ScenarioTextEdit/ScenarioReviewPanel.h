#ifndef SCENARIOREVIEWPANEL_H
#define SCENARIOREVIEWPANEL_H

#include <QFrame>

namespace BusinessLogic {
	class ScenarioReviewModel;
}

class ColoredToolButton;
class FlatButton;
class QAbstractItemModel;

namespace UserInterface
{
	class ScenarioTextEdit;


	/**
	 * @brief Класс панели рецензирования
	 */
	class ScenarioReviewPanel : public QFrame
	{
		Q_OBJECT

	public:
		explicit ScenarioReviewPanel(ScenarioTextEdit* _editor, QWidget* _parent = 0);

	signals:
		/**
		 * @brief Включение/выключение режима рецензирования
		 */
		void toggled(bool);

	private slots:
		/**
		 * @brief Обновить доступность действий при смене курсора
		 */
		void aboutUpdateActionsEnable();

		void aboutChangeTextColor(const QColor& _color);
		void aboutChangeTextBgColor(const QColor& _color);
		void aboutChangeTextHighlight(const QColor& _color);
		void aboutAddComment(const QColor& _color);
		void doneReview(bool _done);
		void clearReview();

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

		/**
		 * @brief Получить указатель на модель редакторских заметок
		 */
		BusinessLogic::ScenarioReviewModel* reviewModel() const;

	private:
		/**
		 * @brief Редактор текста, к которому привязана панель
		 */
		ScenarioTextEdit* m_editor;

		/**
		 * @brief Кнопка активации панели
		 */
		FlatButton* m_activateButton;

		/**
		 * @brief Цвет текста
		 */
		ColoredToolButton* m_textColor;

		/**
		 * @brief Цвет фона
		 */
		ColoredToolButton* m_textBgColor;

		/**
		 * @brief Выделение текста (а-ля вордовский маркер)
		 */
		ColoredToolButton* m_textHighlight;

		/**
		 * @brief Комментарий
		 */
		ColoredToolButton* m_comment;

		/**
		 * @brief Решено
		 */
		FlatButton* m_done;

		/**
		 * @brief Очистить форматирование
		 */
		FlatButton* m_clear;
	};
}

#endif // SCENARIOREVIEWPANEL_H

