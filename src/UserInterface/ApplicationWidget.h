#ifndef APPLICATIONWIDGET_H
#define APPLICATIONWIDGET_H

#include <QWidget>


/**
 * @brief Виджет приложения
 */
class ApplicationWidget : public QWidget
{
	Q_OBJECT
public:
	explicit ApplicationWidget(QWidget *parent = 0);

signals:

public slots:


private:
	void initView();
};

#endif // APPLICATIONWIDGET_H
