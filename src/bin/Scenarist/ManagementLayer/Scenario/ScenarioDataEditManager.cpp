#include "ScenarioDataEditManager.h"

#include <UserInterfaceLayer/Scenario/ScenarioDataEdit/ScenarioDataEdit.h>

using ManagementLayer::ScenarioDataEditManager;
using UserInterface::ScenarioDataEdit;


ScenarioDataEditManager::ScenarioDataEditManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_dataEdit(new ScenarioDataEdit(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* ScenarioDataEditManager::toolbar() const
{
	return m_dataEdit->toolbar();
}

QWidget* ScenarioDataEditManager::view() const
{
	return m_dataEdit;
}

void ScenarioDataEditManager::clear()
{
	m_dataEdit->clear();
}

QString ScenarioDataEditManager::scenarioName() const
{
	return m_dataEdit->scenarioName();
}

void ScenarioDataEditManager::setScenarioName(const QString& _name)
{
	m_dataEdit->setScenarioName(_name);
}

QString ScenarioDataEditManager::scenarioSynopsis() const
{
	return m_dataEdit->scenarioSynopsis();
}

void ScenarioDataEditManager::setScenarioSynopsis(const QString& _synopsis)
{
	m_dataEdit->setScenarioSynopsis(_synopsis);
}

void ScenarioDataEditManager::setScenarioSynopsisFromScenes(const QString& _synopsis)
{
	m_dataEdit->setScenarioSynopsisFromScenes(_synopsis);
}

void ScenarioDataEditManager::initView()
{

}

void ScenarioDataEditManager::initConnections()
{
	connect(m_dataEdit, SIGNAL(scenarioNameChanged()), this, SIGNAL(scenarioNameChanged()));
	connect(m_dataEdit, SIGNAL(scenarioSynopsisChanged()), this, SIGNAL(scenarioSynopsisChanged()));
	connect(m_dataEdit, SIGNAL(buildSynopsisFromScenes()), this, SIGNAL(buildSynopsisFromScenes()));
}
