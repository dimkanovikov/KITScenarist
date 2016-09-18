#include "card.h"

#include "pdlg_action.h"
#include "textutils.h"

#include <QApplication>
#include <QPainter>

namespace {
	/**
	 * @brief Поссчитать количество родителей элемента
	 */
	static int parentItemsCounter(const QGraphicsItem* item) {
		int count = 0;
		if (item->parentItem() != nullptr) {
			++count;
			count += parentItemsCounter(item->parentItem());
		}

		return count;
	}
}


CardShape::CardShape(QGraphicsItem* _parent) :
	ResizableShape(_parent),
	m_cardType(TypeScene),
	m_isOnInsertionState(false)
{
	setMinSize(QSizeF(50, 32));
	setSize(QSizeF(DEFAULT_WIDTH, DEFAULT_HEIGHT));
}

CardShape::CardShape(CardType _type, const QString& _title, const QString& _description, const QPointF& _pos, QGraphicsItem* _parent) :
	ResizableShape(_pos, _parent),
	m_cardType(_type),
	m_title(_title),
	m_description(_description),
	m_isOnInsertionState(false)
{
	setMinSize(QSizeF(50,32));
	setSize(QSizeF(DEFAULT_WIDTH, DEFAULT_HEIGHT));
}

void CardShape::setCardType(CardShape::CardType _type)
{
	if (m_cardType != _type) {
		prepareGeometryChange();
		m_cardType = _type;
		emit contentsChanged();
	}
}

CardShape::CardType CardShape::cardType() const
{
	return m_cardType;
}

void CardShape::setTitle(const QString& _title)
{
	if (m_title != _title) {
		prepareGeometryChange();
		m_title = _title;
		emit contentsChanged();
	}
}

QString CardShape::title() const
{
	return m_title;
}

void CardShape::setDescription (const QString &_description)
{
	if (m_description != _description) {
		prepareGeometryChange();
		m_description = _description;
		emit contentsChanged();
	}
}

QString CardShape::description() const
{
	return m_description;
}

void CardShape::setOnInstertionState(bool _on)
{
	if (m_isOnInsertionState != _on) {
		prepareGeometryChange();
		m_isOnInsertionState = _on;
	}
}

bool CardShape::isOnInstertionState() const
{
	return m_isOnInsertionState;
}

int CardShape::depthLevel() const
{
	return ::parentItemsCounter(this);
}

bool CardShape::isGrandParentOf(QGraphicsItem* _item) const
{
	QGraphicsItem* parent = _item->parentItem();
	while (parent != nullptr) {
		if (parent == this) {
			return true;
		}
		parent = parent->parentItem();
	}

	return false;
}

void CardShape::adjustSize()
{
	QTextOption textoption;
	textoption.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	textoption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	QSizeF minsz = TextUtils::textRect(
			title(),
			Shape::basicFont(),
			size().width()-14,
			textoption
	);
	setMinSize(QSizeF(minSize().width(), minsz.height()+14));
	if (size().height() < minSize().height())
		setSize(QSizeF(size().width(), minSize().height()));
}

void CardShape::editProperties()
{
	ActionShapePropertiesDialog(this).exec();
}

void CardShape::paint(QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget)
{
	Q_UNUSED(_option);
	Q_UNUSED(_widget);

	adjustSize();

	//
	// Рисуем фон
	//
	_painter->setFont(Shape::basicFont());
	_painter->setPen(Qt::black);
	_painter->setBrush(Shape::innerBrush());
	_painter->drawRect(boundingRect().adjusted(3, 3, -3, -3));
	QTextOption textoption;
	textoption.setAlignment(Qt::AlignTop | Qt::AlignLeft);

	//
	// Рисуем заголовок
	//
	textoption.setWrapMode(QTextOption::NoWrap);
	QFont font = _painter->font();
	font.setBold(true);
	_painter->setFont(font);
	const QRectF titleRect(9, 9, size().width() - 18, _painter->fontMetrics().height());
	const QString titleText = TextUtils::elidedText(title(), _painter->font(), titleRect.size(), textoption);
	_painter->drawText(titleRect, titleText, textoption);

	//
	// Рисуем описание
	//
	textoption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	font.setBold(false);
	_painter->setFont(font);
	const int spacing = titleRect.height() / 2;
	const QRectF descriptionRect(9, titleRect.bottom() + spacing, size().width() - 18, size().height() - titleRect.bottom() - spacing - 9);
	const QString descriptionText = TextUtils::elidedText(description(), _painter->font(), descriptionRect.size(), textoption);
	_painter->drawText(descriptionRect, descriptionText, textoption);

	//
	// Если это группирующий элемент, рисуем область вложения
	//
	if (cardType() == TypeScenesGroup
		|| cardType() == TypeFolder) {
		const QSizeF fullDescriptionRectSize = TextUtils::textRect(description(), _painter->font(), descriptionRect.width(), textoption);
		//
		// Если на карточке есть свободное место для отрисовки декорации
		//
		if (fullDescriptionRectSize.height() < descriptionRect.height()) {
			const QRectF childsRect(
				descriptionRect.left(),
				descriptionRect.top() + fullDescriptionRectSize.height() + spacing,
				descriptionRect.width(),
				descriptionRect.height() - fullDescriptionRectSize.height() - spacing);
			if (m_isOnInsertionState) {
				_painter->fillRect(childsRect, QApplication::palette().highlight());
			} else {
				_painter->fillRect(childsRect, QApplication::palette().dark());
			}
		}
	}

	//
	// Рисуем рамку выделения
	//
	if (isSelected()) {
		setPenAndBrushForSelection(_painter);
		_painter->drawRect(boundingRect().adjusted(1, 1, -1, -1));
	}
}
