/*
 * Copyright (C) 2015  Dimka Novikov, to@dimkanovikov.pro
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * Full license: https://github.com/dimkanovikov/WidgetAnimationFramework/blob/master/LICENSE
 */

#include "SlideBackgroundDecorator.h"

#include <QPainter>

using WAF::SlideBackgroundDecorator;


SlideBackgroundDecorator::SlideBackgroundDecorator(QWidget* _parent) :
	QWidget(_parent)
{
}

void SlideBackgroundDecorator::grabParent(const QSize& _size)
{
	resize(_size);
	m_background = QPixmap(_size);

//	m_background = parentWidget()->grab(QRect(QPoint(), _size));

	QPainter painter;
	painter.begin(&m_background);
	parentWidget()->render(&painter, QPoint(), QRegion(QRect(QPoint(), _size)));
	painter.end();
}

void SlideBackgroundDecorator::paintEvent(QPaintEvent* _event)
{
	QPainter painter(this);
	painter.drawPixmap(0, 0, m_background);

	QWidget::paintEvent(_event);
}

