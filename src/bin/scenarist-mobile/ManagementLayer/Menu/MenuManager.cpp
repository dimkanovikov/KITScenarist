#include "MenuManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <UserInterfaceLayer/Menu/MenuView.h>
#include <UserInterfaceLayer/Menu/LoginDialog.h>

#include <3rd_party/Helpers/PasswordStorage.h>
#include <3rd_party/Widgets/WAF/Animation.h>

#include <QApplication>

using ManagementLayer::MenuManager;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;
using UserInterface::MenuView;
using UserInterface::LoginDialog;


MenuManager::MenuManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new MenuView(_parentWidget)),
	m_loginDialog(new LoginDialog(_parentWidget))
{
	initData();
	initView();
	initConnections();
	initStyleSheet();
}

QWidget* MenuManager::view() const
{
	return m_view;
}

void MenuManager::showMenu()
{
	//
	// Скрываем виртуальную клавиатуру
	//
	QApplication::inputMethod()->hide();

	//
	// Выкатываем меню
	//
	WAF::Animation::sideSlideIn(m_view, WAF::LeftSide);
}

void MenuManager::userLogged()
{
	if (m_userName.isEmpty()) {
		//
		// Загрузим имя пользователя из настроек
		//
		m_userName =
				StorageFacade::settingsStorage()->value(
					"application/user-name",
					SettingsStorage::ApplicationSettings);
	}

	const bool isLogged = true;
	m_view->setUserLogged(isLogged, m_userName);
	WAF::Animation::sideSlideOut(m_loginDialog);
}

void MenuManager::retryLogin(const QString& _error)
{
	//
	// Показать диалог авторизации с заданной ошибкой
	//
	m_loginDialog->setUserName(m_userName);
	m_loginDialog->setPassword(m_password);
	m_loginDialog->setError(_error);
	m_loginDialog->hideProgressBar();

	if (m_loginDialog->isHidden()) {
		WAF::Animation::sideSlideIn(m_loginDialog, WAF::TopSide);
	}
}

void MenuManager::userUnlogged()
{
	const bool isUnlogged = false;
	m_view->setUserLogged(isUnlogged);
	m_userName.clear();
}

void MenuManager::showProjectSubmenu(const QString& _projectName)
{
	m_view->showProjectSubmenu(_projectName);
}

void MenuManager::hideProjectSubmenu()
{
	m_view->hideProjectSubmenu();
}

void MenuManager::initData()
{
}

void MenuManager::initView()
{
	m_view->hide();
	m_loginDialog->hide();
}

void MenuManager::initConnections()
{
	connect(m_view, &MenuView::backClicked, [=](){
		WAF::Animation::sideSlideOut(m_view);
	});
	connect(m_view, &MenuView::signInClicked, [=](){
		//
		// Если не авторизован - авторизация
		//
		if (m_userName.isEmpty()) {
			m_loginDialog->clear();
			WAF::Animation::sideSlideIn(m_loginDialog, WAF::TopSide);
		}
		//
		// В противном случае - закрытие авторизации
		//
		else {
			emit logoutRequested();
		}
	});
	connect(m_view, &MenuView::projectsClicked, [=](){
		WAF::Animation::sideSlideOut(m_view);
		emit projectsRequested();
	});
	connect(m_view, &MenuView::projectSaveClicked, [=](){
		WAF::Animation::sideSlideOut(m_view);
		emit projectSaveRequested();
	});
	connect(m_view, &MenuView::projectTextClicked, [=](){
		WAF::Animation::sideSlideOut(m_view);
		emit projectTextRequested();
	});


	connect(m_loginDialog, &LoginDialog::loginClicked, [=](){
		m_loginDialog->showProgressBar();
		m_userName = m_loginDialog->userName();
		m_password = m_loginDialog->password();
		emit loginRequested(m_userName, m_password);
	});
	connect(m_loginDialog, &LoginDialog::cancelClicked, [=](){
		WAF::Animation::sideSlideOut(m_loginDialog);
	});
}

void MenuManager::initStyleSheet()
{
}

