#include "qlightboxinputdialog.h"

#include <3rd_party/Helpers/ScrollerHelper.h>
#include <3rd_party/Widgets/SimpleTextEditor/SimpleTextEditorWidget.h>

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QVBoxLayout>


QString QLightBoxInputDialog::getText(QWidget* _parent, const QString& _title, const QString& _label, const QString& _text)
{
	QLightBoxInputDialog dialog(_parent);
	dialog.setWindowTitle(_title);
	dialog.m_label->setText(_label);
	dialog.m_lineEdit->setText(_text);
	dialog.m_textEdit->hide();
	dialog.m_listWidget->hide();

	QString result;
	if (dialog.exec() == QLightBoxDialog::Accepted) {
		result = dialog.m_lineEdit->text();
	}
	return result;
}

QString QLightBoxInputDialog::getLongText(QWidget* _parent, const QString& _title, const QString& _label, const QString& _text)
{
	QLightBoxInputDialog dialog(_parent);
	dialog.setWindowTitle(_title);
	dialog.m_label->setText(_label);
	dialog.m_lineEdit->hide();
	dialog.m_textEdit->setPlainText(_text);
	dialog.m_listWidget->hide();

	QString result;
	if (dialog.exec() == QLightBoxDialog::Accepted) {
		result = dialog.m_textEdit->toPlainText();
	}
	return result;
}

QString QLightBoxInputDialog::getItem(QWidget* _parent, const QString& _title, const QStringList& _items, const QString& _selectedItem)
{
	const bool STRETCH_LIST_WIDGET = true;
	QLightBoxInputDialog dialog(_parent, STRETCH_LIST_WIDGET);
	dialog.setWindowTitle(_title);
	dialog.m_label->hide();
	dialog.m_lineEdit->hide();
	dialog.m_textEdit->hide();
	//
	// Наполняем список переключателями
	//
	{
		QListWidgetItem* item;
		foreach (const QString& itemText, _items) {
			item = new QListWidgetItem(dialog.m_listWidget);
			dialog.m_listWidget->setItemWidget(item, new QRadioButton(itemText));
		}
		const int FIRST_ITEM = 0;
		const int ITEM_FOR_SELECT = _selectedItem.isEmpty() ? FIRST_ITEM : _items.indexOf(_selectedItem);
		QListWidgetItem* itemForSelect = dialog.m_listWidget->item(ITEM_FOR_SELECT);
		if (QRadioButton* radioButton = qobject_cast<QRadioButton*>(dialog.m_listWidget->itemWidget(itemForSelect))) {
			radioButton->setChecked(true);
		}
	}

	QString result;
	if (dialog.exec() == QLightBoxDialog::Accepted) {
		for (int itemIndex = 0; itemIndex < dialog.m_listWidget->count(); ++itemIndex) {
			QListWidgetItem* item = dialog.m_listWidget->item(itemIndex);
			if (QRadioButton* radioButton = qobject_cast<QRadioButton*>(dialog.m_listWidget->itemWidget(item))) {
				if (radioButton->isChecked()) {
					result = radioButton->text();
					break;
				}
			}
		}
	}
	return result;
}

QLightBoxInputDialog::QLightBoxInputDialog(QWidget* _parent, bool _isContentStretchable) :
	QLightBoxDialog(_parent, true, _isContentStretchable),
	m_label(new QLabel(this)),
	m_lineEdit(new QLineEdit(this)),
	m_textEdit(new SimpleTextEditorWidget(this)),
	m_listWidget(new QListWidget(this)),
	m_buttons(new QDialogButtonBox(this))
{
	initView();
	initConnections();
}

void QLightBoxInputDialog::initView()
{
	m_lineEdit->setMinimumWidth(500);

	m_textEdit->setToolbarVisible(false);
	m_textEdit->setMinimumWidth(500);
	m_textEdit->setMinimumHeight(400);

	m_listWidget->setProperty("dialog-container", true);
	m_listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
#ifdef MOBILE_OS
	ScrollerHelper::addScroller(m_listWidget);
#endif

	m_buttons->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	foreach (QAbstractButton* button, m_buttons->buttons())	{
		button->setProperty("flat", true);

#ifdef MOBILE_OS
		//
		// Для мобильных делаем кнопки в верхнем регистре и убераем ускорители
		//
		button->setText(button->text().toUpper().remove("&"));
#endif
	}

	QVBoxLayout* layout = new QVBoxLayout;
#ifdef MOBILE_OS
	layout->setContentsMargins(QMargins());
#endif
	layout->addWidget(m_label);
	layout->addWidget(m_lineEdit);
	layout->addWidget(m_textEdit);
	layout->addWidget(m_listWidget);
	layout->addWidget(m_buttons);
	setLayout(layout);

	QLightBoxDialog::initView();
}

void QLightBoxInputDialog::initConnections()
{
	connect(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QLightBoxDialog::initConnections();
}

QWidget* QLightBoxInputDialog::focusedOnExec() const
{
	QWidget* focusTarget = m_label;
	if (m_lineEdit->isVisible()) {
		focusTarget = m_lineEdit;
	} else if (m_textEdit->isVisible()) {
		focusTarget = m_textEdit;
	}

	return focusTarget;
}

