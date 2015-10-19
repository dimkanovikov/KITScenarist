#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>

class TabBar : public QTabBar
{
    Q_OBJECT

public:
    explicit TabBar(QWidget *_parent = 0);

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

protected:
	QSize tabSizeHint(int _index) const;
};

#endif // TABBAR_H
