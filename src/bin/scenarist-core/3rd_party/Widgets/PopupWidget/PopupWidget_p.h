/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

	  http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __PopupWidget_p_h
#define __PopupWidget_p_h

// Qt includes
#include <QPointer>
class QLabel;
class QPropertyAnimation;

// CTK includes
#include "PopupWidget.h"
#define PopupWindowType Qt::Tool

// -------------------------------------------------------------------------
/// \ingroup Widgets
class PopupWidgetPrivate
  : public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(PopupWidget);
protected:
  PopupWidget* const q_ptr;
public:
  PopupWidgetPrivate(PopupWidget& object);
  ~PopupWidgetPrivate();
  virtual void init();

  bool isOpening()const;
  bool isClosing()const;
  /// Return true if the animation was closing (direction == backward).
  /// It doesn't indicate if the action is still running or finished.
  /// Can only be called in a slot as it uses sender().
  bool wasClosing()const;

  Qt::Alignment pixmapAlignment()const;
  void setupPopupPixmapWidget();

  /// Return the closed geometry for the popup based on the current geometry
  QRect closedGeometry()const;

  /// Return the desired geometry, maybe it won't happen if the size is too
  /// small for the popup.
  QRect desiredOpenGeometry()const;

  QPropertyAnimation* currentAnimation()const;

protected:
  int                 EffectDuration;
  QPropertyAnimation* ScrollAnimation;
  QLabel*             PopupPixmapWidget;
  QWidget*            PopupPixmapWidgetPage;
  QFrame*             Frame;
  QWidget*            FramePage;

  // Geometry attributes
  Qt::Alignment    Alignment;
  Qt::Orientations Orientations;

  PopupWidget::VerticalDirection VerticalDirection;
  Qt::LayoutDirection HorizontalDirection;
};

#endif
