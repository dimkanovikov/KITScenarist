#include "LoginView.h"
#include "ui_LoginView.h"

#include <QDesktopServices>
#include <QUrl>

using UserInterface::LoginView;


LoginView::LoginView(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::LoginView)
{
	m_ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

LoginView::~LoginView()
{
	delete m_ui;
}

QWidget*LoginView::toolbar()
{
	return m_ui->toolbar;
}

void LoginView::clear()
{
	m_ui->error->hide();
	m_ui->userName->clear();
	m_ui->password->clear();
	m_ui->progressBar->hide();
	setControlsEnabled(true);
}

QString LoginView::userName() const
{
	return m_ui->userName->text();
}

void LoginView::setUserName(const QString& _userName)
{
	m_ui->userName->setText(_userName);
}

QString LoginView::password() const
{
	return m_ui->password->text();
}

void LoginView::setPassword(const QString& _password)
{
	m_ui->password->setText(_password);
}

void LoginView::setError(const QString& _error)
{
	m_ui->error->setText(_error);
	m_ui->error->show();
}

void LoginView::showProgressBar()
{
	m_ui->progressReplacer->hide();
	m_ui->progressBar->show();
	setControlsEnabled(false);
}

void LoginView::hideProgressBar()
{
	m_ui->progressReplacer->show();
	m_ui->progressReplacer->setFixedHeight(m_ui->progressBar->height());
	m_ui->progressBar->hide();
	setControlsEnabled(true);
}

void LoginView::initView()
{
	m_ui->error->hide();
	m_ui->progressBar->hide();
}

void LoginView::initConnections()
{
	//
	// TODO: Переделать на регистрацию прямо через программу
	//
	connect(m_ui->signUp, &QPushButton::clicked, [=](){
		QDesktopServices::openUrl(QUrl("https://kitscenarist.ru/cabin.html"));
	});
	connect(m_ui->login, &QPushButton::clicked, this, &LoginView::loginClicked);
}

void LoginView::initStyleSheet()
{
	m_ui->signUp->setProperty("flat", true);
	m_ui->login->setProperty("raised", true);
}

void LoginView::setControlsEnabled(bool _enabled)
{
	m_ui->login->setEnabled(_enabled);
	m_ui->password->setEnabled(_enabled);
	m_ui->signUp->setEnabled(_enabled);
	m_ui->login->setEnabled(_enabled);
}
