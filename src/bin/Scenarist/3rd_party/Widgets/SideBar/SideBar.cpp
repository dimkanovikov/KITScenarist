#include "SideBar.h"

#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QWidgetAction>

namespace {
	/**
	 * @brief Ширина панели
	 */
	const int SIDEBAR_WIDTH = 90;

	/**
	 * @brief Высота вкладки
	 */
	const int TAB_HEIGHT = 70;

	/**
	 * @brief Высота индикатора
	 */
	const int INDICATOR_HEIGHT = 32;

	/**
	 * @brief Размер иконки индикатора
	 */
	const QSize INDICATOR_ICON_SIZE(16, 16);

	/**
	 * @brief Цвет текущей вкладки
	 */
	const QColor CURRENT_TAB_BG_COLOR(38, 40, 42);

	/**
	 * @brief Покрасить иконку в цвет
	 */
	static QPixmap colorizeIcon(const QPixmap& _icon, const QColor& _color) {
		QPixmap baseIconPixmap = _icon;
		QPixmap newIconPixmap = baseIconPixmap;

		QPainter painter(&newIconPixmap);
		painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
		painter.fillRect(newIconPixmap.rect(), _color);
		painter.end();

		return newIconPixmap;
	}

	/**
	 * @brief Сделать иконку "неактивной"
	 */
	static QPixmap makeIconInactive(const QPixmap& _icon) {
		return ::colorizeIcon(_icon, QApplication::palette().color(QPalette::Inactive, QPalette::Text));
	}

	/**
	 * @brief Сделать иконку "выключенной"
	 */
	static QPixmap makeIconDisabled(const QPixmap& _icon) {
		return ::colorizeIcon(_icon, QApplication::palette().color(QPalette::Disabled, QPalette::Text));
	}
}

SideTabBar::SideTabBar(QWidget *parent) :
	QWidget(parent),
	m_centerTabs(false),
	m_pressedTab(0),
	m_checkedTab(0),
	m_indicator(new QAction(this))
{
	setFixedWidth(SIDEBAR_WIDTH);

	//
	// По умолчанию индикатор скрыт
	//
	removeIndicator();
}

void SideTabBar::paintEvent(QPaintEvent *event)
{
	QPainter p(this);


	//
	// Фон
	//
	p.fillRect(event->rect(), palette().button());
	//
	// Границы сверху и справа
	//
	p.setPen(palette().dark().color());
	p.drawLine(event->rect().topLeft(), event->rect().topRight());
	p.drawLine(event->rect().topRight(), event->rect().bottomRight());


	//
	// Рисуем вкладки
	//
	int actions_height = m_tabs.size()*TAB_HEIGHT;
	int action_y = m_centerTabs ? (event->rect().height()/2-actions_height/2) : 1;
	foreach (QAction *action, m_tabs)
	{
		QRect actionRect(0, action_y, event->rect().width() - 1, TAB_HEIGHT);

		//
		// Текущая вкладка
		//
		if (action->isChecked()) {
			p.fillRect(actionRect, palette().window());

		}


		int icon_size = 48;

		QRect actionIconRect(0, action_y, event->rect().width(), TAB_HEIGHT-16);
		QPixmap actionImage = action->icon().pixmap(icon_size);
		// Если действие недоступно
		if (!action->isEnabled()) {
			actionImage = ::makeIconDisabled(actionImage);
		}
		// Если действие неактивно
		else if (!action->isChecked()) {
			actionImage = ::makeIconInactive(actionImage);
		}
		// Рисуем иконку
		QIcon actionIcon(actionImage);
		actionIcon.paint(&p, actionIconRect);
		p.setOpacity(1);


		p.setPen(QApplication::palette().color(QPalette::Inactive, QPalette::Text)); // неактивный текст
		if (action->isChecked())
			p.setPen(QApplication::palette().color(QPalette::Active, QPalette::Text)); // активный текст
		if (!action->isEnabled())
			p.setPen(QApplication::palette().color(QPalette::Disabled, QPalette::Text)); // недоступный текст
		QRect actionTextRect(0, action_y+actionRect.height()-23, event->rect().width(), 18);
		p.drawText(actionTextRect, Qt::AlignCenter, action->text());

		action_y += actionRect.height();
	}

	//
	// Рисуем индикатор
	//
	if (m_indicator->isVisible()) {
		const QRect indicatorRect(0, height() - INDICATOR_HEIGHT, SIDEBAR_WIDTH, INDICATOR_HEIGHT);
		//
		// Граница сверху
		//
		p.setPen(palette().dark().color());
		p.drawLine(indicatorRect.topLeft(), indicatorRect.topRight());
		//
		// Иконка индикатора
		//
		m_indicator->icon().paint(&p, indicatorRect);
	}
}

QSize SideTabBar::minimumSizeHint() const
{
	return QSize(SIDEBAR_WIDTH, (m_tabs.size() * TAB_HEIGHT) + (m_indicator->isVisible() ? INDICATOR_HEIGHT : 0));
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

void SideTabBar::addIndicator(const QIcon& _icon, const QString& _title, const QString& _message)
{
	m_indicator->setIcon(_icon);
	m_indicator->setText(QString("<b>%1</b><p>%2</p>").arg(_title).arg(_message).replace("\n", "<br/>"));
	m_indicator->setVisible(!_icon.isNull());
	update();
}

void SideTabBar::removeIndicator()
{
	addIndicator(QIcon());
}

void SideTabBar::mousePressEvent(QMouseEvent* _event)
{
	//
	// Нажата кнопка?
	//
	m_pressedTab = tabAt(_event->pos());
	if (m_pressedTab != 0 && m_pressedTab != m_checkedTab) {
		//
		// Обновим внешний вид
		//
		update();
	}
	//
	// Нажат индикатор?
	//
	else if (m_indicator->isVisible()
			 && !m_indicator->text().isEmpty()
			 && _event->pos().y() > (height() - INDICATOR_HEIGHT)) {
		//
		// Покажем информацию
		//
		QMenu menu(this);
		QWidgetAction menuText(&menu);
		QLabel label(m_indicator->text());
		label.setMargin(8);
		menuText.setDefaultWidget(&label);
		menu.addAction(&menuText);
		menu.exec(mapToGlobal(QPoint(SIDEBAR_WIDTH, height() - menu.sizeHint().height())));
	}
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
	int actions_height = m_tabs.size()*TAB_HEIGHT;

	int action_y = m_centerTabs ? (rect().height()/2-actions_height/2) : 0;
	foreach(QAction *action, m_tabs)
	{
		QRect actionRect(0, action_y, rect().width(), TAB_HEIGHT);
		if(actionRect.contains(at))
			return action;
		action_y += actionRect.height();
	}
	return 0;
}
