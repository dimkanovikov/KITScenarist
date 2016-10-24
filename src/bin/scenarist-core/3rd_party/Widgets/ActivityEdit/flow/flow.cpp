#include "flow.h"

#include "../anchor/sizeanchor.h"
#include "../scene/customgraphicsscene.h"

#include <QApplication>
#include <QCursor>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPalette>




Flow::Flow(Shape *start, Shape *end, QGraphicsItem *parent)
	: Shape(parent)
{
	_anchor_to_move = -1;
	setGraphicsEffect(NULL);
	setFlag(ItemSendsGeometryChanges);
	setFlag(ItemIsMovable, false);
	_start = start;
	_end = end;
	if (_start==_end)
	{
		QRectF rect = _start->boundingRect();
		int dx = rect.left()-30;
		int dy = rect.top()-30;
		knots << (_start->scenePos() + QPointF(dx, start->center().y()));
		knots << (_start->scenePos() + QPointF(dx, dy));
		knots << (_start->scenePos() + QPointF(start->center().x(), dy));
		updateAnchors();
	}

	setZValue(1000);
	connectSignals();
	updateFlow();
}

Flow::~Flow()
{
	knots.clear();
	updateAnchors();
}

QPointF Flow::center() const
{
	QPolygonF poly = createFlowPolygon();
	QPointF c = Shape::center(), minp;
	double minlength = -100;
	for(int i=0; i<poly.count()-1; ++i)
	{
		QPointF pts = QPointF((poly[i]+poly[i+1])/2.0);
		if (minlength<0 || ((c-pts).manhattanLength() - minlength)<5)
		{
			minp = pts;
			minlength = (c-pts).manhattanLength();
		}
	}
	return minp;
}

void Flow::setFlowKnots (const QList<QPointF> &knots)
{
	this->knots = knots;
	updateFlow();
}

void Flow::setStartShape (Shape *s)
{
	disconnectSignals();
	_start = s;
	updateFlow();
	connectSignals();
}

void Flow::setEndShape (Shape *s)
{
	disconnectSignals();
	_end = s;
	updateFlow();
	connectSignals();
}

void Flow::connectSignals()
{
	connect(_start, SIGNAL(placementChanged()), this, SLOT(shapesChanged()));
	connect(_end, SIGNAL(placementChanged()), this, SLOT(shapesChanged()));
}

void Flow::disconnectSignals()
{
	disconnect(_start, SIGNAL(placementChanged()), this, SLOT(shapesChanged()));
	disconnect(_end, SIGNAL(placementChanged()), this, SLOT(shapesChanged()));
}

QPointF Flow::flowConnectionPoint (const QPointF &anotherEnd, Flow *flow) const
{
	return center();
}

Shape *Flow::startShape() const
{
	return _start;
}

Shape *Flow::endShape() const
{
	return _end;
}

QPainterPath Flow::basicShape() const
{
	QPainterPath path;
	path.moveTo(_start_pt);
	for(int i=0; i<knots.count(); ++i)
		path.lineTo(knots[i]);
	path.lineTo(_end_pt);
	return path;
}

QPainterPath Flow::shape() const
{
	QPainterPathStroker stroker;
	stroker.setWidth(7);
	stroker.setCapStyle(Qt::RoundCap);
	stroker.setJoinStyle(Qt::RoundJoin);
	return stroker.createStroke(basicShape());
}

void Flow::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	updateAnchors();
	if (isSelected()) {
		setPenAndBrushForSelection(painter);
	} else {
		QPen pen(QApplication::palette().text(), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
		painter->setPen(pen);
		painter->setBrush(Qt::transparent);
	}
	painter->drawPath(basicShape());
}

void Flow::paintSelection (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	setPenAndBrushForSelection(painter);
	painter->drawPath(shape());
}

void Flow::updateAnchors()
{
	while(anchors.count()>knots.count())
	{
		SizeAnchor *last = anchors[anchors.count()-1];
		disconnect(last, SIGNAL(moving()), this, SLOT(anchorMoved()));
		disconnect(last, SIGNAL(stopMoving()), this, SLOT(removeNeighborKnots()));
		disconnect(last, SIGNAL(startMoving()), this, SIGNAL(stateIsAboutToBeChangedByUser()));
		if (last->scene() && last->scene()->items().contains(last))
			last->scene()->removeItem(last);
		delete last;
		anchors.removeAt(anchors.count()-1);
	}
	while(anchors.count()<knots.count())
	{
		SizeAnchor *a;
		anchors << (a=new SizeAnchor(NULL));
		connect(a, SIGNAL(startMoving()), this, SIGNAL(stateIsAboutToBeChangedByUser()));
		connect(a, SIGNAL(moving()), this, SLOT(anchorMoved()));
		connect(a, SIGNAL(stopMoving()), this, SLOT(removeNeighborKnots()));
	}
	for(int i=0; i<anchors.count(); ++i)
		if (anchors[i]->scene()!=this->scene() && this->scene())
			this->scene()->addItem(anchors[i]);
	for(int i=0; i<anchors.count(); ++i)
		if (anchors[i]->scene())
		{
			anchors[i]->setVisible(this->isSelected());
			SizeAnchor *a = anchors[i];
			a->blockSignals(true);
			if (anchors[i]->parentItem() != nullptr) {
				anchors[i]->setPos(anchors[i]->parentItem()->mapFromScene(knots[i]));
			} else {
				anchors[i]->setPos(knots[i]);
			}
			a->blockSignals(false);
		}
}

void Flow::removeNeighborKnots()
{
	bool found;
	do {
		found = false;
		QPolygonF poly = createFlowPolygon();
		if (poly.count()==2) break;
		for(int i=1; i<poly.count(); ++i)
		{
			if ((poly[i-1] - poly[i]).manhattanLength() < 15)
			{
				knots.removeAt((i - (i==1? 0 : 1))-1);
				found = true;
				break;
			}
		}
	} while(found);
	updateAnchors();
	updateFlow();
}

QPolygonF Flow::createFlowPolygon() const
{
	QPolygonF poly;
	poly << _start_pt;
	for(int i=0; i<knots.count(); ++i)
		poly << knots[i];
	poly << _end_pt;
	return poly;
}

void Flow::anchorMoved()
{
	int index = anchors.indexOf((SizeAnchor *)sender());

	if (index>=0 && index<anchors.count())
	{
		QPolygonF poly = createFlowPolygon();
		poly[0] = QPointF(startShape()->scenePos() + QPointF(_start->boundingRect().width()/2, _start->boundingRect().height()/2));
		poly[poly.count()-1] = QPointF(endShape()->scenePos() + QPointF(_end->boundingRect().width()/2, _end->boundingRect().height()/2));
		int polyindex = index + 1;
		QList<double> ver = (QList<double>() << poly[polyindex-1].x() << poly[polyindex+1].x());
		QList<double> hor = (QList<double>() << poly[polyindex-1].y() << poly[polyindex+1].y());
		QPointF ap = anchors[index]->scenePos();

		double newx, newy;

		if (abs(ap.x()-ver[0])<5) newx = ver[0];
		else if (abs(ap.x()-ver[1])<5) newx = ver[1];
		else newx = ap.x();

		if (abs(ap.y()-hor[0])<5) newy = hor[0];
		else if (abs(ap.y()-hor[1])<5) newy = hor[1];
		else newy = ap.y();

		if (!anchors[index]->isBeingMovedByUser())
			knots[index] = anchors[index]->scenePos();
		else
			knots[index] = QPointF(newx, newy);
	}
	emit placementChanged();
	updateFlow();
}

void Flow::updateFlow()
{
	prepareGeometryChange();
	QPolygonF poly;
	poly << _start->scenePos() + QPointF(_start->boundingRect().width()/2, _start->boundingRect().height()/2);
	for(int i=0; i<knots.count(); ++i)
		poly << knots[i];
	poly << _end->scenePos() + QPointF(_end->boundingRect().width()/2, _end->boundingRect().height()/2);
	_start_pt = _start->flowConnectionPoint(poly[1], this) + _start->scenePos();
	_end_pt = _end->flowConnectionPoint(poly[poly.count()-2], this) + _end->scenePos();
}

void Flow::shapesChanged()
{
	updateFlow();
	emit moving();
}

QRectF Flow::boundingRect() const
{
	return basicShape().boundingRect()
			.normalized()
			.adjusted(-5,-5,5,5); // пофигу
}

void Flow::mousePressEvent (QGraphicsSceneMouseEvent *event)
{
	_dragstart = event->scenePos();
	_anchor_to_move = -1;
	Shape::mousePressEvent(event);
}

void Flow::mouseReleaseEvent (QGraphicsSceneMouseEvent *event)
{
	this->setCursor(Qt::ArrowCursor);
	_anchor_to_move = -1;
	Shape::mouseReleaseEvent(event);
	selectAnchors(isSelected());
}

void Flow::mouseMoveEvent (QGraphicsSceneMouseEvent *event)
{
	if ((event->scenePos() - _dragstart).manhattanLength() > 20
		&& _anchor_to_move == -1) {
		QPolygonF poly;
		poly << _start_pt;
		for(int i=0; i<knots.count(); ++i)
			poly << knots[i];
		poly << _end_pt;

		for(int i=1; i<poly.count(); ++i)
		{
			QPainterPath path;
			path.moveTo(poly[i-1]);
			path.lineTo(poly[i]);
			QPainterPathStroker stroker;
			stroker.setWidth(7);
			stroker.setCapStyle(Qt::RoundCap);
			stroker.setJoinStyle(Qt::RoundJoin);
			path = stroker.createStroke(path);
			if (path.contains(_dragstart))
			{
				knots.insert(i-1, _dragstart);
				updateAnchors();
				anchors[i-1]->setCursor(Qt::SizeFDiagCursor);
				anchors[i-1]->setVisible(true);
				anchors[i-1]->setSelected(false);
				_anchor_to_move = i-1;
				break;
			}
		}
	}

	else if (_anchor_to_move >= 0) {
		anchors[_anchor_to_move]->setPos(event->scenePos());
		emit placementChanged();
	}

	Shape::mouseMoveEvent(event);
}

void Flow::selectAnchors (bool select)
{
	for(int i=0; i<anchors.count(); ++i)
		if (anchors[i]->scene())
		{
			anchors[i]->setVisible(select);
			anchors[i]->setSelected(select);
		}
}

QVariant Flow::itemChange (GraphicsItemChange change, const QVariant &value)
{
	if (change==ItemSelectedChange)
		selectAnchors(qvariant_cast<bool>(value));
	if (change==ItemSceneChange)
	{
		QGraphicsScene *scene = qvariant_cast<QGraphicsScene *>(value);
		if (scene)
		{
			for(int i=0; i<anchors.count(); ++i)
				scene->addItem(anchors[i]);
		}
		else
		{
			for(int i=0; i<anchors.count(); ++i)
				if (anchors[i]->scene())
					anchors[i]->scene()->removeItem(anchors[i]);
		}
	}
	return Shape::itemChange(change, value);
}
