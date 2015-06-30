#ifndef SCENARIOREVIEWMODEL_H
#define SCENARIOREVIEWMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include <QColor>

namespace BusinessLogic {
	class ScenarioTextDocument;


	/**
	 * @brief Модель рецензирования
	 */
	class ScenarioReviewModel : public QAbstractListModel
	{
		Q_OBJECT

	public:
		/**
		 * @brief Дополнительные роли для модели
		 */
		enum ReviewModelRoles {
			IsDoneRole = Qt::UserRole + 1,
			CommentsRole,
			CommentsAuthorsRole,
			CommentsDatesRole
		};

	public:
		explicit ScenarioReviewModel(ScenarioTextDocument* _parent);

		/**
		 * @brief Реализация стандартных методов
		 */
		/** @{ */
		int rowCount(const QModelIndex& _parent = QModelIndex()) const;
		QVariant data(const QModelIndex& _index, int _role) const;
		bool removeRows(int _row, int _count, const QModelIndex& _parent = QModelIndex());
		/** @} */

		/**
		 * @brief Установить цвет текста для заметки
		 */
		void setReviewMarkTextColor(int _startPosition, int _length, const QColor& _color);

		/**
		 * @brief Установить цвет фона для заметки
		 */
		void setReviewMarkTextBgColor(int _startPosition, int _length, const QColor& _color);

		/**
		 * @brief Установить выделение для заметки
		 */
		void setReviewMarkTextHighlight(int _startPosition, int _length, const QColor& _color);

		/**
		 * @brief Установить комментарий для заметки
		 */
		/** @{ */
		void setReviewMarkComment(int _startPosition, int _length, const QString& _comment);
		void setReviewMarkComment(const QModelIndex& _index, const QString& _comment);
		/** @} */

		/**
		 * @brief Добавить комментарий к существующей редакторской заметке
		 */
		void addReviewMarkComment(const QModelIndex& _index, const QString& _comment);

		/**
		 * @brief Обновить комментарий
		 */
		void updateReviewMarkComment(const QModelIndex& _index, int _commentIndex, const QString& _comment);

		/**
		 * @brief Установить флаг "выполнено" для заметки
		 */
		/** @{ */
		void setReviewMarkIsDone(int _cursorPosition, bool _isDone);
		void setReviewMarkIsDone(const QModelIndex& _index, bool _isDone);
		/** @} */

		/**
		 * @brief Удалить заметку
		 */
		/** @{ */
		void removeMark(int _cursorPosition);
		void removeMark(const QModelIndex& _index, int _commentIndex = 0);
		/** @{ */

		/**
		 * @brief Получить начальную позицию выделения
		 */
		int markStartPosition(const QModelIndex& _index) const;

		/**
		 * @brief Получить длинну выделения
		 */
		int markLength(const QModelIndex& _index) const;

		/**
		 * @brief Индекс элемента по позиции
		 */
		QModelIndex indexForPosition(int _position);

	signals:
		/**
		 * @brief В документ были внесены редакторские примечания
		 */
		void reviewChanged();

	private slots:
		/**
		 * @brief Сформировать модель комментариев
		 */
		void aboutUpdateReviewModel(int _position, int _removed, int _added);

	private:
		/**
		 * @brief Документ, по которому строится модель
		 */
		ScenarioTextDocument* m_document;

		/**
		 * @brief Класс информации о редакторской метке
		 */
		class ReviewMarkInfo {
		public:
			ReviewMarkInfo() : startPosition(0), length(0), isDone(false) {}

			/**
			 * @brief Позиция начала
			 */
			int startPosition;

			/**
			 * @brief Длина выделения
			 */
			int length;

			/**
			 * @brief Позиция конца
			 */
			int endPosition() const;

			/**
			 * @brief Цвет выделения
			 */
			/** @{ */
			QColor foreground;
			QColor background;
			/** @} */

			/**
			 * @brief Проработан
			 */
			bool isDone;

			/**
			 * @brief Тексты комментариев
			 */
			QStringList comments;

			/**
			 * @brief Авторы заметок
			 */
			QStringList authors;

			/**
			 * @brief Даты заметок
			 */
			QStringList dates;
		};

		/**
		 * @brief Редакторские заметки
		 */
		QList<ReviewMarkInfo> m_reviewMarks;

		/**
		 * @brief Карта заметок, для быстрого поиска по позиции (начальная/конечная позиция, номер заметки)
		 */
		QMap<int, int> m_reviewMap;
	};
}

#endif // SCENARIOREVIEWMODEL_H
