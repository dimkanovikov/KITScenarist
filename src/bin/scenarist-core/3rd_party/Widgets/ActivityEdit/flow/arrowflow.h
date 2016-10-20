#ifndef ARROWFLOW_H
#define ARROWFLOW_H
#include "flow.h"

class FlowText;

/****************************************************************//**
 * @class ArrowFlow
 * @brief Control flow, связь со стрелкой на конце.
 * Как и все связи, позволяет вытягивать узлы.
 * Имеет одно свойство - текст над связью. Он автоматически
 * заключается в кавычки при выводе. Наконечник стрелки рисуется
 * у конечной фигуры.
 * @see Shape
 * @see Flow
 ********************************************************************/

class ArrowFlow : public Flow
{
	Q_OBJECT
public:
	static bool canConnect (Shape *s, Shape *e);	///< Проверка возможности соединения фигур этой связью
	virtual bool canConnectStartTo (Shape *s);
	virtual bool canConnectEndTo (Shape *s);
	ArrowFlow(Shape *start, Shape *end, QGraphicsItem *parent = 0);
	~ArrowFlow();
	virtual void setText (const QString &newtext);	///< Задать текст над связью
	virtual const QString &text() const;			///< Возвращает текст над связью
	virtual FlowText *textShape() const;			///< Возвращает указатель на фигуру текста

	/**
	 * @brief Переопределяем, чтобы скрыть соединение родителя с потомком
	 */
	QPainterPath shape() const override;

protected:
	/// Нарисовать наконечник стрелки
	virtual void paintCap (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual QVariant itemChange (GraphicsItemChange change, const QVariant &value);
	void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
	FlowText *_flowtext;	/// Указатель на связанную со связью фигуру текста
	QString _text;			/// Текст над связью
};

#endif // ARROWFLOW_H
