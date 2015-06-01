#include "qlightboxinputdialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>


QString QLightBoxInputDialog::getText(QWidget* _parent, const QString& _title, const QString& _label, const QString& _text)
{
	QLightBoxInputDialog dialog(_parent);
	dialog.setWindowTitle(_title);
	dialog.m_label->setText(_label);
	dialog.m_textInput->setText(_text);
	dialog.m_buttons->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	QString result;
	if (dialog.exec() == QLightBoxDialog::Accepted) {
		result = dialog.m_textInput->text();
	}
	return result;
}

QLightBoxInputDialog::QLightBoxInputDialog(QWidget* _parent) :
	QLightBoxDialog(_parent),
	m_label(new QLabel(this)),
	m_textInput(new QLineEdit(this)),
	m_buttons(new QDialogButtonBox(this))
{
	initView();
	initConnections();
}

void QLightBoxInputDialog::initView()
{
	m_textInput->setMinimumWidth(300);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(m_label);
	layout->addWidget(m_textInput);
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
	return m_textInput;
}

