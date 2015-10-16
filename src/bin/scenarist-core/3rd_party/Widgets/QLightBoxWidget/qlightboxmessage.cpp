#include "qlightboxmessage.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVariant>
#include <QVBoxLayout>

namespace {
	/**
	 * @brief Размер иконки
	 */
	const QSize ICON_PIXMAP_SIZE(48, 48);
}


QDialogButtonBox::StandardButton QLightBoxMessage::critical(QWidget* _parent, const QString& _title,
	const QString& _text, QDialogButtonBox::StandardButtons _buttons,
	QDialogButtonBox::StandardButton _defaultButton)
{
	return message(_parent, _title, _text, QStyle::SP_MessageBoxCritical, _buttons, _defaultButton);
}

QDialogButtonBox::StandardButton QLightBoxMessage::information(QWidget* _parent, const QString& _title,
	const QString& _text, QDialogButtonBox::StandardButtons _buttons,
	QDialogButtonBox::StandardButton _defaultButton)
{
	return message(_parent, _title, _text, QStyle::SP_MessageBoxInformation, _buttons, _defaultButton);
}

QDialogButtonBox::StandardButton QLightBoxMessage::question(QWidget* _parent, const QString& _title,
	const QString& _text, QDialogButtonBox::StandardButtons _buttons,
	QDialogButtonBox::StandardButton _defaultButton)
{
	return message(_parent, _title, _text, QStyle::SP_MessageBoxQuestion, _buttons, _defaultButton);
}

QDialogButtonBox::StandardButton QLightBoxMessage::warning(QWidget* _parent, const QString& _title,
	const QString& _text, QDialogButtonBox::StandardButtons _buttons,
	QDialogButtonBox::StandardButton _defaultButton)
{
	return message(_parent, _title, _text, QStyle::SP_MessageBoxWarning, _buttons, _defaultButton);
}

QDialogButtonBox::StandardButton QLightBoxMessage::message(QWidget* _parent, const QString& _title,
	const QString& _text, QStyle::StandardPixmap _pixmap, QDialogButtonBox::StandardButtons _buttons,
	QDialogButtonBox::StandardButton _defaultButton)
{
	QLightBoxMessage message(_parent);
	message.setWindowTitle(_title);
	message.m_icon->setPixmap(message.style()->standardIcon(_pixmap).pixmap(ICON_PIXMAP_SIZE));
	message.m_text->setText(_text);
	message.m_buttons->setStandardButtons(_buttons);
	if (_buttons.testFlag(_defaultButton)) {
		message.m_buttons->button(_defaultButton)->setDefault(true);
	}
#ifndef Q_OS_ANDROID
	foreach (QAbstractButton* button, message.m_buttons->buttons())	{
		button->setProperty("flat", true);
	}
#endif

	return (QDialogButtonBox::StandardButton)message.exec();
}

QLightBoxMessage::QLightBoxMessage(QWidget* _parent) :
	QLightBoxDialog(_parent),
	m_icon(new QLabel(this)),
	m_text(new QLabel(this)),
	m_buttons(new QDialogButtonBox(this))
{
	initView();
	initConnections();
}

void QLightBoxMessage::initView()
{
	m_icon->setFixedSize(ICON_PIXMAP_SIZE);
	m_text->setWordWrap(true);
	m_text->setMinimumWidth(300);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->addWidget(m_icon, 0, Qt::AlignLeft | Qt::AlignTop);
	topLayout->addWidget(m_text, 1);
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addLayout(topLayout);
	layout->addWidget(m_buttons);
	setLayout(layout);

	setMinimumWidth(500);

	QLightBoxDialog::initView();
}

void QLightBoxMessage::initConnections()
{
	connect(m_buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(aboutClicked(QAbstractButton*)));

	QLightBoxDialog::initConnections();
}

void QLightBoxMessage::aboutClicked(QAbstractButton* _button)
{
	done(m_buttons->standardButton(_button));
}

