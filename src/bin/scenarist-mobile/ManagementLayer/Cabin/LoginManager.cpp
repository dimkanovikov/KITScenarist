#include "LoginManager.h"

#include <UserInterfaceLayer/Cabin/LoginView.h>

using ManagementLayer::LoginManager;
using UserInterface::LoginView;


LoginManager::LoginManager(QObject* _parent, QWidget* _parentWidget):
	QObject(_parent),
	m_view(new LoginView(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* LoginManager::toolbar() const
{
	return m_view->toolbar();
}

QWidget* LoginManager::view() const
{
	return m_view;
}

void LoginManager::clear()
{
	m_view->clear();
}

void LoginManager::setUserName(const QString& _userName)
{
	m_view->setUserName(_userName);
}

void LoginManager::setPassword(const QString& _password)
{
	m_view->setPassword(_password);
}

void LoginManager::setError(const QString& _error)
{
	m_view->hideProgressBar();
	m_view->setError(_error);
}

void LoginManager::showProgressBar()
{
	m_view->showProgressBar();
}

void LoginManager::hideProgressBar()
{
	m_view->hideProgressBar();
}

void LoginManager::initView()
{

}

void LoginManager::initConnections()
{
	connect(m_view, &LoginView::loginClicked, [=] {
		m_view->showProgressBar();
		emit loginRequested(m_view->userName(), m_view->password());
	});
}
