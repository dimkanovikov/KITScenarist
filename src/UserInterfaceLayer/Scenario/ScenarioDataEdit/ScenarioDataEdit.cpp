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

void ScenarioDataEdit::clear()
{
	ui->name->clear();
	ui->sourceSynopsis->clear();
	ui->fromScenarioSynopsis->clear();
	ui->isSourceSynopsis->setChecked(true);
	ui->synopsisStack->setCurrentWidget(ui->sourceSynopsisPage);

	ui->name->setFocus();
}

QString ScenarioDataEdit::scenarioName() const
{
	return ui->name->text();
}

void ScenarioDataEdit::setScenarioName(const QString& _name)
{
	ui->name->setText(_name);
}

QString ScenarioDataEdit::scenarioSynopsis() const
{
	return ui->sourceSynopsis->toHtml();
}

void ScenarioDataEdit::setScenarioSynopsis(const QString& _synopsis)
{
	ui->sourceSynopsis->setHtml(_synopsis);
}

void ScenarioDataEdit::setScenarioSynopsisFromScenes(const QString& _synopsis)
{
	ui->fromScenarioSynopsis->setHtml(_synopsis);
}

void ScenarioDataEdit::aboutCurrentSynopsisChanged()
{
	if (ui->isSourceSynopsis->isChecked()) {
		ui->synopsisStack->setCurrentWidget(ui->sourceSynopsisPage);
	} else {
		ui->synopsisStack->setCurrentWidget(ui->fromScenarioSynopsisPage);
	}
}

void ScenarioDataEdit::initView()
{
	clear();
}

void ScenarioDataEdit::initConnections()
{
	connect(ui->isSourceSynopsis, SIGNAL(toggled(bool)), this, SLOT(aboutCurrentSynopsisChanged()));
	connect(ui->isFromScenarioSynopsis, SIGNAL(clicked()), this, SIGNAL(buildSynopsisFromScenes()));
}

void ScenarioDataEdit::initStyleSheet()
{
	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

	ui->mainContainer->setProperty("mainContainer", true);
}
