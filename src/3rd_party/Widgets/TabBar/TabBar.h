#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>

class TabBar : public QTabBar
{
    Q_OBJECT

public:
    explicit TabBar(QWidget *_parent = 0);

protected:
    QSize tabSizeHint(int _index) const;
    QSize sizeHint() const;
};

#endif // TABBAR_H
