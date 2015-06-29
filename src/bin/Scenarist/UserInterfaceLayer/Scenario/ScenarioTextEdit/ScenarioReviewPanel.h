#ifndef SCENARIOREVIEWPANEL_H
#define SCENARIOREVIEWPANEL_H

#include <QFrame>

namespace BusinessLogic {
	class ScenarioReviewModel;
}

class ColoredToolButton;
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

	public slots:
		/**
		 * @brief Включить/выключить рецензирование
		 */
		void setIsActive(bool _active);

	protected:
		/**
		 * @brief Переопределяем для того, чтобы проксировать события клавиатуры в редактор
		 */
		bool event(QEvent* _event);

	private slots:
		/**
		 * @brief Показать себя, при смене позиции курсора
		 */
		void aboutShow();

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
		/** @{ */
		void initConnections();
		void removeConnections();
		/** @} */

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
		 * @brief Активно ли рецензирование
		 */
		bool m_isActive;

		/**
		 * @brief Редактор текста, к которому привязана панель
		 */
		ScenarioTextEdit* m_editor;

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
		ColoredToolButton* m_done;

		/**
		 * @brief Очистить форматирование
		 */
		ColoredToolButton* m_clear;
	};
}

#endif // SCENARIOREVIEWPANEL_H
