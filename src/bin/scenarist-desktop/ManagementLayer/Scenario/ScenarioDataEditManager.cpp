#include "ScenarioDataEditManager.h"

#include <UserInterfaceLayer/Scenario/ScenarioDataEdit/ScenarioDataEdit.h>

#include <3rd_party/Helpers/TextEditHelper.h>

using ManagementLayer::ScenarioDataEditManager;
using UserInterface::ScenarioDataEdit;


ScenarioDataEditManager::ScenarioDataEditManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ScenarioDataEdit(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* ScenarioDataEditManager::toolbar() const
{
	return m_view->toolbar();
}

QWidget* ScenarioDataEditManager::view() const
{
	return m_view;
}

void ScenarioDataEditManager::clear()
{
	m_view->clear();
}

QString ScenarioDataEditManager::scenarioName() const
{
	return m_view->scenarioName();
}

void ScenarioDataEditManager::setScenarioName(const QString& _name)
{
	m_view->setScenarioName(_name);
}

QString ScenarioDataEditManager::scenarioSynopsis() const
{
	return TextEditHelper::removeDocumentTags(m_view->scenarioSynopsis());;
}

void ScenarioDataEditManager::setScenarioSynopsis(const QString& _synopsis)
{
	m_view->setScenarioSynopsis(_synopsis);
}

void ScenarioDataEditManager::setScenarioSynopsisFromScenes(const QString& _synopsis)
{
	m_view->setScenarioSynopsisFromScenes(_synopsis);
}

void ScenarioDataEditManager::setCommentOnly(bool _isCommentOnly)
{
	m_view->setCommentOnly(_isCommentOnly);
}

void ScenarioDataEditManager::initView()
{

}

void ScenarioDataEditManager::initConnections()
{
	connect(m_view, SIGNAL(scenarioNameChanged()), this, SIGNAL(scenarioNameChanged()));
	connect(m_view, SIGNAL(scenarioSynopsisChanged()), this, SIGNAL(scenarioSynopsisChanged()));
	connect(m_view, SIGNAL(buildSynopsisFromScenes()), this, SIGNAL(buildSynopsisFromScenes()));
}
