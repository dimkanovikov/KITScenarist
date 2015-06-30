#ifndef SCENARIOREVIEWVIEW_H
#define SCENARIOREVIEWVIEW_H

#include <3rd_party/Widgets/QWidgetListView/qtmodelwidget.h>

#include <QTextLayout>

class ColoredToolButton;
class QLabel;
class QStandardItemModel;
class QTextDocument;
class QVBoxLayout;
class QWidgetListView;

namespace UserInterface {

	class ScenarioTextEdit;


	/**
	 * @brief Класс виджета редакторской заметки
	 */
	class ReviewMarkWidget : public QWidget
	{
		Q_OBJECT

	public:
		explicit ReviewMarkWidget(QWidget* _parent = 0);

		/**
		 * @brief Установить редакторское замечание в виджет
		 */
		void setReviewMark(const QColor& _color, const QString& _author, const QString& _date,
			const QString& _comment);

		/**
		 * @brief Выделить/снять выделение
		 */
		void setSelected(bool _selected);

		/**
		 * @brief Получить текст комментария
		 */
		QString comment() const;

	protected:
		/**
		 * @brief Переопределяем для настройки внешнего вида
		 */
		void paintEvent(QPaintEvent* _event);

	private:
		/**
		 * @brief настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

	private:
		/**
		 * @brief Цвет заметки, если цвет невалидный, это комментарий к заметке
		 */
		QLabel* m_color;

		/**
		 * @brief Автор заметки
		 */
		QLabel* m_author;

		/**
		 * @brief Дата заметки
		 */
		QLabel* m_date;

		/**
		 * @brief Текст заметки
		 */
		QLabel* m_comment;

		/**
		 * @brief Выделен
		 */
		bool m_isSelected;
	};


	/**
	 * @brief Класс виджета группы редакторских заметок
	 */
	class ScenarioReviewWidget : public QtModelWidget
	{
		Q_OBJECT

	public:
		Q_INVOKABLE
		explicit ScenarioReviewWidget(QAbstractItemModel* _model, const QModelIndex& _index);

		/**
		 * @brief Реализуем логику изменения данных виджетом
		 */
		void dataChanged();

	signals:
		/**
		 * @brief Изменился текст документа
		 */
		void textChanged();

	private slots:
		/**
		 * @brief Обработать запрос на отображение контекстного меню
		 */
		void aboutContextMenuRequested(const QPoint& _pos);

		/**
		 * @brief Изменить текст
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

		/**
		 * @brief Настроить отображение
		 */
		void initStylesheet();

	private:
		/**
		 * @brief Список из заметки и ответов на неё
		 */
		QList<ReviewMarkWidget*> m_reviewMarks;

		/**
		 * @brief Компоновщик дочерних виджетов
		 */
		QVBoxLayout* m_layout;
	};


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
		void initStylesheet();

	private:
		/**
		 * @brief Представление комментариев
		 */
		QWidgetListView* m_view;

		/**
		 * @brief Редактор сценария
		 */
		ScenarioTextEdit* m_editor;
	};
}

#endif // SCENARIOREVIEWVIEW_H
