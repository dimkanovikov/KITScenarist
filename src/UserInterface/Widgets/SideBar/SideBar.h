#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QtWidgets//QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QIcon>
#include <QAction>

class SideBar : public QWidget
{
	Q_OBJECT

public:
	SideBar(QWidget *parent = 0);
	~SideBar();

	void addAction(QAction *action);
	QAction *addAction(const QString &text, const QIcon &icon = QIcon());

signals:
	void currentChanged(int index);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	QSize minimumSizeHint() const;

private:
	QAction* actionAt(const QPoint &at);

private:
	bool m_centerActions;
	QList<QAction*> _actions;
	QAction *_pressedAction;
	QAction *_checkedAction;
};

#endif // SIDEBAR_H
