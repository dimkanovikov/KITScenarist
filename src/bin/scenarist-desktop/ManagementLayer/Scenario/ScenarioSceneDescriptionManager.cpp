#include "ScenarioSceneDescriptionManager.h"

#include <UserInterfaceLayer/Scenario/ScenarioSceneDescription/ScenarioSceneDescription.h>

using ManagementLayer::ScenarioSceneDescriptionManager;
using UserInterface::ScenarioSceneDescription;


ScenarioSceneDescriptionManager::ScenarioSceneDescriptionManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ScenarioSceneDescription(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* ScenarioSceneDescriptionManager::view() const
{
	return m_view;
}

void ScenarioSceneDescriptionManager::setTitle(const QString& _title)
{
	m_view->setTitle(_title);
}

void ScenarioSceneDescriptionManager::setDescription(const QString& _description)
{
	m_view->setDescription(_description);
}

void ScenarioSceneDescriptionManager::setCommentOnly(bool _isCommentOnly)
{
	m_view->setCommentOnly(_isCommentOnly);
}

void ScenarioSceneDescriptionManager::initView()
{
	m_view->setObjectName("scenarioSceneDescriptionView");
}

void ScenarioSceneDescriptionManager::initConnections()
{
	connect(m_view, &ScenarioSceneDescription::titleChanged, this, &ScenarioSceneDescriptionManager::titleChanged);
    connect(m_view, &ScenarioSceneDescription::copyDescriptionToScriptPressed, this, &ScenarioSceneDescriptionManager::copyDescriptionToScriptRequested);
	connect(m_view, &ScenarioSceneDescription::descriptionChanged, this, &ScenarioSceneDescriptionManager::descriptionChanged);
}
