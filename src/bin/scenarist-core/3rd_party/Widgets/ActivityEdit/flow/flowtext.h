#ifndef FLOWTEXT_H
#define FLOWTEXT_H
#include "../shape/shape.h"
#include "arrowflow.h"

class ArrowFlow;

/**
 * @class FlowText
 * Представляет собой фигуру, отображающую текст над связью.
 * Ее можно перемещать, при этом она не привязывается к сетке.
 * Она жестко связана со своей связью и использует ее свойство text()
 * в качестве отображаемого текста. Отрисовка происходит также
 * относительно связи.
 *
 * @see ArrowFlow
 * @see ArrowFlow::text()
 */
class FlowText : public Shape
{
	Q_OBJECT
public:
	FlowText (ArrowFlow *flow, QGraphicsItem *parent = 0);
	Flow *flow() const;
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void updateShape();
	int offsetX() const, offsetY() const;
	void setOffset (int newx, int newy);
protected:
	QRectF _bounding, _textrect;
	virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	int _deltax, _deltay;
public slots:
	void flowPlacementChanged();
	void flowSelected();
	void flowContentsChanged();
private slots:
	void itWasMoved();
private:
	ArrowFlow *_flow;
};

#endif // FLOWTEXT_H
