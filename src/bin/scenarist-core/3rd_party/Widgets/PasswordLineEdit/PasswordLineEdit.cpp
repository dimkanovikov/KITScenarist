/*
* Copyright (C) 2016 Alexey Polushkin, armijo38@yandex.ru
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation; either
* version 3 of the License, or any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* Full license: http://dimkanovikov.pro/license/GPLv3
*/

#include "PasswordLineEdit.h"
#include "../FlatButton/FlatButton.h"

#include <QStyle>

PasswordLineEdit::PasswordLineEdit(QWidget* _parent) :
	QLineEdit(_parent),
	m_eye(new FlatButton(this))
{
	m_eye->setIcons(QIcon(":Graphics/Icons/eye-off.png"));
	m_eye->setIconSize(QSize(15, 15));
	m_eye->setCursor(Qt::ArrowCursor);
	m_eye->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	m_eye->setVisible(true);
	connect(m_eye, &QToolButton::clicked, this, &PasswordLineEdit::eyeClicked);

	m_isAsterisk = true;
	QLineEdit::setEchoMode(QLineEdit::Password);
}

void PasswordLineEdit::resetAsterisk()
{
	m_isAsterisk = false;
	eyeClicked();
}

void PasswordLineEdit::resizeEvent(QResizeEvent *_event)
{
	QLineEdit::resizeEvent(_event);

	//
	// Корректируем положение кнопки
	//
	const QSize eyeSizeHint = m_eye->sizeHint();
	const int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

	m_eye->move(
		rect().right() - frameWidth - eyeSizeHint.width(),
		(rect().bottom() - eyeSizeHint.height()) / 2
		);
}

void PasswordLineEdit::eyeClicked()
{
	if (m_isAsterisk) {
		m_eye->setIcons(QIcon(":Graphics/Icons/eye.png"));
		QLineEdit::setEchoMode(QLineEdit::Normal);
	} else {
		m_eye->setIcons(QIcon(":Graphics/Icons/eye-off.png"));
		QLineEdit::setEchoMode(QLineEdit::Password);
	}
	m_isAsterisk = !m_isAsterisk;
}
