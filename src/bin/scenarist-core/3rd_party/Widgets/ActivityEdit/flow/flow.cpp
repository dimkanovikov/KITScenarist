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
	m_anchorToMove = -1;
	setGraphicsEffect(NULL);
	setFlag(ItemSendsGeometryChanges);
	setFlag(ItemIsMovable, false);
	m_startShape = start;
	m_endShape = end;
	if (m_startShape==m_endShape)
	{
		QRectF rect = m_startShape->boundingRect();
		int dx = rect.left()-30;
		int dy = rect.top()-30;
		m_knots << (m_startShape->scenePos() + QPointF(dx, start->center().y()));
		m_knots << (m_startShape->scenePos() + QPointF(dx, dy));
		m_knots << (m_startShape->scenePos() + QPointF(start->center().x(), dy));
		updateAnchors();
	}

	setZValue(1000);
	connectSignals();
	updateFlow();
}

Flow::~Flow()
{
	m_knots.clear();
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
	m_knots = knots;
	updateFlow();
}

void Flow::removeAllFlowKnots()
{
	m_knots.clear();

	updateAnchors();
	updateFlow();
}

void Flow::setStartShape (Shape *s)
{
	disconnectSignals();
	m_startShape = s;
	updateFlow();
	connectSignals();
}

void Flow::setEndShape (Shape *s)
{
	disconnectSignals();
	m_endShape = s;
	updateFlow();
	connectSignals();
}

void Flow::connectSignals()
{
	connect(m_startShape, SIGNAL(placementChanged()), this, SLOT(shapesChanged()));
	connect(m_endShape, SIGNAL(placementChanged()), this, SLOT(shapesChanged()));
}

void Flow::disconnectSignals()
{
	disconnect(m_startShape, SIGNAL(placementChanged()), this, SLOT(shapesChanged()));
	disconnect(m_endShape, SIGNAL(placementChanged()), this, SLOT(shapesChanged()));
}

QPointF Flow::flowConnectionPoint (const QPointF &anotherEnd, Flow *flow) const
{
	Q_UNUSED(anotherEnd);
	Q_UNUSED(flow);

	return center();
}

Shape *Flow::startShape() const
{
	return m_startShape;
}

Shape *Flow::endShape() const
{
	return m_endShape;
}

QPainterPath Flow::basicShape() const
{
	QPainterPath path;
	path.moveTo(m_startPoint);
	for(int i=0; i<m_knots.count(); ++i)
		path.lineTo(m_knots[i]);
	path.lineTo(m_endPoint);
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
	Q_UNUSED(option);
	Q_UNUSED(widget);

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
	Q_UNUSED(option);
	Q_UNUSED(widget);

	setPenAndBrushForSelection(painter);
	painter->drawPath(shape());
}

void Flow::updateAnchors()
{
	while(m_anchors.count()>m_knots.count())
	{
		SizeAnchor *last = m_anchors[m_anchors.count()-1];
		disconnect(last, SIGNAL(moving()), this, SLOT(anchorMoved()));
		disconnect(last, SIGNAL(stopMoving()), this, SLOT(removeNeighborKnots()));
		disconnect(last, SIGNAL(startMoving()), this, SIGNAL(stateIsAboutToBeChangedByUser()));
		if (last->scene() && last->scene()->items().contains(last))
			last->scene()->removeItem(last);
		delete last;
		m_anchors.removeAt(m_anchors.count()-1);
	}
	while(m_anchors.count()<m_knots.count())
	{
		SizeAnchor *a;
		m_anchors << (a=new SizeAnchor(NULL));
		connect(a, SIGNAL(startMoving()), this, SIGNAL(stateIsAboutToBeChangedByUser()));
		connect(a, SIGNAL(moving()), this, SLOT(anchorMoved()));
		connect(a, SIGNAL(stopMoving()), this, SLOT(removeNeighborKnots()));
	}
	for(int i=0; i<m_anchors.count(); ++i)
		if (m_anchors[i]->scene()!=this->scene() && this->scene())
			this->scene()->addItem(m_anchors[i]);
	for(int i=0; i<m_anchors.count(); ++i)
		if (m_anchors[i]->scene())
		{
			m_anchors[i]->setVisible(this->isSelected());
			SizeAnchor *a = m_anchors[i];
			a->blockSignals(true);
			if (m_anchors[i]->parentItem() != nullptr) {
				m_anchors[i]->setPos(m_anchors[i]->parentItem()->mapFromScene(m_knots[i]));
			} else {
				m_anchors[i]->setPos(m_knots[i]);
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
				m_knots.removeAt((i - (i==1? 0 : 1))-1);
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
	poly << m_startPoint;
	for(int i=0; i<m_knots.count(); ++i)
		poly << m_knots[i];
	poly << m_endPoint;
	return poly;
}

void Flow::anchorMoved()
{
	int index = m_anchors.indexOf((SizeAnchor *)sender());

	if (index>=0 && index<m_anchors.count())
	{
		QPolygonF poly = createFlowPolygon();
		poly[0] = QPointF(startShape()->scenePos() + QPointF(m_startShape->boundingRect().width()/2, m_startShape->boundingRect().height()/2));
		poly[poly.count()-1] = QPointF(endShape()->scenePos() + QPointF(m_endShape->boundingRect().width()/2, m_endShape->boundingRect().height()/2));
		int polyindex = index + 1;
		QList<double> ver = (QList<double>() << poly[polyindex-1].x() << poly[polyindex+1].x());
		QList<double> hor = (QList<double>() << poly[polyindex-1].y() << poly[polyindex+1].y());
		QPointF ap = m_anchors[index]->scenePos();

		double newx, newy;

		if (abs(ap.x()-ver[0])<5) newx = ver[0];
		else if (abs(ap.x()-ver[1])<5) newx = ver[1];
		else newx = ap.x();

		if (abs(ap.y()-hor[0])<5) newy = hor[0];
		else if (abs(ap.y()-hor[1])<5) newy = hor[1];
		else newy = ap.y();

		if (!m_anchors[index]->isBeingMovedByUser())
			m_knots[index] = m_anchors[index]->scenePos();
		else
			m_knots[index] = QPointF(newx, newy);
	}
	emit placementChanged();
	updateFlow();
}

void Flow::updateFlow()
{
	prepareGeometryChange();
	QPolygonF poly;
	poly << m_startShape->scenePos() + QPointF(m_startShape->boundingRect().width()/2, m_startShape->boundingRect().height()/2);
	for(int i=0; i<m_knots.count(); ++i)
		poly << m_knots[i];
	poly << m_endShape->scenePos() + QPointF(m_endShape->boundingRect().width()/2, m_endShape->boundingRect().height()/2);
	m_startPoint = m_startShape->flowConnectionPoint(poly[1], this) + m_startShape->scenePos();
	m_endPoint = m_endShape->flowConnectionPoint(poly[poly.count()-2], this) + m_endShape->scenePos();
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
	m_dragstart = event->scenePos();
	m_anchorToMove = -1;
	Shape::mousePressEvent(event);
}

void Flow::mouseReleaseEvent (QGraphicsSceneMouseEvent *event)
{
	this->setCursor(Qt::ArrowCursor);
	m_anchorToMove = -1;
	Shape::mouseReleaseEvent(event);
	selectAnchors(isSelected());
}

void Flow::mouseMoveEvent (QGraphicsSceneMouseEvent *event)
{
	if ((event->scenePos() - m_dragstart).manhattanLength() > 20
		&& m_anchorToMove == -1) {
		QPolygonF poly;
		poly << m_startPoint;
		for(int i=0; i<m_knots.count(); ++i)
			poly << m_knots[i];
		poly << m_endPoint;

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
			if (path.contains(m_dragstart))
			{
				m_knots.insert(i-1, m_dragstart);
				updateAnchors();
				m_anchors[i-1]->setCursor(Qt::SizeFDiagCursor);
				m_anchors[i-1]->setVisible(true);
				m_anchors[i-1]->setSelected(false);
				m_anchorToMove = i-1;
				break;
			}
		}
	}

	else if (m_anchorToMove >= 0) {
		m_anchors[m_anchorToMove]->setPos(event->scenePos());
		emit placementChanged();
	}

	Shape::mouseMoveEvent(event);
}

void Flow::selectAnchors (bool select)
{
	for(int i=0; i<m_anchors.count(); ++i)
		if (m_anchors[i]->scene())
		{
			m_anchors[i]->setVisible(select);
			m_anchors[i]->setSelected(select);
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
			for(int i=0; i<m_anchors.count(); ++i)
				scene->addItem(m_anchors[i]);
		}
		else
		{
			for(int i=0; i<m_anchors.count(); ++i)
				if (m_anchors[i]->scene())
					m_anchors[i]->scene()->removeItem(m_anchors[i]);
		}
	}
	return Shape::itemChange(change, value);
}
