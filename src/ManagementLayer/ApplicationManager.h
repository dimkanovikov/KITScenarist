#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <QObject>

class ScenarioTextEdit;
class QLabel;

class ApplicationManager : public QObject
{
	Q_OBJECT
public:
	explicit ApplicationManager(QObject *parent = 0);

	void exec();

signals:

public slots:
	void print();
	void updatePositionDuration();

private:
	ScenarioTextEdit* textEdit;
	QLabel* label;
};

#endif // APPLICATIONMANAGER_H
