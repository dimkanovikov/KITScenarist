#include "ScenarioEditorWidget.h"

#include "ScenarioNavigatorWidget.h"
#include "ScenarioTextEditWidget.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QVariant>
#include <QFile>


ScenarioEditorWidget::ScenarioEditorWidget(QWidget *parent) :
	QFrame(parent),
	m_navigatorWidget(0),
	m_editorWidget(0)
{
	initView();
	initConnections();
	initStyleSheet();
}

ScenarioTextEdit* ScenarioEditorWidget::scenarioTextEdit()
{
	return editorWidget()->editor();
}

void ScenarioEditorWidget::initView()
{
	QSplitter* splitter = new QSplitter(this);
	splitter->setHandleWidth(1);
	splitter->addWidget(navigatorWidget());
	splitter->addWidget(editorWidget());
	splitter->setStretchFactor(1, 1);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->addWidget(splitter);

	this->setLayout(layout);
}

void ScenarioEditorWidget::initConnections()
{

}

void ScenarioEditorWidget::initStyleSheet()
{

}

ScenarioNavigatorWidget* ScenarioEditorWidget::navigatorWidget()
{
	if (m_navigatorWidget == 0) {
		m_navigatorWidget = new ScenarioNavigatorWidget(this, editorWidget()->editor());
	}
	return m_navigatorWidget;
}

ScenarioTextEditWidget* ScenarioEditorWidget::editorWidget()
{
	if (m_editorWidget == 0) {
		m_editorWidget = new ScenarioTextEditWidget(this);
	}
	return m_editorWidget;
}
