#include <QGraphicsScene>
#include "../anchor/sizeanchor.h"
#include <QDebug>
#include <QPainter>
#include <QMessageBox>
#include "../flow/flow.h"
#include "resizableshape.h"
#include "../scene/customgraphicsscene.h"

namespace {
	/**
	 * @brief Испустить сигнал о перемещении фигуры и всех его детей и якорей
	 */
	template<typename T>
	static void emitMovingFor(T* _shape) {
		//
		// Сигналим о смещении самой фигуры
		//
		emit _shape->moving();
		//
		// Сигналим о смещении детей
		//
		foreach (QGraphicsItem* child, _shape->childItems()) {
			if (Shape* childShape = dynamic_cast<Shape*>(child)) {
				emitMovingFor(childShape);
			} else if (SizeAnchor* childAncor = dynamic_cast<SizeAnchor*>(child)) {
				emitMovingFor(childAncor);
			}
		}
	}
}


void ResizableShape::adjustSize()
{
	setSize(minSize());
}

ResizableShape::ResizableShape (QGraphicsItem *parent)
	: QGraphicsItem(parent)
{
	initialize();
}

ResizableShape::ResizableShape (const QPointF &pos, QGraphicsItem *parent)
	: QGraphicsItem(parent)
	, Shape(parent)
{
	initialize();
	setPos(pos);
}

void ResizableShape::initialize()
{
	setFlag(ItemIsMovable);
	setFlag(ItemIsSelectable);
	setFlag(ItemSendsGeometryChanges);
	setVisible(true);
	_size = QSizeF(100, 30);
	_minsize = QSizeF(70, 30);
	setResizable(true);
	anchor = new SizeAnchor(this);
	anchor->setZValue(zValue()+1);
	anchor->setVisible(false);
	connect(anchor, SIGNAL(stopMoving()), this, SIGNAL(stopSizingByUser()));
	connect(anchor, SIGNAL(movingByUser()), this, SLOT(anchorMoved()));
	connect(this, SIGNAL(sizeChanged()), this, SIGNAL(placementChanged()));
}

ResizableShape::~ResizableShape()
{
	if (scene())
	{
		disconnect(anchor, SIGNAL(startMoving()), this, SIGNAL(stateIsAboutToBeChangedByUser()));
		scene()->removeItem(anchor);
	}
	delete anchor;
}

QRectF ResizableShape::boundingRect() const
{
	return QRectF(QPointF(0,0), size());
}

/**
 * Подогнать положение маркера при перемещении фигуры или изменении размера извне.
 */
void ResizableShape::adjustAnchor()
{
	anchor->blockSignals(true);
	anchor->setPos(scenePos().x() + size().width(), scenePos().y() + size().height());
	anchor->blockSignals(false);

	foreach (QGraphicsItem* child, childItems()) {
		if (ResizableShape* childShape = dynamic_cast<ResizableShape*>(child)) {
			childShape->adjustAnchor();
		}
	}
}

QVariant ResizableShape::itemChange (GraphicsItemChange change, const QVariant &value)
{
	switch(change)
	{
		case ItemSceneChange:
		{
			QGraphicsScene *scene = qvariant_cast<QGraphicsScene *>(value);
			if (scene)
			{
				scene->addItem(anchor);
				connect(anchor, SIGNAL(startMoving()), this, SIGNAL(stateIsAboutToBeChangedByUser()));
			}
			else
			{
				disconnect(anchor, SIGNAL(startMoving()), this, SIGNAL(stateIsAboutToBeChangedByUser()));
				this->scene()->removeItem(anchor);
			}
			adjustAnchor();
			break;
		}
		case ItemPositionHasChanged:
			adjustAnchor();
			::emitMovingFor(this);
			break;
		case ItemSelectedChange:
			showAnchors(!isSelected());
			emit selected();
			break;
		default:
			break;
	}
	return QGraphicsItem::itemChange(change, value);
}

/**
 * Показать/скрыть маркер.
 * @param visible true, если он должен быть виден
 */
void ResizableShape::showAnchors (bool visible)
{
	anchor->setVisible(visible && resizable());
}

/**
 * Слот активируется, когда маркер перемещен.
 * Он изменяет размеры соответственно положению маркера.
 * При этом контролируется, чтобы они были не меньше минимальных.
 */
void ResizableShape::anchorMoved()
{
	prepareGeometryChange();
	QPointF pt = anchor->scenePos() - scenePos();

	//
	// Определим минимальный размер фигуры
	//
	QSizeF minimumSize = _minsize;
	//
	// ... если есть вложенные элементы, то фигура не может быть меньше области занимаемой детьми
	//
	if (!childItems().isEmpty()) {
		qreal maxWidth = minimumSize.width();
		qreal maxHeight = minimumSize.height();
		for (QGraphicsItem* childItem : childItems()) {
			const QRectF childRect = childItem->mapToParent(childItem->boundingRect()).boundingRect();
			if (childRect.right() > maxWidth) {
				maxWidth = childRect.right();
			}
			if (childRect.bottom() > maxHeight) {
				maxHeight = childRect.bottom();
			}
		}
		minimumSize = QSizeF(maxWidth, maxHeight);
	}

	//
	// Корректируем позицию маркера
	//
	if (pt.x() < minimumSize.width()) {
		pt.setX(minimumSize.width());
	}
	if (pt.y() < minimumSize.height()) {
		pt.setY(minimumSize.height());
	}

	setSize(QSizeF(pt.x(), pt.y()));
}

/**
 * @return Размер фигуры.
 */
QSizeF ResizableShape::size() const
{
	return _size;
}

/**
 * Изменить размеры фигуры.
 * @param newsize Новый размер фигуры.
 */
void ResizableShape::setSize(const QSizeF &newsize)
{
	_size = newsize;
	adjustAnchor();
	update();
	emit sizeChanged();
}

/**
 * @return Минимальный размер фигуры.
 */
QSizeF ResizableShape::minSize() const
{
	return _minsize;
}

/**
 * Задать минимальный размер фигуры.
 * @param mins Новый минимальный размер фигуры.
 */
void ResizableShape::setMinSize (const QSizeF &mins)
{
	_minsize = mins;
}

/**
 * Запретить/разрешить изменение размеров.
 * @param enabled true, если размеры можно менять.
 */
void ResizableShape::setResizable (bool enabled)
{
	_resizable = enabled;
}

/**
 * @return true, если размеры можно поменять
 */
bool ResizableShape::resizable() const
{
	return _resizable;
}
