#include "ScenarioSceneSynopsisManager.h"

#include <UserInterfaceLayer/Scenario/ScenarioSceneSynopsis/ScenarioSceneSynopsis.h>

using ManagementLayer::ScenarioSceneSynopsisManager;
using UserInterface::ScenarioSceneSynopsis;


ScenarioSceneSynopsisManager::ScenarioSceneSynopsisManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_sceneSynopsis(new ScenarioSceneSynopsis(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* ScenarioSceneSynopsisManager::view() const
{
	return m_sceneSynopsis;
}

void ScenarioSceneSynopsisManager::setHeader(const QString& _header)
{
	m_sceneSynopsis->setHeader(_header);
}

void ScenarioSceneSynopsisManager::setSynopsis(const QString& _synopsis)
{
	m_sceneSynopsis->setSynopsis(_synopsis);
}

void ScenarioSceneSynopsisManager::initView()
{

}

void ScenarioSceneSynopsisManager::initConnections()
{
	connect(m_sceneSynopsis, SIGNAL(synopsisChanged(QString)), this, SIGNAL(synopsisChanged(QString)));
}
