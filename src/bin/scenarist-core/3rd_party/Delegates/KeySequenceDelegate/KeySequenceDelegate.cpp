#include "KeySequenceDelegate.h"

#include <QKeySequenceEdit>


KeySequenceDelegate::KeySequenceDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{
}

QWidget* KeySequenceDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	Q_UNUSED(option);
	Q_UNUSED(index);

	QKeySequenceEdit* editor = new QKeySequenceEdit(parent);

	return editor;
}

void KeySequenceDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	const QString value = index.model()->data(index, Qt::EditRole).toString();

	QKeySequenceEdit* keySequenceEdit = qobject_cast<QKeySequenceEdit*>(editor);
	keySequenceEdit->setKeySequence(QKeySequence(value));
}

void KeySequenceDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QKeySequenceEdit *keySequenceEdit = qobject_cast<QKeySequenceEdit*>(editor);
	const QString value = keySequenceEdit->keySequence().toString(QKeySequence::NativeText);
	model->setData(index, value, Qt::EditRole);
}

void KeySequenceDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(index);

	editor->setGeometry(option.rect);
}

