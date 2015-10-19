#include "LoginDialog.h"
#include "ui_LoginDialog.h"

#include <QDesktopServices>
#include <QUrl>

using UserInterface::LoginDialog;


LoginDialog::LoginDialog(QWidget* _parent) :
	QWidget(_parent),
	m_ui(new Ui::LoginDialog)
{
	m_ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

LoginDialog::~LoginDialog()
{
	delete m_ui;
}

void LoginDialog::clear()
{
	m_ui->error->hide();
	m_ui->userName->clear();
	m_ui->password->clear();
	m_ui->progressBar->hide();
	m_ui->content->setEnabled(true);
}

QString LoginDialog::userName() const
{
	return m_ui->userName->text();
}

void LoginDialog::setUserName(const QString& _userName)
{
	m_ui->userName->setText(_userName);
}

QString LoginDialog::password() const
{
	return m_ui->password->text();
}

void LoginDialog::setPassword(const QString& _password)
{
	m_ui->password->setText(_password);
}

void LoginDialog::setError(const QString& _error)
{
	m_ui->error->setText(_error);
	m_ui->error->show();
	updateSize();
}

void LoginDialog::showProgressBar()
{
	m_ui->progressBar->show();
	m_ui->content->setEnabled(false);
	updateSize();
}

void LoginDialog::hideProgressBar()
{
	m_ui->progressBar->hide();
	m_ui->content->setEnabled(true);
	updateSize();
}

void LoginDialog::initView()
{
	m_ui->error->hide();
	m_ui->progressBar->hide();
}

void LoginDialog::initConnections()
{
	connect(m_ui->signUp, &QPushButton::clicked, [=](){
		QDesktopServices::openUrl(QUrl("https://kitscenarist.ru/cabin.html"));
	});
	connect(m_ui->login, &QPushButton::clicked, this, &LoginDialog::loginClicked);
	connect(m_ui->cancel, &QPushButton::clicked, this, &LoginDialog::cancelClicked);
}

void LoginDialog::initStyleSheet()
{
	m_ui->signUp->setProperty("flat", true);
	m_ui->login->setProperty("raised", true);
	m_ui->cancel->setProperty("raised", true);
}

void LoginDialog::updateSize()
{
	resize(width(), sizeHint().height());
}
