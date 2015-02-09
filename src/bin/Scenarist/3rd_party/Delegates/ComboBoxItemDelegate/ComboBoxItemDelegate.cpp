#include "ComboBoxItemDelegate.h"

#include <QComboBox>


ComboBoxItemDelegate::ComboBoxItemDelegate(QObject *parent, QAbstractItemModel* model, bool _editable) :
	QStyledItemDelegate(parent),
	m_model(model),
	m_editable(_editable)
{
}

QWidget* ComboBoxItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	Q_UNUSED(option);
	Q_UNUSED(index);

	QComboBox* editor = new QComboBox(parent);
	editor->setModel(m_model);
	editor->setEditable(m_editable);
	editor->setInsertPolicy(QComboBox::NoInsert);

	return editor;
}

void ComboBoxItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QString value = index.model()->data(index, Qt::EditRole).toString();

	QComboBox* comboBox = static_cast<QComboBox*>(editor);
	comboBox->setCurrentText(value);
}

void ComboBoxItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QComboBox *comboBox = static_cast<QComboBox*>(editor);
	QString value = comboBox->currentText();
	model->setData(index, value, Qt::EditRole);
}

void ComboBoxItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}
