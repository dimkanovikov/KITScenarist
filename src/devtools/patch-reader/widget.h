#ifndef WIDGET_H
#define WIDGET_H

#include <QTabWidget>

class Widget : public QTabWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
};
#endif // WIDGET_H
