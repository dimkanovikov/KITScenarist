#include "SideBar.h"

namespace {
	const int ACTION_HEIGHT = 70;
	const int ACTION_WIDTH = 90;

	void convertToGrayscale (QImage& _img)
	{
		for (int r = 0; r < _img.height (); ++r)
		{
			for (int c = 0; c < _img.width (); ++c)
			{
				QRgb rgba = _img.pixel (c, r);

				int gray = qGray  (rgba);
				int a    = qAlpha (rgba);

				_img.setPixel (c, r, qRgba (gray, gray, gray, a));
			}
		}
	}
}

SideTabBar::SideTabBar(QWidget *parent)
	: QWidget(parent), m_centerTabs(false), m_pressedTab(0), m_checkedTab(0)
{
	setFixedWidth(ACTION_WIDTH);
}

SideTabBar::~SideTabBar()
{

}

void SideTabBar::paintEvent(QPaintEvent *event)
{
	QPainter p(this);

	p.fillRect(event->rect(), QBrush(QColor(32, 37, 41)));
	p.setPen(QColor(95, 95, 95));
	p.drawLine(event->rect().topRight(), event->rect().bottomRight());

	int actions_height = m_tabs.size()*ACTION_HEIGHT;

	int action_y = m_centerTabs ? (event->rect().height()/2-actions_height/2) : 0;
	foreach(QAction *action, m_tabs)
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

		if(action == m_tabs.last())
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
		QImage actionImage(action->icon().pixmap(icon_size).toImage());
		// Если действие недоступно
		if (!action->isEnabled()) {
			convertToGrayscale(actionImage);
		}
		// Если действие неактивно
		if (!action->isChecked()) {
			p.setOpacity(0.5);
		}
		// Рисуем иконку
		QIcon actionIcon(QPixmap::fromImage(actionImage));
		actionIcon.paint(&p, actionIconRect);
		p.setOpacity(1);


		p.setPen(QColor(217, 217, 217)); // неактивный текст
		if (action->isChecked())
			p.setPen(QColor(255, 255, 255)); // активный текст
		if (!action->isEnabled())
			p.setPen(QColor(160, 160, 160)); // недоступный текст
		QRect actionTextRect(0, action_y+actionRect.height()-23, event->rect().width(), 18);
		p.drawText(actionTextRect, Qt::AlignCenter, action->text());

		action_y += actionRect.height();
	}

}

QSize SideTabBar::minimumSizeHint() const
{
	return QSize(ACTION_WIDTH, m_tabs.size()*ACTION_HEIGHT);
}

void SideTabBar::addTab(QAction *action)
{
	action->setCheckable(true);
	if (m_tabs.isEmpty()) {
		action->setChecked(true);
		m_checkedTab = action;
	}
	m_tabs.push_back(action);
	update();
	return;
}

QAction *SideTabBar::addTab(const QString &text, const QIcon &icon)
{
	QAction *action = new QAction(icon, text, this);
	addTab(action);
	return action;
}

void SideTabBar::setCurrent(int _index)
{
	//
	// Если индекс в допустимом пределе и выделено не текущая вкладка
	//
	if (_index < m_tabs.size()
		&& m_tabs.indexOf(m_checkedTab) != _index) {
		m_checkedTab->setChecked(false);
		m_checkedTab = m_tabs.at(_index);
		m_checkedTab->setChecked(true);
		update();
		emit currentChanged(_index);
	}
}

QList<QAction*> SideTabBar::tabs() const
{
	return m_tabs;
}

void SideTabBar::mousePressEvent(QMouseEvent *event)
{
	m_pressedTab = tabAt(event->pos());
	if(m_pressedTab == 0 || m_pressedTab == m_checkedTab)
		return;
	update();
}

void SideTabBar::mouseReleaseEvent(QMouseEvent *event)
{
	QAction* tempAction = tabAt(event->pos());
	if(m_pressedTab != tempAction || tempAction == 0 || !tempAction->isEnabled())
	{
		m_pressedTab = 0;
		return;
	}
	if(m_checkedTab != 0)
		m_checkedTab->setChecked(false);
	m_checkedTab = m_pressedTab;
	if(m_checkedTab != 0)
		m_checkedTab->setChecked(true);
	update();
	m_pressedTab = 0;

	emit currentChanged(m_tabs.indexOf(m_checkedTab));
	return;
}

QAction* SideTabBar::tabAt(const QPoint &at)
{
	int actions_height = m_tabs.size()*ACTION_HEIGHT;

	int action_y = m_centerTabs ? (rect().height()/2-actions_height/2) : 0;
	foreach(QAction *action, m_tabs)
	{
		QRect actionRect(0, action_y, rect().width(), ACTION_HEIGHT);
		if(actionRect.contains(at))
			return action;
		action_y += actionRect.height();
	}
	return 0;
}
