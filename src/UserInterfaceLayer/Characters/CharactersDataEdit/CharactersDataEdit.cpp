#include "CharactersDataEdit.h"
#include "ui_CharactersDataEdit.h"

using UserInterface::CharactersDataEdit;


CharactersDataEdit::CharactersDataEdit(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CharactersDataEdit)
{
	ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

CharactersDataEdit::~CharactersDataEdit()
{
	delete ui;
}

void CharactersDataEdit::clean()
{
	ui->sourceName->setText(tr("Choose Character"));
	ui->name->clear();
	initView();
}

void CharactersDataEdit::setName(const QString& _name)
{
	ui->sourceName->setText(_name);
	ui->name->setText(_name);
	initView();
}

QString CharactersDataEdit::name() const
{
	return ui->name->text();
}

void CharactersDataEdit::aboutDataChanged()
{
	ui->save->setEnabled(true);
}

void CharactersDataEdit::initView()
{
	ui->save->setEnabled(false);
}

void CharactersDataEdit::initConnections()
{
	connect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(aboutDataChanged()));
	connect(ui->save, SIGNAL(clicked()), this, SLOT(initView()));
	connect(ui->save, SIGNAL(clicked()), this, SIGNAL(saveCharacter()));
	connect(ui->cancel, SIGNAL(clicked()), this, SIGNAL(reloadCharacter()));
}

void CharactersDataEdit::initStyleSheet()
{
	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

	ui->mainContainer->setProperty("mainContainer", true);
}
