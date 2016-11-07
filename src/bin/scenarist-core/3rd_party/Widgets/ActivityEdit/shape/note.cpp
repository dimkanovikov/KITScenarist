#include "note.h"

#include "textutils.h"

#include <QApplication>
#include <QPainter>
#include <QPalette>

namespace {
	const int DEFAULT_WIDTH = 150;
	const int DEFAULT_HEIGHT = 80;
}


NoteShape::NoteShape(QGraphicsItem *_parent) :
	ResizableShape(_parent)
{
	setSize(QSizeF(DEFAULT_WIDTH, DEFAULT_HEIGHT));
}


NoteShape::NoteShape(const QString& _text, const QPointF& _pos, QGraphicsItem* _parent) :
	ResizableShape(_pos, _parent),
	m_text(_text)
{
	setSize(QSizeF(DEFAULT_WIDTH, DEFAULT_HEIGHT));
}

void NoteShape::setText(const QString& _text)
{
	if (m_text != _text) {
		prepareGeometryChange();
		m_text = _text;
		emit contentsChanged();
	}
}

QString NoteShape::text() const
{
	return m_text;
}

void NoteShape::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget)
{
	Q_UNUSED(_option);
	Q_UNUSED(_widget);

	const QPalette palette = QApplication::palette();

	//
	// Рисуем фон
	//
	_painter->setBrush(palette.base());
	_painter->setPen(palette.text().color());
	_painter->drawPath(shape());
	QRectF r = boundingRect();
	//
	// ... декорацию угла
	//
	_painter->drawPolygon(QPolygonF()
		 << QPointF(r.width() - 10, 3)
		 << QPointF(r.width() - 10, 10)
		 << QPointF(r.width() - 3, 10)
	);

	//
	// Рисуем текст
	//
	_painter->setFont(Shape::basicFont());
	QTextOption textoption;
	textoption.setAlignment(Qt::AlignLeft | Qt::AlignTop);
	textoption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	const QRectF titleRect = r.adjusted(9, 9, -9, -9);
	const QString titleText = TextUtils::elidedText(text(), _painter->font(), titleRect.size(), textoption);
	_painter->drawText(titleRect, titleText, textoption);

	//
	// Рисуем выделение
	//
	if (isSelected()) {
		setPenAndBrushForSelection(_painter);
		_painter->drawPath(selectionShape());
	}
}


QPainterPath NoteShape::shape() const
{
	QRectF r = boundingRect();
	QPainterPath path;
	path.addPolygon(QPolygonF()
		<< QPointF(3, 3)
		<< QPointF(r.width() - 10, 3)
		<< QPointF(r.width() - 3, 10)
		<< QPointF(r.width() - 3, r.height() - 3)
		<< QPointF(3, r.height() - 3)
		<< QPointF(3, 3)
	);
	return path;
}

QPainterPath NoteShape::selectionShape() const
{
	QRectF r = boundingRect();
	QPainterPath path;
	path.addPolygon(QPolygonF()
		<< QPointF(1, 1)
		<< QPointF(r.width()-9, 1)
		<< QPointF(r.width()-1, 9)
		<< QPointF(r.width()-1, r.height()-1)
		<< QPointF(1, r.height()-1)
		<< QPointF(1, 1)
	);
	return path;
}

int NoteShape::defaultZValue() const
{
	return 10;
}
