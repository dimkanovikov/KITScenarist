#include "MenuManager.h"

#include <UserInterfaceLayer/Menu/MenuView.h>

using ManagementLayer::MenuManager;
using UserInterface::MenuView;


MenuManager::MenuManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new MenuView(_parentWidget))
{
	initView();
	initConnections();
	initStyleSheet();
}

QWidget* MenuManager::view() const
{
	return m_view;
}

void MenuManager::initView()
{
	m_view->hide();
}

void MenuManager::initConnections()
{

}

void MenuManager::initStyleSheet()
{
}

