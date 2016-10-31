#include "sizeanchor.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QCursor>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

bool SizeAnchor::isBeingMovedByUser() const
{
	return !isSelected();
}

SizeAnchor::SizeAnchor (QGraphicsItem *parent)
	: QObject()
	, QGraphicsItem(parent)
{
	setFlag(ItemIsMovable);
	setFlag(ItemIsSelectable);
	setFlag(ItemSendsGeometryChanges);
	setCursor(Qt::SizeFDiagCursor);
	setVisible(true);
	setZValue(1002);
}

SizeAnchor::~SizeAnchor()
{
}

QVariant SizeAnchor::itemChange (GraphicsItemChange change, const QVariant &value)
{
	if (change==ItemPositionHasChanged)
		emit moving();
	return QGraphicsItem::itemChange(change, value);
}

QRectF SizeAnchor::boundingRect() const
{
	return QRectF(-SIZE/2,-SIZE/2,SIZE,SIZE);
}

void SizeAnchor::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setBrush(Qt::darkGray);
	painter->setPen(Qt::green);
	painter->drawRect(QRectF(-VISIBLE_SIZE/2, -VISIBLE_SIZE/2, VISIBLE_SIZE, VISIBLE_SIZE));
}

QList<bool> SizeAnchor::fixSelectedItems (bool fix, const QList<bool> &list)
{
	// (!) Подразумевается, что между парными вызовами этой функции
	//     элементы на диаграмму не добавляются
	QList<bool> prev;
	QList<QGraphicsItem *> sel = scene()->selectedItems();
	for(int i=0; i<sel.count(); ++i)
		prev << (sel[i]->flags() & ItemIsMovable),
		sel[i]->setFlag(ItemIsMovable, (fix? false : list.at(i)) );
	return prev;
}

void SizeAnchor::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	setSelected(false);
	setFlag(ItemIsSelectable, false);
	templist = fixSelectedItems(true);
	QGraphicsItem::mousePressEvent(event);
	emit startMoving();
}

void SizeAnchor::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	fixSelectedItems(false, templist);
	QGraphicsItem::mouseReleaseEvent(event);
	setFlag(ItemIsSelectable, true);
	emit stopMoving();
}

void SizeAnchor::mouseMoveEvent (QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseMoveEvent(event);
	if (event->buttons() & Qt::LeftButton)
		emit movingByUser();
}
