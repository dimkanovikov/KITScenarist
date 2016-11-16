#ifndef VERTICALLINE_H
#define VERTICALLINE_H

#include "shape.h"


/**
 * @brief Вертикальная линия на всю сцену
 */
class VerticalLineShape : public virtual Shape
{
	Q_OBJECT

public:
	VerticalLineShape(QGraphicsItem* _parent = NULL);
	VerticalLineShape(const QPointF& _pos, QGraphicsItem* _parent = NULL);

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
	 * @brief Переопределяем, чтобы смещать линию строго по горизонтали
	 */
	QVariant itemChange(GraphicsItemChange _change, const QVariant& _value) override;

private:
	/**
	 * @brief Находимся ли в методе определения области отрисовки линии, чтобы избежать зацикливания
	 */
	mutable bool m_inBoundingRect;
};

#endif // VERTICALLINE_H
