#include "LocationsDataEdit.h"
#include "ui_LocationsDataEdit.h"

using UserInterface::LocationsDataEdit;


LocationsDataEdit::LocationsDataEdit(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::LocationsDataEdit)
{
	ui->setupUi(this);

	initView();
	initConnections();
}

LocationsDataEdit::~LocationsDataEdit()
{
	delete ui;
}

void LocationsDataEdit::clean()
{
	ui->sourceName->setText(tr("Choose Location"));
	ui->name->clear();
	initView();
}

void LocationsDataEdit::setName(const QString& _name)
{
	ui->sourceName->setText(_name);
	ui->name->setText(_name);
	initView();
}

QString LocationsDataEdit::name() const
{
	return ui->name->text();
}

void LocationsDataEdit::aboutDataChanged()
{
	ui->save->setEnabled(true);
}

void LocationsDataEdit::initView()
{
	ui->save->setEnabled(false);
}

void LocationsDataEdit::initConnections()
{
	connect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(aboutDataChanged()));
	connect(ui->save, SIGNAL(clicked()), this, SLOT(initView()));
	connect(ui->save, SIGNAL(clicked()), this, SIGNAL(saveLocation()));
	connect(ui->cancel, SIGNAL(clicked()), this, SIGNAL(reloadLocation()));
}
