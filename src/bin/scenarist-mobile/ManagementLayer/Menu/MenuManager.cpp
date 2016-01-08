#include "MenuManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <UserInterfaceLayer/Menu/MenuView.h>

#include <3rd_party/Helpers/PasswordStorage.h>
#include <3rd_party/Widgets/WAF/Animation.h>

#include <QApplication>

using ManagementLayer::MenuManager;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;
using UserInterface::MenuView;


MenuManager::MenuManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new MenuView(_parentWidget))
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
	QApplication::processEvents();

	//
	// Выкатываем меню
	//
	WAF::Animation::sideSlideIn(m_view, WAF::LeftSide);
}

void MenuManager::userLogged()
{
	//
	// Загрузим имя пользователя из настроек
	//
	const QString userName =
		StorageFacade::settingsStorage()->value(
			"application/user-name",
			SettingsStorage::ApplicationSettings);
	m_view->setUserLogged(userName);
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
}

void MenuManager::initConnections()
{
	connect(m_view, &MenuView::backClicked, [=] {
		WAF::Animation::sideSlideOut(m_view);
	});
	connect(m_view, &MenuView::cabinClicked, [=] {
		WAF::Animation::sideSlideOut(m_view);
		emit cabinRequested();
	});
	connect(m_view, &MenuView::projectsClicked, [=] {
		WAF::Animation::sideSlideOut(m_view);
		emit projectsRequested();
	});
	connect(m_view, &MenuView::projectSaveClicked, [=] {
		WAF::Animation::sideSlideOut(m_view);
		emit projectSaveRequested();
	});
	connect(m_view, &MenuView::projectTextClicked, [=] {
		WAF::Animation::sideSlideOut(m_view);
		emit projectTextRequested();
	});
	connect(m_view, &MenuView::settingsClicked, [=] {
		WAF::Animation::sideSlideOut(m_view);
		emit settingsRequested();
	});
}

void MenuManager::initStyleSheet()
{
}

