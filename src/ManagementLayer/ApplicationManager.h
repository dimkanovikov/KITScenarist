#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <QObject>

class ApplicationManager : public QObject
{
	Q_OBJECT
public:
	explicit ApplicationManager(QObject *parent = 0);

	void exec();

signals:

public slots:
	void print();

};

#endif // APPLICATIONMANAGER_H
