#ifndef SHAPE_H
#define SHAPE_H

#include <QGraphicsItem>
#include <QObject>
#include <QVector>
#include <QTextOption>
#include <QDomElement>

class SizeAnchor;
class Flow;


/**
 * @class Shape
 * Этот класс - общий предок для всех элементов диаграммы, в том числе для связей.
 * Он отвечает за присоединение связей, за положение фигуры, за рисование, и т.п.
 * Еще он позволяет копировать фигуры.
 * Все видимые элементы диаграммы (исключая маркеры) принадлежат к этому типу.
 */
class Shape : public QObject, public virtual QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)
public:
	static QBrush innerBrush();
	static QFont basicFont();
	static QFontMetrics basicFontMetrics();
	Shape (QGraphicsItem *parent = 0);
	virtual ~Shape();

	/**
	 * Настроить свойства фигуры (например, показать диалог).
	 */
	virtual void editProperties();

	virtual QPointF	flowConnectionPoint (const QPointF &anotherEnd, Flow *flow) const;
	virtual QPointF center() const;
	void emitStateIsAboutToBeChangedByUser();	///< Отправка уведомления об изменении состояния
protected:
	virtual void initialize();
	virtual QVariant itemChange (GraphicsItemChange change, const QVariant &value);
	virtual void setPenAndBrushForSelection (QPainter *painter);
	virtual void mousePressEvent (QGraphicsSceneMouseEvent *event);
	virtual void mouseMoveEvent (QGraphicsSceneMouseEvent *event);
	virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);
	bool waspressed;
signals:
	/**
	 * Испускается перед изменением состояния фигуры.
	 * Нужно сцене, фактически, чтобы контролировать отмену.
	 */
	void stateIsAboutToBeChangedByUser();

	/** Испускается, когда пользователь начинает перемещать фигуру. */
	void startMovingByUser();

	/** Испускается при изменении положения путем перетаскивания мышью. */
	void movingByUser();

	/** Испускается, когда пользователь отпускает фигуру */
	void stopMovingByUser();

	/** Испускается, когда изменяется содержимое фигуры (например, текст). */
	void contentsChanged();

	/** Испускается, когда изменяется положение фигуры (размер, координаты, и т.п.) */
	void placementChanged();

	/** Испускается, когда фигура выделяется. */
	void selected();

	/** Испускается, когда фигура перемещается (любым образом). */
	void moving();
};


#endif // SHAPE_H
