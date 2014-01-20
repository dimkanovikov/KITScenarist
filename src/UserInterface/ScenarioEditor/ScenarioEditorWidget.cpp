#include "ScenarioEditorWidget.h"

#include "ScenarioNavigatorWidget.h"
#include "ScenarioTextEditWidget.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QVariant>
#include <QFile>


ScenarioEditorWidget::ScenarioEditorWidget(QWidget *parent) :
	QFrame(parent)
{
	initView();
	initConnections();
	initStyleSheet();
}

void ScenarioEditorWidget::initView()
{
	ScenarioTextEditWidget* editorWidget = new ScenarioTextEditWidget(this);
	ScenarioNavigatorWidget* navigatorWidget = new ScenarioNavigatorWidget(this, editorWidget->editor());

	QSplitter* splitter = new QSplitter(this);
	splitter->setHandleWidth(1);
	splitter->addWidget(navigatorWidget);
	splitter->addWidget(editorWidget);
	splitter->setStretchFactor(1, 1);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins(1,1,1,1));
	layout->addWidget(splitter);

	this->setLayout(layout);
}

void ScenarioEditorWidget::initConnections()
{

}

void ScenarioEditorWidget::initStyleSheet()
{
	// Считываем стиль из ресурсов
	//
	QFile styleSheetFile(":/Interface/UI/ScenarioEditor/style.qss");
	styleSheetFile.open(QIODevice::ReadOnly);
	QString styleSheet = styleSheetFile.readAll();

	//
	// Применяем стиль
	//
	setStyleSheet(styleSheet);
}
