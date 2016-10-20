#include "shape.h"

#include <QApplication>
#include <QGraphicsScene>
#include "../anchor/sizeanchor.h"
#include <QDebug>
#include <QPainter>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include "../flow/flow.h"

/**
 * Нужно, чтобы уведомить фигуру извне, что ее состояние должно быть сохранено.
 * Например, при вызове диалога свойств происходит изменение текста. Само по себе
 * такое действие ничего не значит: оно происходит, например, при копировании фигуры,
 * при загрузке из файла, и т.п., т.е. не всегда приводит к созданию состояния отмены.
 * С другой стороны, после редактирования свойств в диалоге состояние очевидно подлежит
 * сохранению; значит, должен быть способ явно указать фигуре, чтобы она отправила сигнал
 * сохранения состояния. Метод как раз это и реализует.
 */
void Shape::emitStateIsAboutToBeChangedByUser()
{
	emit stateIsAboutToBeChangedByUser();
}

void Shape::mouseReleaseEvent (QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseReleaseEvent(event);
	waspressed = false;
	emit stopMovingByUser();
}

void Shape::mousePressEvent (QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
	waspressed = true;
}

void Shape::mouseMoveEvent (QGraphicsSceneMouseEvent *event)
{
	if (waspressed)
	{
		emit startMovingByUser();
		waspressed = false;
	}
	emit movingByUser();
	QGraphicsItem::mouseMoveEvent(event);
}

/**
 * @return Стандартный шрифт, используемый по умолчанию на диаграмме.
 */
QFont Shape::basicFont()
{
	return QFont("Verdana", 8, QFont::Normal);
}

/**
 * @brief Метрики стандартного шрифта
 */
QFontMetrics Shape::basicFontMetrics()
{
	return QFontMetrics(basicFont());
}

/**
 * @return Кисть, используемая для заливки элементов диаграммы.
 */
QBrush Shape::innerBrush()
{
	return QColor(255,255,255,255);
}

/**
 * Определяет точку на контуре фигуры для подключения связей к фигуре.
 * @param anotherEnd Противоположный конец связи.
 * @param flow Связь, которую подключаем.
 * @return Точка на контуре фигуры, к которой следует вести связь.
 */
QPointF Shape::flowConnectionPoint (const QPointF &anotherEnd, Flow *flow) const
{
	QPolygonF poly = shape().toFillPolygon(QTransform());
	QPointF p1 = poly.at(0), p2, intersectPoint;
	QLineF line, centerLine(center()+scenePos(), anotherEnd);
	bool found = false;
	for (int i = 1; i<poly.count(); ++i)
	{
		p2 = poly.at(i);
		line = QLineF(p1+scenePos(), p2+scenePos());
		QLineF::IntersectType intersectType = line.intersect(centerLine, &intersectPoint);
		if (intersectType == QLineF::BoundedIntersection)
		{
			found = true;
			break;
		}
		p1 = p2;
	}
	return found? intersectPoint - scenePos() : center();
}

/**
 * Задает кисть и перо для рисования выделения.
 */
void Shape::setPenAndBrushForSelection (QPainter *painter)
{
	painter->setBrush(Qt::transparent);
	QPen pen(QApplication::palette().highlight(), 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
	painter->setPen(pen);
}

Shape::Shape (QGraphicsItem *parent)
	: QGraphicsItem(parent)
{
	initialize();
	connect(this, SIGNAL(moving()), this, SIGNAL(placementChanged()));
}

void Shape::initialize()
{
	waspressed = false;
	setFlag(ItemIsMovable);
	setFlag(ItemIsSelectable);
	setFlag(ItemSendsGeometryChanges);
	setVisible(true);
	connect(this, SIGNAL(startMovingByUser()), this, SIGNAL(stateIsAboutToBeChangedByUser()));
}

Shape::~Shape()
{
	disconnect(this, SIGNAL(moving()), this, SIGNAL(placementChanged()));
}

QVariant Shape::itemChange (GraphicsItemChange change, const QVariant &value)
{
	switch(change)
	{
		case ItemPositionHasChanged:
		{
			QVariant v = QGraphicsItem::itemChange(change, value);
			emit moving();
			return v;
		} break;
		case ItemSelectedChange:
			emit selected();
			break;
		default:
			break;
	}
	return QGraphicsItem::itemChange(change, value);
}

/**
 * Задает центр фигуры. К нему направлены все связи.
 * По умолчанию это центр ограничивающего прямоугольника.
 */
QPointF Shape::center() const
{
	QRectF rect = boundingRect();
	return QPointF(rect.width()/2.0, rect.height()/2.0)+rect.topLeft();
}

