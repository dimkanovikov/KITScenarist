#ifndef SCALABLEWRAPPER_H
#define SCALABLEWRAPPER_H

#include <QGraphicsView>

class QGestureEvent;
class QTextEdit;


/**
 * @brief Обёртка над редактором текста, чтобы сделать его масштабируемым
 */
class ScalableWrapper : public QGraphicsView
{
	Q_OBJECT

public:
	explicit ScalableWrapper(QTextEdit* _editor, QWidget* _parent = 0);

	/**
	 * @brief Получить редактор, вокруг которого построена обёртка
	 */
	QTextEdit* editor() const;

	/**
	 * @brief Установить коэффициент масштабирование
	 */
	void setZoomRange(qreal _zoomRange);

signals:
	/**
	 * @brief Изменился коэффициент масштабирования
	 */
	void zoomRangeChanged(qreal) const;

protected:
	/**
	 * @brief Переопределяем для обработки жестов
	 */
	bool event(QEvent* _event);

	/**
	 * @brief Переопределяется для того, чтобы скорректировать размер встроеного редактора
	 */
	void paintEvent(QPaintEvent* _event);

	/**
	 * @brief Переопределяется для реализации увеличения/уменьшения
	 */
	void wheelEvent(QWheelEvent* _event);

	/**
	 * @brief Обрабатываем жест увеличения масштаба
	 */
	void gestureEvent(QGestureEvent* _event);

	/**
	 * @brief Переопределяется для отлавливания контекстного меню текстового редактора
	 */
	bool eventFilter(QObject* _object, QEvent* _event);

private:
	/**
	 * @brief Включить/выключить синхронизацию полос прокрутки между редактором и представлением
	 */
	void setupScrollingSynchronization(bool _needSync);

	/**
	 * @brief Пересчитать размер редактора текста
	 */
	void updateTextEditSize();

	/**
	 * @brief Собственно масштабирование представления текстового редактора
	 */
	void scaleTextEdit();

private:
	/**
	 * @brief Сцена в которой будет позиционироваться редактор
	 */
	QGraphicsScene* m_scene;

	/**
	 * @brief Указатель на сам редактор
	 */
	QTextEdit* m_editor;

	/**
	 * @brief Графическое представление редактора
	 */
	QGraphicsProxyWidget* m_editorProxy;

	/**
	 * @brief Коэффициент масштабирования
	 */
	qreal m_zoomRange;

	/**
	 * @brief Инерционный тормоз масштабирования при помощи жестов
	 */
	int m_gestureZoomInertionBreak;

	/**
	 * @brief Вспомогательный элемент, посредством которого настраивается размер полос прокрутки
	 */
	QGraphicsRectItem* m_rect;
};

#endif // SCALABLEWRAPPER_H
