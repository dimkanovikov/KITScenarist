#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

#include "graphlogic.h"

class MainWindow;
class GraphLogic;
class QGestureEvent;

/** Responsibilities:
  * - Handle scene zoom in/out events
  * - Close scene (clean), new scene (clean & add first node)
  * - Pass key events to GraphLogic
  */
class GraphWidget : public QGraphicsView
{
	Q_OBJECT

public:

	GraphWidget(QWidget* parent = 0);

	void newScene();
	void closeScene();

	void load(const QString& _xml);
	QString save() const;

	GraphLogic *graphLogic() const;

public slots:
	void zoomIn();
	void zoomOut();

signals:

	void contentChanged();
	void notification(const QString &msg);

protected:
	/**
	 * @brief Переопределяем для обработки жестов
	 */
	bool event(QEvent* _event);

	/**
	 * @brief Обрабатываем жест увеличения масштаба
	 */
	void gestureEvent(QGestureEvent* _event);

	void keyPressEvent(QKeyEvent* _event);
    void keyReleaseEvent(QKeyEvent* _event);

    void mouseMoveEvent(QMouseEvent* _event);

	void wheelEvent(QWheelEvent* _event);
	void drawBackground(QPainter *painter, const QRectF &rect);

private:
	void scaleView(qreal factor);

private:
	QGraphicsScene *m_scene;
	GraphLogic *m_graphlogic;

	/**
	 * @brief Инерционный тормоз масштабирования при помощи жестов
	 */
	int m_gestureZoomInertionBreak;

    /**
     * @brief Последняя позиция мыши в момент скроллинга полотна
     */
    QPoint m_scrollingLastPos;

    /**
     * @brief Происходит ли в данный момент скроллинг с зажатым пробелом
     */
    bool m_inScrolling;
};

#endif // GRAPHWIDGET_H
