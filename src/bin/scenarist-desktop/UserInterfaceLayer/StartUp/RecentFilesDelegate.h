#ifndef RECENTFILESDELEGATE_H
#define RECENTFILESDELEGATE_H

#include <QStyledItemDelegate>

namespace UserInterface
{
	class RecentFileWidget;

	/**
	 * @brief Делегат для отрисовки файла проекта в списке недавно открытых
	 */
	class RecentFilesDelegate : public QStyledItemDelegate
	{
		Q_OBJECT

	public:
		explicit RecentFilesDelegate(QObject* _parent = 0);
		~RecentFilesDelegate();

		void paint (QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const;
		QSize sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const;

	private:
		/**
		 * @brief Виджет для отрисовки
		 */
		RecentFileWidget* m_fileWidget;

	};
}

#endif // RECENTFILESDELEGATE_H
