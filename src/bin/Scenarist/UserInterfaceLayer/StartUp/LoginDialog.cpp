#include "LoginDialog.h"
#include "ui_LoginDialog.h"

using UserInterface::LoginDialog;


LoginDialog::LoginDialog(QWidget* _parent) :
	QLightBoxDialog(_parent),
	ui(new Ui::LoginDialog)
{
	ui->setupUi(this);

	initView();
	initConnections();
}

LoginDialog::~LoginDialog()
{
	delete ui;
}

QString LoginDialog::userName() const
{
	return ui->userName->text();
}

void LoginDialog::setUserName(const QString& _userName)
{
	ui->userName->setText(_userName);
}

QString LoginDialog::password() const
{
	return ui->password->text();
}

void LoginDialog::setPassword(const QString& _password)
{
	ui->password->setText(_password);
}

void LoginDialog::setError(const QString& _error)
{
	ui->error->setText(_error);
	ui->error->show();
	resize(width(), sizeHint().height());
}

QWidget* LoginDialog::focusedOnExec() const
{
	return ui->userName;
}

void LoginDialog::initView()
{
	ui->error->hide();
	ui->buttons->addButton(tr("Login"), QDialogButtonBox::AcceptRole);

	QLightBoxDialog::initView();
}

void LoginDialog::initConnections()
{
	connect(ui->buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui->buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QLightBoxDialog::initConnections();
}
