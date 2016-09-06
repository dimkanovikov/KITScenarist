#ifndef HORIZONTALLINE_H
#define HORIZONTALLINE_H

#include "shape.h"


/**
 * @brief Горизонтальная линия на всю сцену
 */
class HorizontalLineShape : public virtual Shape
{
	Q_OBJECT

public:
	HorizontalLineShape(QGraphicsItem* _parent = NULL);
	HorizontalLineShape(const QPointF& _pos, QGraphicsItem* _parent = NULL);

	/**
	 * @brief Область, которую занимает линия
	 */
	QRectF boundingRect() const override;

protected:
	/**
	 * @brief Нарисовать линию
	 */
	void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) override;

	/**
	 * @brief Переопределяем, чтобы смещать линию строго по вертикали
	 */
	QVariant itemChange(GraphicsItemChange _change, const QVariant& _value) override;

private:
	/**
	 * @brief Находимся ли в методе определения области отрисовки линии, чтобы избежать зацикливания
	 */
	mutable bool m_inBoundingRect;
};

#endif // HORIZONTALLINE_H
