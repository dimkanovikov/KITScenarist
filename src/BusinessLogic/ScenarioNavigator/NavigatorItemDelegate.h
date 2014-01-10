#ifndef NAVIGATORITEMDELEGATE_H
#define NAVIGATORITEMDELEGATE_H

#include <QStyledItemDelegate>

class NavigatorItemWidget;


/**
 * @brief Делегат для отрисовки элементов навигатора
 */
class NavigatorItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit NavigatorItemDelegate(QObject* _parent = 0);
	~NavigatorItemDelegate();

	void paint (QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const;
	QSize sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const;

private:
	/**
	 * @brief Виджет для отрисовки элементов навигатора
	 */
	NavigatorItemWidget* m_itemWidget;
};

#endif // NAVIGATORITEMDELEGATE_H
