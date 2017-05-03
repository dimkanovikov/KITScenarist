#include "ResearchNavigatorItemDelegate.h"

#include <3rd_party/Helpers/ImageHelper.h>

#include <QPainter>

namespace {
    const int ICON_SIZE = 20;
    const int TOP_MARGIN = 8;
    const int BOTTOM_MARGIN = 8;
    const int ITEMS_SPACING = 8;
}

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
    QStyleOptionViewItem opt = _option;
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
    // ... если это родитель верхнего уровня
    //
    if (!_index.parent().isValid()) {
        headerFont.setBold(true);
    }
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
        if(opt.features.testFlag(QStyleOptionViewItem::Alternate))
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
    const int RIGHT_MARGIN = 12;
	//
	// ... фон
	//
	_painter->fillRect(opt.rect, backgroundBrush);
    //
    // ... разделитель
    //
    QPoint left = opt.rect.bottomLeft();
    left.setX(0);
    _painter->setPen(QPen(opt.palette.dark(), 0.5));
    _painter->drawLine(left, opt.rect.bottomRight());
	//
	// Меняем координаты, чтобы рисовать было удобнее
	//
	_painter->translate(opt.rect.topLeft());
	//
	// ... иконка
    //
    const QRect iconRect(0, TOP_MARGIN, ICON_SIZE, ICON_SIZE);
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
        TOP_MARGIN,
		opt.rect.width() - iconRect.right() - ITEMS_SPACING - RIGHT_MARGIN,
        ICON_SIZE
		);
	QString header = _index.data(Qt::DisplayRole).toString();
	header = _painter->fontMetrics().elidedText(header, Qt::ElideRight, headerRect.width());
    _painter->drawText(headerRect, Qt::AlignLeft | Qt::AlignVCenter, header);

	_painter->restore();
}

QSize ResearchNavigatorItemDelegate::sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	Q_UNUSED(_option);
	Q_UNUSED(_index);

    const int height = TOP_MARGIN + ICON_SIZE + BOTTOM_MARGIN;
	const int width = 50;
	return QSize(width, height);
}
