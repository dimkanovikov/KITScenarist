#ifndef SHAPE_NOTE_H
#define SHAPE_NOTE_H

#include "resizableshape.h"


/**
 * @class Заметка
 */
class NoteShape : public ResizableShape
{
	Q_OBJECT

public:
	NoteShape(QGraphicsItem* _parent = NULL);
	NoteShape(const QString& _text, const QPointF& _pos, QGraphicsItem* _parent = NULL);

	/**
	 * @brief Текст заметки
	 */
	/** @{ */
	void setText(const QString& _text);
	QString text() const;
	/** @} */

protected:
	/**
	 * @brief Нарисовать заметку
	 */
	void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) override;

	/**
	 * @brief Переопределяем для более точного определения области заметки
	 */
	QPainterPath shape() const override;

	/**
	 * @brief Область выделения заметки
	 */
	QPainterPath selectionShape() const;

    /**
     * @brief Z-индекс для заметки по умолчанию должен быть больше чем у карточки
     */
    int defaultZValue() const;

private:
	/**
	 * @brief Текст заметки
	 */
	QString m_text;
};


#endif // SHAPE_NOTE_H
