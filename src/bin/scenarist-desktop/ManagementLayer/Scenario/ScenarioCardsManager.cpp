#include "ScenarioCardsManager.h"

#include <Domain/Scenario.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>

#include <UserInterfaceLayer/Scenario/ScenarioCards/ScenarioCardsView.h>

using ManagementLayer::ScenarioCardsManager;
using UserInterface::ScenarioCardsView;

ScenarioCardsManager::ScenarioCardsManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ScenarioCardsView(_parentWidget)),
	m_scenario(nullptr)
{
	initConnections();
}

QWidget* ScenarioCardsManager::view() const
{
	return m_view;
}

void ScenarioCardsManager::loadCurrentProject()
{
	//
	// Загрузим сценарий
	//
	m_scenario = DataStorageLayer::StorageFacade::scenarioStorage()->current();
	QString scheme = m_scenario->scheme();
	//
	// ... если схема пуста, просим сформировать для нас её черновой вариант из текста
	//
	if (scheme.isEmpty()) {
		emit needDirtyScheme();
	}
	//
	// ... а если схема есть, то просто загружаем её
	//
	else {
		m_view->load(m_scenario->scheme());
	}

	//
	// TODO: загружать и черновик и чистовик, и использовать схему в соответствии с текущим режимом работы
	//
}

void ScenarioCardsManager::loadCurrentProjectSettings(const QString& _projectPath)
{
	//
	// TODO: текущий режим чистовик/черновик
	//
}

void ScenarioCardsManager::saveCurrentProject()
{
	//
	// Сохраняем сценарий
	//
	m_scenario->setScheme(m_view->save());
	DataStorageLayer::StorageFacade::scenarioStorage()->storeScenario(m_scenario);

	//
	// TODO: сохранять черновик
	//
}

void ScenarioCardsManager::saveCurrentProjectSettings(const QString& _projectPath)
{
	//
	// TODO: сохранять последний используемый режим
	//
}

void ScenarioCardsManager::closeCurrentProject()
{
	m_view->clear();
}

void ScenarioCardsManager::setCommentOnly(bool _isCommentOnly)
{
	m_view->setCommentOnly(_isCommentOnly);
}

void ScenarioCardsManager::initConnections()
{
	connect(m_view, &ScenarioCardsView::schemeChanged, this, &ScenarioCardsManager::schemeChanged);
}
