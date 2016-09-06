#include "ScenarioCardsManager.h"

#include "UserInterfaceLayer/Scenario/ScenarioCards/ScenarioCardsView.h"

using ManagementLayer::ScenarioCardsManager;
using UserInterface::ScenarioCardsView;

ScenarioCardsManager::ScenarioCardsManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ScenarioCardsView(_parentWidget))
{

}

QWidget* ScenarioCardsManager::view() const
{
	return m_view;
}
