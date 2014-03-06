#include "ScenarioTextEditManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEditWidget.h>

using ManagementLayer::ScenarioTextEditManager;
using BusinessLogic::ScenarioDocument;
using UserInterface::ScenarioTextEditWidget;


ScenarioTextEditManager::ScenarioTextEditManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ScenarioTextEditWidget(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* ScenarioTextEditManager::view() const
{
	return m_view;
}

void ScenarioTextEditManager::setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document)
{
	m_view->setScenarioDocument(_document);
}

void ScenarioTextEditManager::setDuration(const QString& _duration)
{
	m_view->setDuration(_duration);
}

void ScenarioTextEditManager::initView()
{

}

void ScenarioTextEditManager::initConnections()
{
	connect(m_view, SIGNAL(cursorPositionChanged(int)), this, SIGNAL(cursorPositionChanged(int)));
}
