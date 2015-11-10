#ifndef RESEARCHNAVIGATORITEMDELEGATE_H
#define RESEARCHNAVIGATORITEMDELEGATE_H

#include <QStyledItemDelegate>


namespace UserInterface
{
	/**
	 * @brief Делегат для отрисовки элементов навигатора
	 */
	class ResearchNavigatorItemDelegate : public QStyledItemDelegate
	{
		Q_OBJECT

	public:
		explicit ResearchNavigatorItemDelegate(QObject* _parent = 0);

		void paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const;
		QSize sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const;
	};
}

#endif // RESEARCHNAVIGATORITEMDELEGATE_H
