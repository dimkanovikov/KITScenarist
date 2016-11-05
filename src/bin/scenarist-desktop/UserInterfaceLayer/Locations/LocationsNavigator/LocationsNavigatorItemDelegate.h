#ifndef LOCATIONSNAVIGATORITEMDELEGATE_H
#define LOCATIONSNAVIGATORITEMDELEGATE_H

#include <QStyledItemDelegate>


namespace UserInterface
{
	class LocationsNavigatorItemWidget;

	/**
	 * @brief Делегат для отрисовки элементов навигатора
	 */
	class LocationsNavigatorItemDelegate : public QStyledItemDelegate
	{
		Q_OBJECT

	public:
		explicit LocationsNavigatorItemDelegate(QObject* _parent = 0);
		~LocationsNavigatorItemDelegate();

//		void paint (QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const;
		QSize sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const;

	private:
		/**
		 * @brief Виджет для отрисовки элементов навигатора
		 */
		LocationsNavigatorItemWidget* m_itemWidget;
	};
}

#endif // LOCATIONSNAVIGATORITEMDELEGATE_H
