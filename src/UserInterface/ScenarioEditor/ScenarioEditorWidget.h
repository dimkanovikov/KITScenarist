#ifndef SCENARIOEDITORWIDGET_H
#define SCENARIOEDITORWIDGET_H

#include <QFrame>

class ScenarioEditorWidget : public QFrame
{
	Q_OBJECT
public:
	explicit ScenarioEditorWidget(QWidget *parent = 0);

signals:

public slots:

private:
	void initView();
	void initConnections();
	void initStyleSheet();

};

#endif // SCENARIOEDITORWIDGET_H
