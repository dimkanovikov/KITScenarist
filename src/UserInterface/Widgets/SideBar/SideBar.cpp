#include "SideBar.h"

namespace {
	const int ACTION_HEIGHT = 70;
	const int ACTION_WIDTH = 90;
}

SideBar::SideBar(QWidget *parent)
	: QWidget(parent), m_centerActions(false), _pressedAction(NULL), _checkedAction(NULL)
{
	setFixedWidth(ACTION_WIDTH);
}

SideBar::~SideBar()
{

}

void SideBar::paintEvent(QPaintEvent *event)
{
	QPainter p(this);

	//Adapter l'adresse de la texture ici
	QImage texture(":/Interface/UI/SideBar/sidebar-texture.png");
	p.fillRect(event->rect(), QBrush(texture));
	p.setPen(QColor(95, 95, 95));
	p.drawLine(event->rect().topRight(), event->rect().bottomRight());

	int actions_height = _actions.size()*ACTION_HEIGHT;

	int action_y = m_centerActions ? (event->rect().height()/2-actions_height/2) : 0;
	foreach(QAction *action, _actions)
	{
		QRect actionRect(0, action_y, event->rect().width(), ACTION_HEIGHT);


		if(action->isChecked())
		{
			p.setOpacity(0.5);
			p.fillRect(actionRect, QColor(19, 19, 19));
			p.setPen(QColor(9, 9, 9));
			p.drawLine(actionRect.topLeft(), actionRect.topRight());
			p.setOpacity(1);

		}

		if(action == _actions.last())
		{
			p.setPen(QColor(15, 15, 15));
			p.drawLine(QPoint(0, actionRect.bottomLeft().y()-1), QPoint(actionRect.width(), actionRect.bottomRight().y()-1));
			p.setPen(QColor(95, 95, 95));
			p.drawLine(actionRect.bottomLeft(), actionRect.bottomRight());

		}

		if(!action->isChecked())
		{
			p.setPen(QColor(15, 15, 15));
			p.drawLine(actionRect.topLeft(), actionRect.topRight());
			p.setPen(QColor(95, 95, 95));
			p.drawLine(QPoint(0, actionRect.topLeft().y()+1), QPoint(actionRect.width(), actionRect.topRight().y()+1));
		}

		int icon_size = 48;

		QRect actionIconRect(0, action_y, event->rect().width(), ACTION_HEIGHT-16);
		QImage actionImage(
					action->icon().pixmap(
						icon_size,
						action->isEnabled() ? QIcon::Normal : QIcon::Disabled
						)
					.toImage()
					);
		QIcon actionIcon(QPixmap::fromImage(actionImage));
		if(!action->isChecked()) {
			p.setOpacity(0.5);
		}
		actionIcon.paint(&p, actionIconRect);
		p.setOpacity(1);


		p.setPen(QColor(217, 217, 217));
		if(action->isChecked())
			p.setPen(QColor(255, 255, 255));
		QRect actionTextRect(0, action_y+actionRect.height()-23, event->rect().width(), 18);
		p.drawText(actionTextRect, Qt::AlignCenter, action->text());

		action_y += actionRect.height();
	}

}

QSize SideBar::minimumSizeHint() const
{
	return QSize(ACTION_WIDTH, _actions.size()*ACTION_HEIGHT);
}

void SideBar::addAction(QAction *action)
{
	action->setCheckable(true);
	if (_actions.isEmpty()) {
		action->setChecked(true);
		_checkedAction = action;
	}
	_actions.push_back(action);
	update();
	return;
}

QAction *SideBar::addAction(const QString &text, const QIcon &icon)
{
	QAction *action = new QAction(icon, text, this);
	addAction(action);
	return action;
}

void SideBar::mousePressEvent(QMouseEvent *event)
{
	_pressedAction = actionAt(event->pos());
	if(_pressedAction == NULL || _pressedAction == _checkedAction)
		return;
	update();
}

void SideBar::mouseReleaseEvent(QMouseEvent *event)
{
	QAction* tempAction = actionAt(event->pos());
	if(_pressedAction != tempAction || tempAction == NULL || !tempAction->isEnabled())
	{
		_pressedAction = NULL;
		return;
	}
	if(_checkedAction != NULL)
		_checkedAction->setChecked(false);
	_checkedAction = _pressedAction;
	if(_checkedAction != NULL)
		_checkedAction->setChecked(true);
	update();
	_pressedAction = NULL;

	emit currentChanged(_actions.indexOf(_checkedAction));
	return;
}

QAction* SideBar::actionAt(const QPoint &at)
{
	int actions_height = _actions.size()*ACTION_HEIGHT;

	int action_y = m_centerActions ? (rect().height()/2-actions_height/2) : 0;
	foreach(QAction *action, _actions)
	{
		QRect actionRect(0, action_y, rect().width(), ACTION_HEIGHT);
		if(actionRect.contains(at))
			return action;
		action_y += actionRect.height();
	}
	return NULL;
}
