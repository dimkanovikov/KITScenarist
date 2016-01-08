#include "CabinManager.h"

#include <UserInterfaceLayer/Cabin/CabinView.h>

using ManagementLayer::CabinManager;
using UserInterface::CabinView;


CabinManager::CabinManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new CabinView(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* CabinManager::toolbar() const
{
	return m_view->toolbar();
}

QWidget* CabinManager::view() const
{
	return m_view;
}

void CabinManager::initView()
{

}

void CabinManager::initConnections()
{
	connect(m_view, &CabinView::logoutRequested, this, &CabinManager::logoutRequested);
}
