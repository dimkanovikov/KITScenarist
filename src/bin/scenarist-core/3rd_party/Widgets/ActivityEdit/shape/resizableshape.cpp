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
    : QGraphicsItem(parent), Shape(parent)
{
	initialize();
}

ResizableShape::ResizableShape (const QPointF &pos, QGraphicsItem *parent)
    : QGraphicsItem(parent), Shape(parent)
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
    m_anchor = new SizeAnchor(this);
    m_anchor->setZValue(zValue()+1);
    m_anchor->setVisible(false);
    connect(m_anchor, SIGNAL(stopMoving()), this, SIGNAL(stopSizingByUser()));
    connect(m_anchor, SIGNAL(movingByUser()), this, SLOT(anchorMoved()));
	connect(this, SIGNAL(sizeChanged()), this, SIGNAL(placementChanged()));
}

ResizableShape::~ResizableShape()
{
    if (scene())
    {
        disconnect(m_anchor, &SizeAnchor::startMoving, this, &ResizableShape::stateIsAboutToBeChangedByUser);
        scene()->removeItem(m_anchor);
    }
    delete m_anchor;
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
    m_anchor->blockSignals(true);
    m_anchor->setPos(scenePos().x() + size().width(), scenePos().y() + size().height());
    m_anchor->blockSignals(false);

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
                scene->addItem(m_anchor);
                connect(m_anchor, SIGNAL(startMoving()), this, SIGNAL(stateIsAboutToBeChangedByUser()));
			}
			else
			{
                disconnect(m_anchor, SIGNAL(startMoving()), this, SIGNAL(stateIsAboutToBeChangedByUser()));
                this->scene()->removeItem(m_anchor);
			}
			adjustAnchor();
			break;
		}
		case ItemPositionHasChanged:
			adjustAnchor();
			::emitMovingFor(this);
			break;
        case ItemSelectedChange: {
            setZValue(value.toBool() ? 1000 : defaultZValue());
            showAnchors(!isSelected());
            emit selected();
            break;
        }
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
    m_anchor->setVisible(visible && resizable());
}

int ResizableShape::defaultZValue() const
{
    return 1;
}

/**
 * Слот активируется, когда маркер перемещен.
 * Он изменяет размеры соответственно положению маркера.
 * При этом контролируется, чтобы они были не меньше минимальных.
 */
void ResizableShape::anchorMoved()
{
	prepareGeometryChange();
    QPointF pt = m_anchor->scenePos() - scenePos();

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
