#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QtWidgets//QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QIcon>
#include <QAction>

class SideTabBar : public QWidget
{
	Q_OBJECT

public:
	SideTabBar(QWidget *parent = 0);

	void addTab(QAction* action);
	QAction* addTab(const QString& text, const QIcon& icon = QIcon());

	void setCurrent(int _index);

	QList<QAction*> tabs() const;

	/**
	 * @brief Добавить индикатор вниз панели
	 * @note Вызов с пустыми параметрами, равен вызову функции removeIndicator
	 */
	void addIndicator(const QIcon& _icon, const QString& _title = QString::null, const QString& _message = QString::null);

	/**
	 * @brief Убрать индикатор из панели
	 */
	void removeIndicator();

signals:
	void currentChanged(int index);

protected:
	void paintEvent(QPaintEvent* event);
	void mousePressEvent(QMouseEvent* _event);
	void mouseReleaseEvent(QMouseEvent* event);
	QSize minimumSizeHint() const;

private:
	QAction* tabAt(const QPoint& at);

private:
	bool m_centerTabs;
	QList<QAction*> m_tabs;
	QAction* m_pressedTab;
	QAction* m_checkedTab;

	/**
	 * @brief Индикатор внизу панели
	 */
	QAction* m_indicator;
};

#endif // SIDEBAR_H
