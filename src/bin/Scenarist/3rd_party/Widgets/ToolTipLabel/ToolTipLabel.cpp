#include "ToolTipLabel.h"

#include <QMouseEvent>
#include <QToolTip>


ToolTipLabel::ToolTipLabel(QWidget* _parent) :
	QLabel(_parent)
{
	setMouseTracking(true);
	setCursor(Qt::BlankCursor);
}

void ToolTipLabel::mouseMoveEvent(QMouseEvent* _event)
{
	QLabel::mouseMoveEvent(_event);

	QToolTip::showText(mapToGlobal(_event->pos()), toolTip(), this);
}
