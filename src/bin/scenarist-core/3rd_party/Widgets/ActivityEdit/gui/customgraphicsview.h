#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QHash>
#include <QGraphicsItem>

class QGestureEvent;


/**
 * @brief Переопределяется для реализации области выделения
 */
class CustomGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	explicit CustomGraphicsView(QWidget* _parent = 0);

	/**
	 * @brief Методы масштабирования
	 */
	/** @{ */
	void zoomIn();
	void zoomOut();
	/** @} */

signals:
	/**
	 * @brief Нажата кнопка Delete или Backspace
	 */
	void deletePressed();

protected:
	/**
	 * @brief Переопределяем для обработки жестов
	 */
	bool event(QEvent* _event);

	/**
	 * @brief Обрабатываем жест увеличения масштаба
	 */
	void gestureEvent(QGestureEvent* _event);

	/**
	 * @brief Обрабатываем изменение масштаба при помощи ролика
	 */
	void wheelEvent(QWheelEvent* _event);

	/**
	 * @brief Переопределяем для обработки горячих клавиш изменения масштаба
	 */
	void keyPressEvent (QKeyEvent* _event);

	/**
	 * @brief Реализация области выделения
	 */
	/** @{ */
	void mousePressEvent (QMouseEvent* _event);
	void mouseMoveEvent (QMouseEvent* _event);
	void mouseReleaseEvent (QMouseEvent* _event);
	/** @} */

private:
	/**
	 * @brief Масштабировать представление на заданный коэффициент
	 */
	void scaleView(qreal _factor);

	/**
	 * @brief Выделить отдельные элементы
	 */
	void selectItemsWithCtrl(QPainterPath& _area);

	/**
	 * @brief Фигура области выделения
	 */
	QGraphicsRectItem* m_rubberRect;

	/**
	 * @brief Позиция начала области выделения
	 */
	QPoint m_startPos;

	/**
	 * @brief Происходит ли в данный момент выделение области
	 */
	bool m_inRubberBanding;

	/**
	 * @brief Список выделенных элементов
	 * @note Не до конца понял, зачем он тут нужен, но пока оставил, чтобы время не тратить
	 */
	QHash<QGraphicsItem*, bool> prevSelState;

	/**
	 * @brief Инерционный тормоз масштабирования при помощи жестов
	 */
	int m_gestureZoomInertionBreak;
};

#endif // CUSTOMGRAPHICSVIEW_H
