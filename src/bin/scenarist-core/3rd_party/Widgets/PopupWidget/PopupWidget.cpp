/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  =========================================================================*/

// Qt includes
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QLayout>
#include <QPropertyAnimation>

// CTK includes
#include "PopupWidget_p.h"

// -------------------------------------------------------------------------
PopupWidgetPrivate::PopupWidgetPrivate(PopupWidget& object)
	:q_ptr(&object)
{
	this->EffectDuration = 300; // in ms
	this->ScrollAnimation = 0;
	this->PopupPixmapWidget = 0;
	// Geometry attributes
	this->Alignment = Qt::AlignJustify | Qt::AlignBottom;
	this->Orientations = Qt::Vertical;
	this->VerticalDirection = PopupWidget::TopToBottom;
	this->HorizontalDirection = Qt::LeftToRight;
}

// -------------------------------------------------------------------------
PopupWidgetPrivate::~PopupWidgetPrivate()
{
}

// -------------------------------------------------------------------------
void PopupWidgetPrivate::init()
{
	Q_Q(PopupWidget);

	this->PopupPixmapWidget = new QLabel(q);
	this->Frame = new QFrame(q);

	this->ScrollAnimation = new QPropertyAnimation(q, "effectGeometry", q);
	this->ScrollAnimation->setDuration(this->EffectDuration);
	QObject::connect(this->ScrollAnimation, SIGNAL(finished()),
		q, SLOT(onEffectFinished()));

	q->setEasingCurve(QEasingCurve::OutCubic);

	this->FramePage = new QWidget();
	QVBoxLayout* verticalLayout = new QVBoxLayout(this->FramePage);
	verticalLayout->setContentsMargins(0, 0, 0, 0);
	verticalLayout->addWidget(this->Frame);

	q->addWidget(FramePage);

	this->PopupPixmapWidgetPage = new QWidget();
	verticalLayout = new QVBoxLayout(this->PopupPixmapWidgetPage);
	verticalLayout->setContentsMargins(0, 0, 0, 0);
	verticalLayout->addWidget(this->PopupPixmapWidget);

	q->addWidget(this->PopupPixmapWidgetPage);
}

// -------------------------------------------------------------------------
QPropertyAnimation* PopupWidgetPrivate::currentAnimation()const
{
	return this->ScrollAnimation;
}

// -------------------------------------------------------------------------
bool PopupWidgetPrivate::isOpening()const
{
	return this->currentAnimation()->state() == QAbstractAnimation::Running &&
		this->currentAnimation()->direction() == QAbstractAnimation::Forward;
}

// -------------------------------------------------------------------------
bool PopupWidgetPrivate::isClosing()const
{
	return this->currentAnimation()->state() == QAbstractAnimation::Running &&
		this->currentAnimation()->direction() == QAbstractAnimation::Backward;
}

// -------------------------------------------------------------------------
bool PopupWidgetPrivate::wasClosing()const
{
	Q_Q(const PopupWidget);
	return qobject_cast<QAbstractAnimation*>(q->sender())->direction()
		== QAbstractAnimation::Backward;
}

// -------------------------------------------------------------------------
void PopupWidgetPrivate::setupPopupPixmapWidget()
{
	Q_Q(PopupWidget);
	this->PopupPixmapWidget->setAlignment(this->pixmapAlignment());
	QPixmap pixmap;

//     FramePage->grab();
	pixmap = FramePage->grab();
//     pixmap.fill(Qt::red);

	this->PopupPixmapWidget->setPixmap(pixmap);
}

// -------------------------------------------------------------------------
Qt::Alignment PopupWidgetPrivate::pixmapAlignment()const
{
	Qt::Alignment alignment;
	if (this->VerticalDirection == PopupWidget::TopToBottom)
	{
		alignment |= Qt::AlignBottom;
	}
	else// if (this->VerticalDirection == PopupWidget::BottomToTop)
	{
		alignment |= Qt::AlignTop;
	}

	if (this->HorizontalDirection == Qt::LeftToRight)
	{
		alignment |= Qt::AlignRight;
	}
	else// if (this->VerticalDirection == PopupWidget::BottomToTop)
	{
		alignment |= Qt::AlignLeft;
	}
	return alignment;
}

// -------------------------------------------------------------------------
QRect PopupWidgetPrivate::closedGeometry()const
{
	Q_Q(const PopupWidget);

	QRect openGeom = desiredOpenGeometry();

	if (this->Orientations & Qt::Vertical)
	{
		openGeom.setHeight(0);
	}

	if (this->Orientations & Qt::Horizontal)
	{
		openGeom.setWidth(0);
	}

	return openGeom;
}

// -------------------------------------------------------------------------
QRect PopupWidgetPrivate::desiredOpenGeometry()const
{
	Q_Q(const PopupWidget);
	QSize size = FramePage->size();
	if (!FramePage->testAttribute(Qt::WA_WState_Created))
	{
		size = FramePage->sizeHint();
	}
	return QRect(FramePage->pos(), size);

}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// Qt::FramelessWindowHint is required on Windows for Translucent background
// Qt::Toolip is preferred to Qt::Popup as it would close itself at the first
// click outside the widget (typically a click in the BaseWidget)
PopupWidget::PopupWidget(QWidget* parentWidget)
: Superclass(parentWidget)
, d_ptr(new PopupWidgetPrivate(*this))
{
	Q_D(PopupWidget);
	d->init();
}

// -------------------------------------------------------------------------
PopupWidget::PopupWidget(PopupWidgetPrivate* pimpl, QWidget* parentWidget)
: Superclass(parentWidget)
, d_ptr(pimpl)
{
}

// -------------------------------------------------------------------------
PopupWidget::~PopupWidget()
{
}

QFrame* PopupWidget::frame()const
{
	Q_D(const PopupWidget);
	return qobject_cast<QFrame*>(d->Frame);
}
// -------------------------------------------------------------------------
int PopupWidget::effectDuration()const
{
	Q_D(const PopupWidget);
	return d->EffectDuration;
}

// -------------------------------------------------------------------------
void PopupWidget::setEffectDuration(int duration)
{
	Q_D(PopupWidget);
	d->EffectDuration = duration;
	d->ScrollAnimation->setDuration(d->EffectDuration);
}

// -------------------------------------------------------------------------
QEasingCurve::Type PopupWidget::easingCurve()const
{
	Q_D(const PopupWidget);
	return d->ScrollAnimation->easingCurve().type();
}

// -------------------------------------------------------------------------
void PopupWidget::setEasingCurve(QEasingCurve::Type easingCurve)
{
	Q_D(PopupWidget);
	d->ScrollAnimation->setEasingCurve(easingCurve);
}

// -------------------------------------------------------------------------
Qt::Alignment PopupWidget::alignment()const
{
	Q_D(const PopupWidget);
	return d->Alignment;
}

// -------------------------------------------------------------------------
void PopupWidget::setAlignment(Qt::Alignment alignment)
{
	Q_D(PopupWidget);
	d->Alignment = alignment;
}

// -------------------------------------------------------------------------
Qt::Orientations PopupWidget::orientation()const
{
	Q_D(const PopupWidget);
	return d->Orientations;
}

// -------------------------------------------------------------------------
void PopupWidget::setOrientation(Qt::Orientations orientations)
{
	Q_D(PopupWidget);
	d->Orientations = orientations;
}

// -------------------------------------------------------------------------
PopupWidget::VerticalDirection PopupWidget::verticalDirection()const
{
	Q_D(const PopupWidget);
	return d->VerticalDirection;
}

// -------------------------------------------------------------------------
void PopupWidget::setVerticalDirection(PopupWidget::VerticalDirection verticalDirection)
{
	Q_D(PopupWidget);
	d->VerticalDirection = verticalDirection;
}

// -------------------------------------------------------------------------
Qt::LayoutDirection PopupWidget::horizontalDirection()const
{
	Q_D(const PopupWidget);
	return d->HorizontalDirection;
}

// -------------------------------------------------------------------------
void PopupWidget::setHorizontalDirection(Qt::LayoutDirection horizontalDirection)
{
	Q_D(PopupWidget);
	d->HorizontalDirection = horizontalDirection;
}

// -------------------------------------------------------------------------
void PopupWidget::onEffectFinished()
{
	Q_D(PopupWidget);
	if (d->wasClosing())
	{
//         this->hide();
		emit this->popupOpened(false);
	}
	else
	{
		this->setCurrentWidget(d->FramePage);
		emit this->popupOpened(true);
	}
}

// --------------------------------------------------------------------------
void PopupWidget::showPopup()
{
	Q_D(PopupWidget);

	if (this->size().width() * this->size().height() != 0 &&
		d->currentAnimation()->state() == QAbstractAnimation::Stopped)
	{
		return;
	}

	// If the layout has never been activated, the widget doesn't know its
	// minSize/maxSize and we then wouldn't know what's its true geometry.
	if (this->layout() && !this->testAttribute(Qt::WA_WState_Created))
	{
		this->layout()->activate();
	}

	QRect openGeometry = d->desiredOpenGeometry();
	QRect closedGeometry = d->closedGeometry();

	d->currentAnimation()->setDirection(QAbstractAnimation::Forward);

	if (d->currentAnimation()->state() == QAbstractAnimation::Stopped) {
		d->ScrollAnimation->setStartValue(closedGeometry);
		d->ScrollAnimation->setEndValue(openGeometry);
		d->setupPopupPixmapWidget();
		this->setCurrentWidget(d->PopupPixmapWidgetPage);
		this->show();
	}

	switch (d->currentAnimation()->state())
	{
	case QAbstractAnimation::Stopped:
		d->currentAnimation()->start();
		break;
	case QAbstractAnimation::Paused:
		d->currentAnimation()->resume();
		break;
	default:
	case QAbstractAnimation::Running:
		break;
	}
}

// --------------------------------------------------------------------------
void PopupWidget::hidePopup()
{
	Q_D(PopupWidget);

//	if (!this->isVisible() &&
//		d->currentAnimation()->state() == QAbstractAnimation::Stopped)
//	{
//		return;
//	}
	d->currentAnimation()->setDirection(QAbstractAnimation::Backward);

	QRect openGeometry = d->desiredOpenGeometry();
	QRect closedGeometry = d->closedGeometry();

	if (d->currentAnimation()->state() == QAbstractAnimation::Stopped) {
		d->ScrollAnimation->setStartValue(closedGeometry);
		d->ScrollAnimation->setEndValue(openGeometry);
		d->setupPopupPixmapWidget();
		this->setCurrentWidget(d->PopupPixmapWidgetPage);
	}

	switch (d->currentAnimation()->state())
	{
	case QAbstractAnimation::Stopped:
		d->currentAnimation()->start();
		break;
	case QAbstractAnimation::Paused:
		d->currentAnimation()->resume();
		break;
	default:
	case QAbstractAnimation::Running:
		break;
	}
}

// --------------------------------------------------------------------------
QRect PopupWidget::effectGeometry()const
{
	Q_D(const PopupWidget);
	return d->PopupPixmapWidget->geometry();
}

// --------------------------------------------------------------------------
void PopupWidget::setEffectGeometry(QRect newGeometry)
{
	Q_D(PopupWidget);
	//d->PopupPixmapWidget->setGeometry(newGeometry);
	//d->PopupPixmapWidget->setFixedSize(newGeometry.width(), newGeometry.height());
	this->setFixedSize(newGeometry.width(), newGeometry.height());
	d->PopupPixmapWidget->repaint();
}
