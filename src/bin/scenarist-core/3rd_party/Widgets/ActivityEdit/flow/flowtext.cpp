#include "flowtext.h"

#include "../scene/customgraphicsscene.h"

#include <QApplication>
#include <QGraphicsScene>
#include <QPainter>
#include <QPalette>

FlowText::FlowText (ArrowFlow *flow, QGraphicsItem *parent)
	: Shape(parent)
	, _flow(flow)
{
	setGraphicsEffect(NULL);
	_deltax = 0, _deltay = 0;
	connect(_flow, SIGNAL(selected()), this, SLOT(flowSelected()));
	connect(_flow, SIGNAL(placementChanged()), this, SLOT(flowPlacementChanged()));
	connect(_flow, SIGNAL(contentsChanged()), this, SLOT(flowContentsChanged()));
	connect(this, SIGNAL(movingByUser()), SLOT(itWasMoved()));

	setFlag(QGraphicsItem::ItemIsMovable, false);
	setFlag(QGraphicsItem::ItemIsSelectable, false);

	setZValue(1001);
}

Flow *FlowText::flow() const
{
	return _flow;
}

QRectF FlowText::boundingRect() const
{
	return _bounding;
}

QPainterPath FlowText::shape() const
{
	QPainterPath shp;
	shp.addRect(_textrect);
	return shp;
}

void FlowText::itWasMoved()
{
	_deltax = (_flow->center()-this->pos()).x();
	_deltay = (_flow->center()-this->pos()).y();
	updateShape();
	prepareGeometryChange();
}

int FlowText::offsetX() const
{
	return _deltax;
}

int FlowText::offsetY() const
{
	return _deltay;
}

void FlowText::setOffset (int newx, int newy)
{
	_deltax = newx, _deltay = newy;
	updateShape();
}

void FlowText::updateShape()
{
	this->setVisible(_flow->text()!="");
	this->setPos(_flow->center()-QPointF(_deltax, _deltay));
	QPainterPath _path;
	QFontMetricsF fm(Shape::basicFont());
	_textrect = fm.boundingRect(QRectF(0,0,10000,10000), Qt::AlignLeft, "["+_flow->text()+"]");
	_textrect = _textrect.translated(-_textrect.width()/2.0, -_textrect.height()/2.0);
	_path.addRect(_textrect);
	_path.addRect(QRectF(QPointF(
			_textrect.left() + _textrect.width()/2.0,
			_textrect.height()),
			mapFromItem(_flow, _flow->center())
	));
	_bounding = _path.boundingRect().adjusted(-4,-4,4,4);
}

void FlowText::paint (QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget)
{
    Q_UNUSED(_option);
    Q_UNUSED(_widget);

    const QPalette palette = QApplication::palette();

    _painter->setFont(Shape::basicFont());
    const QColor textColor = palette.text().color();
    QColor bgColor(textColor.red(), textColor.green(), textColor.blue(), 200);
    _painter->setBrush(bgColor);
    _painter->setPen(Qt::NoPen);
    _painter->drawRoundedRect(_textrect.adjusted(-2,-2,2,2), 5,5);
    _painter->setPen(palette.base().color());
    _painter->drawText(_textrect.adjusted(0,0,100,100), _flow->text());

	if (isSelected())
	{
        setPenAndBrushForSelection(_painter);
        _painter->drawRoundedRect(QRectF(
				QPointF(_textrect.left(), _textrect.top()),
				QPointF(_textrect.left()+_textrect.width(), _textrect.top()+_textrect.height())
			).adjusted(-3,-3,3,3), 5,5);
        _painter->drawLine(
				QPointF(_textrect.left(), _textrect.top()+_textrect.height()),
				QPointF(_textrect.left()+_textrect.width(), _textrect.top()+_textrect.height())
		);
        _painter->drawLine(
			QPointF(
				_textrect.left() + _textrect.width()/2.0,
				_textrect.top() + _textrect.height()),
			mapFromItem(_flow, _flow->center())
		);
	}

}

void FlowText::flowContentsChanged()
{
	prepareGeometryChange();
	updateShape();
}

void FlowText::flowPlacementChanged()
{
	prepareGeometryChange();
	updateShape();
}

void FlowText::flowSelected()
{
	prepareGeometryChange();
}

