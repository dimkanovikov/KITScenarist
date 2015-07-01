#ifndef SCENARIOREVIEWVIEW_H
#define SCENARIOREVIEWVIEW_H

#include <QWidget>

class QListView;

namespace UserInterface {

	class ScenarioTextEdit;


	/**
	 * @brief Класс списка редакторских комментариев
	 */
	class ScenarioReviewView : public QWidget
	{
		Q_OBJECT

	public:
		explicit ScenarioReviewView(QWidget* _parent = 0);

		/**
		 * @brief Установить редактор
		 */
		void setEditor(ScenarioTextEdit* _editor);

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
		void aboutEdit(int _commentIndex);

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
		void aboutDelete(int _commentIndex);

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
		 * @brief Представление комментариев
		 */
		QListView* m_view;

		/**
		 * @brief Редактор сценария
		 */
		ScenarioTextEdit* m_editor;
	};
}

#endif // SCENARIOREVIEWVIEW_H
