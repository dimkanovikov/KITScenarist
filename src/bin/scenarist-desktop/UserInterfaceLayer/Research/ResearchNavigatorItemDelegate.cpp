#include "ResearchNavigatorItemDelegate.h"

#include <3rd_party/Helpers/ImageHelper.h>

#include <QPainter>

using UserInterface::ResearchNavigatorItemDelegate;


ResearchNavigatorItemDelegate::ResearchNavigatorItemDelegate(QObject* _parent) :
	QStyledItemDelegate(_parent)
{
}

void ResearchNavigatorItemDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	//
	// Получим настройки стиля
	//
	QStyleOptionViewItemV4 opt = _option;
	initStyleOption(&opt, _index);

	//
	// Рисуем ручками
	//
	_painter->save();
	_painter->setRenderHint(QPainter::Antialiasing, true);

	//
	// Определим кисти и шрифты
	//
	QBrush backgroundBrush = opt.palette.background();
	QBrush textBrush = opt.palette.text();
	QFont headerFont = opt.font;
	//
	// ... для выделенных элементов
	//
	if(opt.state.testFlag(QStyle::State_Selected))
	{
		backgroundBrush = opt.palette.highlight();
		textBrush = opt.palette.highlightedText();
	}
	//
	// ... для остальных
	//
	else
	{
		//
		// Реализация альтернативных цветов в представлении
		//
		if(opt.features.testFlag(QStyleOptionViewItemV2::Alternate))
		{
			backgroundBrush = opt.palette.alternateBase();
			textBrush = opt.palette.windowText();
		}
		else
		{
			backgroundBrush = opt.palette.base();
			textBrush = opt.palette.windowText();
		}
	}

	//
	// Рисуем
	//
	const int MARGIN = 2;
	const int RIGHT_MARGIN = 12;
	const int ITEMS_SPACING = 4;
	const int TEXT_LINE_HEIGHT = _painter->fontMetrics().height();
	//
	// ... фон
	//
	_painter->fillRect(opt.rect, backgroundBrush);
	//
	// Меняем координаты, чтобы рисовать было удобнее
	//
	_painter->translate(opt.rect.topLeft());
	//
	// ... иконка
	//
	const int iconSize =  20;
	const int iconTopMargin = MARGIN;
	const QRect iconRect(MARGIN, iconTopMargin, iconSize, iconSize);
	QPixmap icon = _index.data(Qt::DecorationRole).value<QPixmap>();
	QIcon iconColorized(icon);
	QColor iconColor = textBrush.color();
	ImageHelper::setIconColor(iconColorized, iconRect.size(), iconColor);
	icon = iconColorized.pixmap(iconRect.size());
	_painter->drawPixmap(iconRect, icon);
	//
	// ... заголовок
	//
	_painter->setPen(textBrush.color());
	_painter->setFont(headerFont);
	const QRect headerRect(
		iconRect.right() + ITEMS_SPACING,
		MARGIN,
		opt.rect.width() - iconRect.right() - ITEMS_SPACING - RIGHT_MARGIN,
		TEXT_LINE_HEIGHT
		);
	QString header = _index.data(Qt::DisplayRole).toString();
	header = _painter->fontMetrics().elidedText(header, Qt::ElideRight, headerRect.width());
	_painter->drawText(headerRect, header);

	_painter->restore();
}

QSize ResearchNavigatorItemDelegate::sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	Q_UNUSED(_option);
	Q_UNUSED(_index);

	const int margins = 3 + 3;
	const int height = _option.fontMetrics.height() + margins;
	const int width = 50;
	return QSize(width, height);
}
