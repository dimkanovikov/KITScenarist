#ifndef RESIZABLESHAPE_H
#define RESIZABLESHAPE_H

#include "shape.h"


/**
 * @class ResizableShape
 * Базовый класс для всех растягивающихся фигур.
 * При выделении такой фигуры в правом нижнем углу появляется маркер,
 * которым и производится изменение размера.
 */
class ResizableShape : public Shape
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

public:
	ResizableShape (QGraphicsItem *parent = 0);
	ResizableShape (const QPointF &pos, QGraphicsItem *parent = 0);
	virtual ~ResizableShape();
	virtual QRectF boundingRect() const;
	virtual void setSize (const QSizeF &newsize); QSizeF size() const;
	virtual void setMinSize (const QSizeF &mins); QSizeF minSize() const;
	virtual bool resizable() const; void setResizable (bool enabled);
	virtual void adjustSize();
protected:
	virtual void initialize();
	virtual void adjustAnchor();
	virtual QVariant itemChange (GraphicsItemChange change, const QVariant &value);
	virtual void showAnchors (bool visible = true);
    virtual int defaultZValue() const;
private:
	bool _resizable;
    SizeAnchor *m_anchor;
	QSizeF _size;
	QSizeF _minsize;
signals:
	void stopSizingByUser();
	void sizeChanged();
public slots:
	void anchorMoved();
};

#endif // RESIZABLESHAPE_H
