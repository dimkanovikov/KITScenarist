#include "ScenarioSceneSynopsisManager.h"

#include <UserInterfaceLayer/Scenario/ScenarioSceneSynopsis/ScenarioSceneSynopsis.h>

using ManagementLayer::ScenarioSceneSynopsisManager;
using UserInterface::ScenarioSceneSynopsis;


ScenarioSceneSynopsisManager::ScenarioSceneSynopsisManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ScenarioSceneSynopsis(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* ScenarioSceneSynopsisManager::view() const
{
	return m_view;
}

void ScenarioSceneSynopsisManager::setHeader(const QString& _header)
{
	m_view->setHeader(_header);
}

void ScenarioSceneSynopsisManager::setSynopsis(const QString& _synopsis)
{
	m_view->setSynopsis(_synopsis);
}

void ScenarioSceneSynopsisManager::setCommentOnly(bool _isCommentOnly)
{
	m_view->setCommentOnly(_isCommentOnly);
}

void ScenarioSceneSynopsisManager::initView()
{
	m_view->setObjectName("scenarioSceneSynopsisView");
}

void ScenarioSceneSynopsisManager::initConnections()
{
	connect(m_view, SIGNAL(synopsisChanged(QString)), this, SIGNAL(synopsisChanged(QString)));
}
