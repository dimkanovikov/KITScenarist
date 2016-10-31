#include "card.h"

#include "textutils.h"

#include <3rd_party/Helpers/ImageHelper.h>

#include <QApplication>
#include <QPainter>
#include <QPalette>
#include <QStyleOptionGraphicsItem>

namespace {
	/**
	 * @brief Высота декорации дополнительных цветов
	 */
	const int ADDITIONAL_COLORS_HEIGHT = 10;

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

CardShape::CardShape(const QString& _uuid, CardType _type, const QString& _title,
	const QString& _description, const QString& _colors, const QPointF& _pos, QGraphicsItem* _parent) :
	ResizableShape(_pos, _parent),
	m_uuid(_uuid),
	m_cardType(_type),
	m_title(_title),
	m_description(_description),
	m_colors(_colors),
	m_isOnInsertionState(false)
{
	setMinSize(QSizeF(50,32));
    setSize(QSizeF(DEFAULT_WIDTH, DEFAULT_HEIGHT));
}

void CardShape::setUuid(const QString& _uuid)
{
	if (m_uuid != _uuid) {
		prepareGeometryChange();
		m_uuid = _uuid;
		emit contentsChanged();
	}
}

QString CardShape::uuid() const
{
	return m_uuid;
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

void CardShape::setColors(const QString& _colors)
{
	if (m_colors != _colors) {
		prepareGeometryChange();
		m_colors = _colors;
		emit contentsChanged();
	}
}

QString CardShape::colors() const
{
	return m_colors;
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

void CardShape::paint(QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget)
{
	Q_UNUSED(_option);
	Q_UNUSED(_widget);

	adjustSize();

	const QPalette palette = QApplication::palette();

	//
	// Рисуем фон
	//
	_painter->setPen(palette.dark().color());
	//
	// ... заданным цветом, если он задан
	//
	if (!m_colors.isEmpty()) {
		_painter->setBrush(QColor(m_colors.split(";").first()));
	}
	//
	// ... или стандартным цветом
	//
	else {
        if (m_cardType == TypeFolder) {
            _painter->setBrush(palette.alternateBase());
        } else {
			_painter->setBrush(palette.base());
        }
	}
	_painter->drawRect(boundingRect());
	QTextOption textoption;
	textoption.setAlignment(Qt::AlignTop | Qt::AlignLeft);

	//
	// Рисуем иконку
	//
    const int ICON_SIZE = _painter->fontMetrics().height() + 4;
	const QRect iconRect(7, 5, ICON_SIZE, ICON_SIZE);
	QPixmap icon;
	switch (m_cardType) {
		default:
		case TypeScene: icon.load(":/Graphics/Icons/scene.png"); break;
		case TypeScenesGroup: icon.load(":/Graphics/Icons/scene_group.png"); break;
		case TypeFolder: icon.load(":/Graphics/Icons/folder.png"); break;
	}
	QIcon iconColorized(icon);
	QColor iconColor = palette.text().color();
	ImageHelper::setIconColor(iconColorized, iconRect.size(), iconColor);
	icon = iconColorized.pixmap(iconRect.size());
	_painter->drawPixmap(iconRect, icon.scaled(iconRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

	//
	// Рисуем заголовок
	//
	textoption.setWrapMode(QTextOption::NoWrap);
	_painter->setFont(Shape::basicFont());
	QFont font = _painter->font();
	font.setBold(true);
	_painter->setFont(font);
	_painter->setPen(palette.text().color());
	const int TITLE_HEIGHT = _painter->fontMetrics().height();
	const QRectF titleRect(iconRect.right() + 7, 9, size().width() - iconRect.right() - 7 - 9, TITLE_HEIGHT);
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
	// Если это группирующий элемент, рисуем декорацию вложения
	//
	if (cardType() == TypeScenesGroup
		|| cardType() == TypeFolder) {
		const QSizeF fullDescriptionRectSize = TextUtils::textRect(description(), _painter->font(), descriptionRect.width(), textoption);

		//
		// Если на карточке есть свободное место для отрисовки декорации
		//
		if (fullDescriptionRectSize.height() < descriptionRect.height()) {
			const QRectF childsRect(
				descriptionRect.left() - 6,
				descriptionRect.top() + fullDescriptionRectSize.height() + spacing,
				descriptionRect.width() + 12,
				descriptionRect.height() - fullDescriptionRectSize.height() - spacing);

			//
			// Рисуем линию отделяющую детей
			//
			QPointF left = childsRect.topLeft();
			QPointF right = childsRect.topRight();
			if (m_cardType == TypeScenesGroup) {
				const int SCENE_GROUP_MARGIN = 12;
				left.setX(left.x() + SCENE_GROUP_MARGIN);
				right.setX(right.x() - SCENE_GROUP_MARGIN);
			}
			_painter->setPen(palette.dark().color());
			_painter->drawLine(left, right);

			if (m_isOnInsertionState) {
				//
				// TODO: new design
				//
				_painter->fillRect(childsRect, QApplication::palette().highlight());
			}
		}
	}

	//
	// Рисуем дополнительные цвета
	//
	if (!m_colors.isEmpty()) {
		QStringList colorsNamesList = m_colors.split(";", QString::SkipEmptyParts);
		colorsNamesList.removeFirst();
		//
		// ... если они есть
		//
		if (!colorsNamesList.isEmpty()) {
			//
			// Выссчитываем ширину занимаемую одним цветом
			//
			const qreal colorRectWidth = boundingRect().width() / colorsNamesList.size();
			QRectF colorRect(0, boundingRect().height() - ADDITIONAL_COLORS_HEIGHT, colorRectWidth, ADDITIONAL_COLORS_HEIGHT);
			for (const QString& colorName : colorsNamesList) {
				_painter->fillRect(colorRect, QColor(colorName));
				colorRect.moveLeft(colorRect.right());
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
