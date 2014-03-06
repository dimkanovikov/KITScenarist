#include "ScenarioNavigatorManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>

#include <UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigator.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

using ManagementLayer::ScenarioNavigatorManager;
using BusinessLogic::ScenarioModel;
using UserInterface::ScenarioNavigator;


ScenarioNavigatorManager::ScenarioNavigatorManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_scenarioModel(0),
	m_navigator(new ScenarioNavigator(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* ScenarioNavigatorManager::view() const
{
	return m_navigator;
}

void ScenarioNavigatorManager::setNavigationModel(ScenarioModel* _model)
{
	if (m_scenarioModel != 0) {
		m_scenarioModel->disconnect();
	}
	m_scenarioModel = _model;

	m_navigator->setModel(m_scenarioModel);

	initConnections();
}

void ScenarioNavigatorManager::aboutModelUpdated()
{
	m_navigator->setScenesCount(m_scenarioModel->scenesCount());
}

void ScenarioNavigatorManager::initView()
{
}

void ScenarioNavigatorManager::initConnections()
{
	if (m_scenarioModel != 0) {
		connect(m_scenarioModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
				this, SLOT(aboutModelUpdated()));
	}
}
