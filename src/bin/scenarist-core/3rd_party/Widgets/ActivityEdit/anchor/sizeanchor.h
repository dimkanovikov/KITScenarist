#ifndef SIZEANCHOR_H
#define SIZEANCHOR_H
#include <QObject>
#include <QGraphicsItem>

/**
 * @class Маркер - вспомогательный элемент для
 *        изменения размеров, перетаскивания узлов и т.п.
 * При его перемещении другие выделенные объекты сцены
 * остаются на своих местах.
 */
class Flow;
class SizeAnchor : public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)
public:
	SizeAnchor (QGraphicsItem *parent = NULL);
	~SizeAnchor();
	bool isBeingMovedByUser() const;
	static int const SIZE = 10;
	static int const VISIBLE_SIZE = 6;
signals:
	void moving();
	void startMoving();
	void stopMoving();
	void movingByUser();
protected:
	virtual void mousePressEvent (QGraphicsSceneMouseEvent *event);
	virtual void mouseMoveEvent (QGraphicsSceneMouseEvent *event);
	virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);
	virtual QList<bool> fixSelectedItems (bool fix = true, const QList<bool> &restore = QList<bool>());
	virtual QVariant itemChange (GraphicsItemChange change, const QVariant &value);
	virtual QRectF boundingRect() const;
	virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	QList<bool> templist;
};

#endif // SIZEANCHOR_H
