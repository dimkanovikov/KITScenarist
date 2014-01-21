#ifndef SCENARIOEDITORWIDGET_H
#define SCENARIOEDITORWIDGET_H

#include <QFrame>

class ScenarioNavigatorWidget;
class ScenarioTextEditWidget;
class ScenarioTextEdit;


class ScenarioEditorWidget : public QFrame
{
	Q_OBJECT
public:
	explicit ScenarioEditorWidget(QWidget *parent = 0);

	ScenarioTextEdit* scenarioTextEdit();

signals:

public slots:

private:
	void initView();
	void initConnections();
	void initStyleSheet();

	ScenarioNavigatorWidget* navigatorWidget();
	ScenarioTextEditWidget* editorWidget();

private:
	ScenarioNavigatorWidget* m_navigatorWidget;
	ScenarioTextEditWidget* m_editorWidget;

};

#endif // SCENARIOEDITORWIDGET_H
