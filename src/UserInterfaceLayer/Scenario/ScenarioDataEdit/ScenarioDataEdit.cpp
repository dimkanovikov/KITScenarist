#include "ScenarioDataEdit.h"
#include "ui_ScenarioDataEdit.h"

using UserInterface::ScenarioDataEdit;


ScenarioDataEdit::ScenarioDataEdit(QWidget* _parent) :
	QWidget(_parent),
	ui(new Ui::ScenarioDataEdit)
{
	ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

ScenarioDataEdit::~ScenarioDataEdit()
{
	delete ui;
}

QWidget* ScenarioDataEdit::toolbar() const
{
	return ui->topEmptyLabel;
}

void ScenarioDataEdit::initView()
{

}

void ScenarioDataEdit::initConnections()
{

}

void ScenarioDataEdit::initStyleSheet()
{
	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

	ui->mainContainer->setProperty("mainContainer", true);
}
