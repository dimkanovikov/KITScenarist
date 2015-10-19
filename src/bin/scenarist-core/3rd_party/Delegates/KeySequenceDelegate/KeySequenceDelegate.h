#ifndef KEYSEQUENCEDELEGATE_H
#define KEYSEQUENCEDELEGATE_H

#include <QStyledItemDelegate>


class KeySequenceDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit KeySequenceDelegate(QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // KEYSEQUENCEDELEGATE_H
