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

#include "SideSlideBackgroundDecorator.h"

#include <QPainter>

using WAF::SideSlideBackgroundDecorator;


SideSlideBackgroundDecorator::SideSlideBackgroundDecorator(QWidget* _parent) :
	QWidget(_parent)
{
	resize(maximumSize());

	m_timeline.setDuration(300);
	m_timeline.setUpdateInterval(80);
	m_timeline.setCurveShape(QTimeLine::EaseInOutCurve);
	m_timeline.setStartFrame(0);
	m_timeline.setEndFrame(100);

	m_decorationColor = QColor(0, 0, 0, 0);

	//
	// Анимируем затемнение/осветление
	//
	connect(&m_timeline, &QTimeLine::frameChanged, [=](int _value){
		m_decorationColor = QColor(0, 0, 0, _value);
		repaint();
	});

	//
	// После того, как осветлили фон, скрываем себя
	//
	connect(&m_timeline, &QTimeLine::finished, [=](){
		if (m_timeline.currentFrame() == 0) {
			hide();
		}
	});
}

void SideSlideBackgroundDecorator::grabParent()
{
#ifdef Q_OS_ANDROID
	//
	// В андройде Qt не умеет рисовать прозрачные виджеты https://bugreports.qt.io/browse/QTBUG-43635
	// поэтому сохранем картинку с изображением подложки
	//
	m_background = parentWidget()->grab();
#endif
}

void SideSlideBackgroundDecorator::decorate(bool _dark)
{
	if (m_timeline.state() == QTimeLine::Running) {
		m_timeline.stop();
	}

	m_timeline.setDirection(_dark ? QTimeLine::Forward : QTimeLine::Backward);
	m_timeline.start();
}

void SideSlideBackgroundDecorator::paintEvent(QPaintEvent* _event)
{
	QPainter painter(this);
	painter.drawPixmap(0, 0, m_background);
	painter.fillRect(rect(), m_decorationColor);

	QWidget::paintEvent(_event);
}

void SideSlideBackgroundDecorator::mousePressEvent(QMouseEvent *_event)
{
	emit clicked();

	QWidget::mousePressEvent(_event);
}

