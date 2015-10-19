#ifndef COMBOBOXITEMDELEGATE_H
#define COMBOBOXITEMDELEGATE_H

#include <QStyledItemDelegate>


class ComboBoxItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit ComboBoxItemDelegate(QObject *parent = 0, QAbstractItemModel* model = 0, bool _editable = false);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
	QAbstractItemModel* m_model;
	bool m_editable;
};

#endif // COMBOBOXITEMDELEGATE_H
