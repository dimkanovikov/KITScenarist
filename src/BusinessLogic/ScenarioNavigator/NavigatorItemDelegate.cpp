#include "NavigatorItemDelegate.h"

#include "NavigatorItemWidget.h"

#include <QApplication>
#include <QPainter>


NavigatorItemDelegate::NavigatorItemDelegate(QObject* _parent) :
	QStyledItemDelegate(_parent),
	m_itemWidget(new NavigatorItemWidget)
{
}

NavigatorItemDelegate::~NavigatorItemDelegate()
{
	delete m_itemWidget;
}

void NavigatorItemDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	QVariant icon = _index.data(Qt::DecorationRole);
	QVariant header = _index.data(Qt::DisplayRole);
	QVariant description = _index.data(Qt::ToolTipRole);
	QVariant timing = _index.data(Qt::UserRole);

	//
	// Установим в виджет данные
	//
	m_itemWidget->setIcon(icon.value<QPixmap>());
	m_itemWidget->setHeader(header.toString());
	m_itemWidget->setDescription(description.toString());
	m_itemWidget->setTiming(timing.toInt());

	//
	// Получим настройки стиля
	//
	QStyleOptionViewItemV4 opt = _option;
	initStyleOption(&opt, _index);

	//
	// Настраиваем размер области отрисовки
	//
	m_itemWidget->resize(opt.rect.size());

	//
	// Настраиваем палитру отображения
	//
	QPalette palette;

	//
	// Для выделенного объекта
	//
	if(opt.state.testFlag(QStyle::State_Selected))
	{
		palette.setBrush(QPalette::Window, qApp->palette().brush(QPalette::Highlight));
		palette.setBrush(QPalette::WindowText, qApp->palette().brush(QPalette::HighlightedText));
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
			palette.setBrush(QPalette::Window, qApp->palette().brush(QPalette::AlternateBase));
			palette.setBrush(QPalette::WindowText, qApp->palette().brush(QPalette::WindowText));
		}
		else
		{
			palette.setBrush(QPalette::Window, qApp->palette().brush(QPalette::Base));
			palette.setBrush(QPalette::WindowText, qApp->palette().brush(QPalette::WindowText));
		}
	}

	m_itemWidget->setPalette(palette);

	//
	// Отрисовываем термин
	//
	_painter->save();
	_painter->setRenderHint(QPainter::Antialiasing, true);
	_painter->translate(opt.rect.topLeft());

	m_itemWidget->render(_painter);

	_painter->restore();
}

QSize NavigatorItemDelegate::sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	Q_UNUSED(_option);
	Q_UNUSED(_index);

	return m_itemWidget->minimumSizeHint();
}
