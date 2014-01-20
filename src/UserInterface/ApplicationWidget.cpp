#include "ApplicationWidget.h"

#include "ScenarioEditor/ScenarioEditorWidget.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QVariant>
#include <QFile>


ApplicationWidget::ApplicationWidget(QWidget *parent) :
	QWidget(parent)
{
	initView();
}

void ApplicationWidget::initView()
{
	ScenarioEditorWidget* editorWidget = new ScenarioEditorWidget(this);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->addWidget(editorWidget);

	this->setLayout(layout);
}
