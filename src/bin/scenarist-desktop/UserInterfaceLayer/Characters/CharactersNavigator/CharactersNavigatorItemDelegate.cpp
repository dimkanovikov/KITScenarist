#include "CharactersNavigatorItemDelegate.h"
#include "CharactersNavigatorItemWidget.h"

#include <QApplication>
#include <QPainter>

using UserInterface::CharactersNavigatorItemDelegate;
using UserInterface::CharactersNavigatorItemWidget;


CharactersNavigatorItemDelegate::CharactersNavigatorItemDelegate(QObject* _parent) :
	QStyledItemDelegate(_parent),
	m_itemWidget(new CharactersNavigatorItemWidget)
{
}

CharactersNavigatorItemDelegate::~CharactersNavigatorItemDelegate()
{
	delete m_itemWidget;
}

//void CharactersNavigatorItemDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const
//{
//	QVariant name = _index.data(Qt::DisplayRole);

//	//
//	// Установим в виджет данные
//	//
//	// ... заголовок
//	//
//	m_itemWidget->setName(name.toString());

//	//
//	// Получим настройки стиля
//	//
//	QStyleOptionViewItem opt = _option;
//	initStyleOption(&opt, _index);

//	//
//	// Настраиваем размер области отрисовки
//	//
//	m_itemWidget->resize(opt.rect.size());

//	//
//	// Настраиваем палитру отображения
//	//
//	QPalette palette;

//	//
//	// Для выделенного объекта
//	//
//	if(opt.state.testFlag(QStyle::State_Selected))
//	{
//		palette.setBrush(QPalette::Window, qApp->palette().brush(QPalette::Highlight));
//		palette.setBrush(QPalette::WindowText, qApp->palette().brush(QPalette::HighlightedText));
//	}
//	//
//	// ... для остальных
//	//
//	else
//	{
//		//
//		// Реализация альтернативных цветов в представлении
//		//
//		if(opt.features.testFlag(QStyleOptionViewItem::Alternate))
//		{
//			palette.setBrush(QPalette::Window, qApp->palette().brush(QPalette::AlternateBase));
//			palette.setBrush(QPalette::WindowText, qApp->palette().brush(QPalette::WindowText));
//		}
//		else
//		{
//			palette.setBrush(QPalette::Window, qApp->palette().brush(QPalette::Base));
//			palette.setBrush(QPalette::WindowText, qApp->palette().brush(QPalette::WindowText));
//		}
//	}

//	//
//	// Устанавливаем палитру виджету и всем его детям
//	//
//	m_itemWidget->setPalette(palette);
//	foreach (QWidget* itemChild, m_itemWidget->findChildren<QWidget*>()) {
//		itemChild->setPalette(palette);
//	}

//	//
//	// Отрисовываем виджет
//	//
//	_painter->save();
//	_painter->setRenderHint(QPainter::Antialiasing, true);
//	_painter->translate(opt.rect.topLeft());

//	m_itemWidget->render(_painter);

//	_painter->restore();
//}

QSize CharactersNavigatorItemDelegate::sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	Q_UNUSED(_option);
	Q_UNUSED(_index);

	return m_itemWidget->minimumSizeHint();
}
