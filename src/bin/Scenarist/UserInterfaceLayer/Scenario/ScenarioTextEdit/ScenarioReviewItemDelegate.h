#ifndef SCENARIOREVIEWITEMDELEGATE_H
#define SCENARIOREVIEWITEMDELEGATE_H

#include <QStyledItemDelegate>


namespace UserInterface
{
	/**
	 * @brief Делегат для отрисовки элементов панели рецензирования
	 */
	class ScenarioReviewItemDelegate : public QStyledItemDelegate
	{
		Q_OBJECT

	public:
		explicit ScenarioReviewItemDelegate(QObject* _parent = 0);

		void paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const;
		QSize sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const;
	};
}

#endif // SCENARIOREVIEWITEMDELEGATE_H
