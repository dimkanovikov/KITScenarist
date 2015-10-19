#include "MenuManager.h"

#include <UserInterfaceLayer/Menu/MenuView.h>
#include <UserInterfaceLayer/Menu/LoginDialog.h>

#include <3rd_party/Widgets/WAF/Animation.h>

using ManagementLayer::MenuManager;
using UserInterface::MenuView;
using UserInterface::LoginDialog;


MenuManager::MenuManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new MenuView(_parentWidget)),
	m_loginDialog(new LoginDialog(_parentWidget))
{
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
	WAF::Animation::sideSlideIn(m_view, WAF::LeftSide);
}

void MenuManager::showProjectSubmenu(const QString& _projectName)
{
	m_view->showProjectSubmenu(_projectName);
}

void MenuManager::hideProjectSubmenu()
{
	m_view->hideProjectSubmenu();
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
		WAF::Animation::sideSlideIn(m_loginDialog, WAF::TopSide);
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
		emit signInRequested(m_loginDialog->userName(), m_loginDialog->password());
		m_loginDialog->showProgressBar();
	});
	connect(m_loginDialog, &LoginDialog::cancelClicked, [=](){
		WAF::Animation::sideSlideOut(m_loginDialog);
	});
}

void MenuManager::initStyleSheet()
{
}

