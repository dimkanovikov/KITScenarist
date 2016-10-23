#include "horizontalline.h"

#include <QCursor>
#include <QGraphicsScene>
#include <QPainter>


namespace {
	/**
	 * @brief Минимальная ширина линии
	 * @brief Используется, если ещё не задана сцена
	 */
	const int LINE_MIN_WIDTH = 100;

	/**
	 * @brief Высота линии
	 */
	const int LINE_HEIGHT = 5;
}


HorizontalLineShape::HorizontalLineShape(QGraphicsItem* _parent) :
	Shape(_parent),
	m_inBoundingRect(false)
{
	setCursor(Qt::SizeVerCursor);
}

HorizontalLineShape::HorizontalLineShape(const QPointF& _pos, QGraphicsItem* _parent) :
	Shape(_parent)
{
	setPos(0, _pos.y());
	setCursor(Qt::SizeVerCursor);
}

QRectF HorizontalLineShape::boundingRect() const
{
    QRectF result(0, 0, LINE_MIN_WIDTH, LINE_HEIGHT);
	if (!m_inBoundingRect) {
		m_inBoundingRect = true;
		if (scene() != nullptr) {
			result.setWidth(scene()->width() - LINE_HEIGHT);
		}
		m_inBoundingRect = false;
	}

	return result;
}

void HorizontalLineShape::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget)
{
	Q_UNUSED(_option);
	Q_UNUSED(_widget);

    //
    // Скорректируем позицию, если нужно
    //
    if (scene() != nullptr
        && pos().x() != scene()->sceneRect().x()) {
        setPos(scene()->sceneRect().x(), pos().y());
    }

    //
    // Рисуем линию
    //
	_painter->save();
	if (isSelected()) {
		setPenAndBrushForSelection(_painter);
		_painter->setBrush(_painter->pen().color());
	} else {
		_painter->setPen(Qt::black);
		_painter->setBrush(Qt::black);
	}
	_painter->drawRect(boundingRect());
	_painter->restore();
}

QVariant HorizontalLineShape::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value)
{
    switch (_change) {
        case ItemPositionChange: {
            QPointF newPosition = _value.toPointF();
            newPosition.setX(scene() != nullptr ? scene()->sceneRect().x() : 0);
            emit moving();
            return newPosition;
        }

        default: {
            break;
        }
    }

	return Shape::itemChange(_change, _value);
}
