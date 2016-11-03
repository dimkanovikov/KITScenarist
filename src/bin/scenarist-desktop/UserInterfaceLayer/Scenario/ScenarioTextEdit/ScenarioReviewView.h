#ifndef SCENARIOREVIEWVIEW_H
#define SCENARIOREVIEWVIEW_H

#include <QListView>

namespace UserInterface {

	class ScenarioTextEdit;


	/**
	 * @brief Класс списка редакторских комментариев
	 */
	class ScenarioReviewView : public QListView
	{
		Q_OBJECT

	public:
		explicit ScenarioReviewView(QWidget* _parent = 0);

		/**
		 * @brief Установить редактор
		 */
		void setEditor(ScenarioTextEdit* _editor);

	signals:

		/**
		 * @brief Запрос отмены действия
		 */
		void undoRequest();

		/**
		 * @brief Запрос повтора действия
		 */
		void redoRequest();

	protected:
		/**
		 * @brief Переопределяется для обновления размеров элементов,
		 *		  т.к. стандартная реализация этого не делает
		 */
		void resizeEvent(QResizeEvent* _event);

		/**
		 * @brief Переопределяется чтобы отлавливать нажатия Ctrl+Z и Ctrl+Shift+Z в дереве
		 */
		void keyPressEvent(QKeyEvent* _event);

	private slots:
		/**
		 * @brief Обновить модель комментариев
		 */
		void aboutUpdateModel();

		/**
		 * @brief Прокрутить курсор в редакторе на выбранную заметку
		 */
		void aboutMoveCursorToMark(const QModelIndex& _index);

		/**
		 * @brief Выделить коментарий в зависимости от положения курсора
		 */
		void aboutSelectMark();

		/**
		 * @brief Обработать запрос на отображение контекстного меню
		 */
		void aboutContextMenuRequested(const QPoint& _pos);

		/**
		 * @brief Изменить текст комментария
		 */
		/** @{ */
		void aboutEdit(int _commentIndex);
		void aboutEdit(const QModelIndex& _index);
		/** @} */

		/**
		 * @brief Ответить на комментарий
		 */
		void aboutReply();

		/**
		 * @brief Пометить выполненым
		 */
		void aboutDone(bool _done);

		/**
		 * @brief Удалить
		 */
		/** @{ */
		void aboutDelete(int _commentIndex);
		void aboutDeleteSelected();
		/** @} */

	private:
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
		 * @brief Редактор сценария
		 */
		ScenarioTextEdit* m_editor;
	};
}

#endif // SCENARIOREVIEWVIEW_H
