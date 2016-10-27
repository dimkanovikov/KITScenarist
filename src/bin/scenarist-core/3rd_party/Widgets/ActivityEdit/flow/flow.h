#ifndef FLOW_H
#define FLOW_H
#include <QObject>
#include <QGraphicsItem>
#include "../shape/shape.h"
#include "../anchor/sizeanchor.h"

/**
 * @class Flow
 * Представляет собой связь (базовый класс) с возможностью создания узлов
 * и некоторым примитивным контролем семантики (соединений).
 */
class Flow : public Shape
{
	Q_OBJECT
public:
	Flow (Shape *start, Shape *end, QGraphicsItem *parent = 0);
	~Flow();

	virtual QPointF center() const;	///< Центральная точка, куда крепятся связи

	/**
	 * Проверяет, можно ли подключить фигуру в качестве стартовой.
	 * Это необходимо при работе с висячими узлами (?): когда узел бросают
	 * на фигуру, нужно контролировать корректность получающегося соединения.
	 * Например, пунктирными связями нельзя соединить два комментария, или
	 * две фигуры другого типа. Одна фигура обязательно должна быть комментарием.
	 * Это и проверяет данная функция.
	 * @param s Фигура, которую планируется использовать в качестве начальной.
	 * @return true, если можно начальную фигуру заменить на s.
	 */
	virtual bool canConnectStartTo (Shape *s) = 0;

	/**
	 * Проверяет, можно ли подключить фигуру в качестве конечной.
	 * Это необходимо при работе с висячими узлами (?): когда узел бросают
	 * на фигуру, нужно контролировать корректность получающегося соединения.
	 * Например, пунктирными связями нельзя соединить два комментария, или
	 * две фигуры другого типа. Одна фигура обязательно должна быть комментарием.
	 * Это и проверяет данная функция.
	 * @param s Фигура, которую планируется использовать в качестве конечной.
	 * @return true, если можно конечную фигуру заменить на s.
	 */
	virtual bool canConnectEndTo (Shape *s) = 0;

	Shape* startShape() const;
	void setStartShape (Shape *s);
	Shape* endShape() const;
	void setEndShape (Shape *s);
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual QPainterPath basicShape() const;

	/// Находит точку на границе, к которой крепится связь
	virtual QPointF flowConnectionPoint (const QPointF &anotherEnd, Flow *flow) const;

	/// Многоугольник, содержащий все вершины связи
	QPolygonF createFlowPolygon() const;

	/// Задать промежуточные узлы связи
	void setFlowKnots (const QList<QPointF> &m_knots);

	/// Удалить все промежуточные узлы связи
	void removeAllFlowKnots();

protected:
	void selectAnchors (bool select);	///< Выделить маркеры перемещения
	virtual void connectSignals();		///< Отключить сигналы от фигуры
	virtual void disconnectSignals();	///< Подключить сигналы к фигуре
	virtual void updateFlow();			///< Перерисовать связь
	virtual void updateAnchors();		///< Обновить положения и видимость маркеров
	virtual QVariant itemChange (GraphicsItemChange change, const QVariant &value);
	virtual void paintSelection (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);	///< Нарисовать выделение
	virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual void mousePressEvent (QGraphicsSceneMouseEvent *event);
	virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);
	virtual void mouseMoveEvent (QGraphicsSceneMouseEvent *event);
protected:
	QList<QPointF> m_knots;			///< Список узлов
	QList<SizeAnchor *> m_anchors;	///< Список маркеров, связанных с соответствующими узлами
	QPointF m_dragstart;				///< Точка, в которой началось вытягивание узла
	QPointF m_startPoint, m_endPoint;		///< Начальная и конечная точки присоединения к фигурам
	int m_anchorToMove;			///< Новый маркер (который приходится перемещать вручную)
public slots:
	void anchorMoved();				///< Срабатывает при перемещении маркера
	void shapesChanged();			///< Срабатывает при изменении (перемещении, ресайзе) фигур
protected slots:
	void removeNeighborKnots();		///< Удаляет слишком близко расположенные соседние узлы
private:
	Shape *m_startShape, *m_endShape;			///< Указатели на фигуры
};

#endif // FLOW_H
