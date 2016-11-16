#ifndef CHARACTERSNAVIGATORITEMDELEGATE_H
#define CHARACTERSNAVIGATORITEMDELEGATE_H

#include <QStyledItemDelegate>


namespace UserInterface
{
	class CharactersNavigatorItemWidget;

	/**
	 * @brief Делегат для отрисовки элементов навигатора
	 */
	class CharactersNavigatorItemDelegate : public QStyledItemDelegate
	{
		Q_OBJECT

	public:
		explicit CharactersNavigatorItemDelegate(QObject* _parent = 0);
		~CharactersNavigatorItemDelegate();

//		void paint (QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const;
		QSize sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const;

	private:
		/**
		 * @brief Виджет для отрисовки элементов навигатора
		 */
		CharactersNavigatorItemWidget* m_itemWidget;
	};
}

#endif // CHARACTERSNAVIGATORITEMDELEGATE_H
