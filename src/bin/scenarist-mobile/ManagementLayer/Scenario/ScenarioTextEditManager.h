#ifndef SCENARIOTEXTEDITMANAGER_H
#define SCENARIOTEXTEDITMANAGER_H

#include <QObject>


namespace UserInterface {
	class ScenarioTextView;
}

namespace BusinessLogic {
	class ScenarioTextDocument;
}

namespace ManagementLayer
{
	/**
	 * @brief Управляющий редактированием сценария
	 */
	class ScenarioTextEditManager : public QObject
	{
		Q_OBJECT

	public:
		explicit ScenarioTextEditManager(QObject* _parent, QWidget* _parentWidget);

		/**
		 * @brief Панель инструментов представления
		 */
		QWidget* toolbar() const;

		/**
		 * @brief Представление
		 */
		QWidget* view() const;

		/**
		 * @brief Задать название сценария
		 */
		void setScenarioName(const QString& _name);

		/**
		 * @brief Установить документ для редактирования
		 */
		void setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document, bool _isDraft = false);

//		/**
//		 * @brief Установить хронометраж
//		 */
//		void setDuration(const QString& _duration);

//		/**
//		 * @brief Установить значения счётчиков
//		 */
//		void setCountersInfo(const QString& _counters);

		/**
		 * @brief Установить позицию курсора
		 */
		void setCursorPosition(int _position);

		/**
		 * @brief Перезагрузить параметры текстового редактора
		 */
		void reloadTextEditSettings();

		/**
		 * @brief Получить текущую позицию курсора
		 */
		int cursorPosition() const;

		/**
		 * @brief Установить список дополнительных курсоров для отрисовки
		 */
		void setAdditionalCursors(const QMap<QString, int>& _cursors);

		/**
		 * @brief Установить режим работы со сценарием
		 */
		void setCommentOnly(bool _isCommentOnly);

	public slots:
		/**
		 * @brief Добавить элемент сценария в указанной позиции
		 */
		void addScenarioItem(int _position, int _type, const QString& _header,
			const QColor& _color, const QString& _description);

		/**
		 * @brief Удалить заданный текст сценария
		 */
		void removeScenarioText(int _from, int _to);

		/**
		 * @brief Отменить последнее действие
		 */
		void aboutUndo();

		/**
		 * @brief Повторить последнее действие
		 */
		void aboutRedo();

	signals:
		/**
		 * @brief Запрос на отображение навигатора
		 */
		void showNavigatorRequested();

		/**
		 * @brief Изменился текст сценария
		 */
		void textChanged();

		/**
		 * @brief Изменилась позиция курсора
		 */
		void cursorPositionChanged(int _position);

		/**
		 * @brief Запрос отмены действия
		 */
		void undoPressed();

		/**
		 * @brief Запрос повтора действия
		 */
		void redoPressed();

	private slots:
		/**
		 * @brief Реакция на изменение коэффициента масштабирования редактора сценария
		 */
		void aboutTextEditZoomRangeChanged(qreal _zoomRange);

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
		 * @brief Редактор
		 */
		UserInterface::ScenarioTextView* m_view;
	};
}

#endif // SCENARIOTEXTEDITMANAGER_H
