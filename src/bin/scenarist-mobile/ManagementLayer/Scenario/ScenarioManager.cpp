#include "ScenarioManager.h"

#include <UserInterfaceLayer/Scenario/ScenarioTextView.h>

using ManagementLayer::ScenarioManager;
using UserInterface::ScenarioTextView;


ScenarioManager::ScenarioManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ScenarioTextView(_parentWidget))
{

}

QWidget* ScenarioManager::toolbar() const
{
	return m_view->toolbar();
}

QWidget* ScenarioManager::view() const
{
	return m_view;
}

