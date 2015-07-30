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

#ifndef __PopupWidget_h
#define __PopupWidget_h

// Qt includes
#include <QEasingCurve>
#include <QFrame>
#include <QMetaType>
#include <QStackedWidget>

class PopupWidgetPrivate;

/// \ingroup Widgets
/// PopupWidget is a popup that opens under, above or on the side of
/// another widget (baseWidget() or its parent widget by default).
/// The children (widgets and layout) of the popup define of the content
/// of the popup. Different effects can be applied during the opening or
/// closing of the popup.
/// See ctkPopupWidget for an automatic control of its opening and closing.
/// \sa baseWidget(), animationEffect, ctkPopupWidget
class PopupWidget : public QStackedWidget
{
  Q_OBJECT

  Q_ENUMS(VerticalDirection)

  /// The property controls the \a animationEffect duration in ms.
  /// If the popup state (open or close) is being changed during the animation,
  /// the active animation is stopped and a new animation is being created from
  /// the current state (geometry, transparency...) to the new final state.
  /// Default to 333ms
  /// \sa effectDuration(), setEffectDuration(), animationEffect, easingCurve
  Q_PROPERTY( int effectDuration READ effectDuration WRITE setEffectDuration);

  /// The property controls the behavior of the opening or closing curve of the
  /// animation effect.
  /// QEasingCurve::InOutQuad by default
  /// \sa easingCurve(), setEasingCurve(), animationEffect, effectDuration
  Q_PROPERTY( QEasingCurve::Type easingCurve READ easingCurve WRITE setEasingCurve);

  /// Where is the popup in relation to the BaseWidget
  /// To vertically justify, use Qt::AlignTop | Qt::AlignBottom.
  /// Qt::AlignJustify | Qt::AlignBottom by default
  Q_PROPERTY( Qt::Alignment alignment READ alignment WRITE setAlignment);

  /// Direction of the scrolling effect, can be Qt::Vertical, Qt::Horizontal or
  /// both Qt::Vertical|Qt::Horizontal.
  /// Vertical by default
  Q_PROPERTY( Qt::Orientations orientation READ orientation WRITE setOrientation);

  /// Control where the popup opens vertically.
  /// TopToBottom by default
  Q_PROPERTY( PopupWidget::VerticalDirection verticalDirection READ verticalDirection WRITE setVerticalDirection);

  /// Control where the popup opens horizontally.
  /// LeftToRight by default
  Q_PROPERTY( Qt::LayoutDirection horizontalDirection READ horizontalDirection WRITE setHorizontalDirection);

public:
	typedef QStackedWidget Superclass;
  /// Although a popup widget is a top-level widget, if a parent is
  /// passed the popup widget will be deleted when that parent is
  /// destroyed (as with any other QObject).
  /// PopupWidget is a top-level widget (Qt::ToolTip), so
  /// even if a parent is passed, the popup will display outside the possible
  /// parent layout.
  /// \sa baseWidget().
  explicit PopupWidget(QWidget* parent = 0);
  virtual ~PopupWidget();

  QFrame* frame() const;

  /// Return the effectDuration property value.
  /// \sa effectDuration
  int effectDuration()const;
  /// Set the effectDuration property value.
  /// \sa effectDuration
  void setEffectDuration(int duration);

  /// Return the easingCurve property value.
  /// \sa easingCurve
  QEasingCurve::Type easingCurve()const;
  /// Set the easingCurve property value.
  /// \sa easingCurve
  void setEasingCurve(QEasingCurve::Type easingCurve);

  /// Return the alignment property value.
  /// \sa alignment
  Qt::Alignment alignment()const;
  /// Set the alignment property value.
  /// \sa alignment
  void setAlignment(Qt::Alignment alignment);

  /// Return the orientation property value.
  /// \sa orientation
  Qt::Orientations orientation()const;
  /// Set the orientation property value.
  /// \sa orientation
  void setOrientation(Qt::Orientations orientation);

  enum VerticalDirection{
	TopToBottom = 1,
	BottomToTop = 2
  };

  /// Return the verticalDirection property value.
  /// \sa verticalDirection
  VerticalDirection verticalDirection()const;
  /// Set the verticalDirection property value.
  /// \sa verticalDirection
  void setVerticalDirection(VerticalDirection direction);

  /// Return the horizontalDirection property value.
  /// \sa horizontalDirection
  Qt::LayoutDirection horizontalDirection()const;
  /// Set the horizontalDirection property value.
  /// \sa horizontalDirection
  void setHorizontalDirection(Qt::LayoutDirection direction);

public Q_SLOTS:
  /// Hide the popup if open or opening. It takes around 300ms
  /// for the fading effect to hide the popup.
  virtual void hidePopup();
  /// Open the popup if closed or closing. It takes around 300ms
  /// for the fading effect to open the popup.
  virtual void showPopup();
  /// Show/hide the popup. It can be conveniently linked to a QPushButton
  /// signal.
  inline void showPopup(bool show);

Q_SIGNALS:
  /// Fired when the popup finished its animation: opening (true) or closing (false).
  /// \sa showPopup(), hidePopup()
  void popupOpened(bool open);

protected:
  explicit PopupWidget(PopupWidgetPrivate* pimpl, QWidget* parent = 0);
  QScopedPointer<PopupWidgetPrivate> d_ptr;
  Q_PROPERTY(QRect effectGeometry READ effectGeometry WRITE setEffectGeometry DESIGNABLE false)

  QRect effectGeometry()const;

protected Q_SLOTS:
  virtual void onEffectFinished();
  void setEffectGeometry(QRect geometry);

private:
  Q_DECLARE_PRIVATE(PopupWidget);
  Q_DISABLE_COPY(PopupWidget);
};

Q_DECLARE_METATYPE(PopupWidget::VerticalDirection)

// -------------------------------------------------------------------------
void PopupWidget::showPopup(bool show)
{
  if (show)
	{
	this->showPopup();
	}
  else
	{
	this->hidePopup();
	}
}

#endif
