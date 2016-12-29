#include "ScenarioStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/ScenarioMapper.h>

#include <BusinessLayer/ScenarioDocument/ScenarioXml.h>

#include <Domain/Scenario.h>

#include <QUuid>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


ScenariosTable* ScenarioStorage::all()
{
	if (m_all == nullptr) {
		m_all = MapperFacade::scenarioMapper()->findAll();
	}
	return m_all;
}

Scenario* ScenarioStorage::current(bool _isDraft)
{
	Scenario* currentScenario = nullptr;
	if (all()->size() > 0) {
		foreach (DomainObject* domainObject, all()->toList()) {
			if (Scenario* scenario = dynamic_cast<Scenario*>(domainObject)) {
				if (scenario->isDraft() == _isDraft) {
					currentScenario = scenario;
					break;
				}
			}
		}
	}

	//
	// Если сценарий ещё не был создан
	//
	if (currentScenario == nullptr) {
		//
		// ... создаём сценарий
		//
		currentScenario =
			new Scenario(Identifier(), BusinessLogic::ScenarioXml::defaultCardsXml(),
						 BusinessLogic::ScenarioXml::defaultTextXml(), _isDraft);
		//
		// ... сохраним сценарий в базе данных
		//
		MapperFacade::scenarioMapper()->insert(currentScenario);
		//
		// ... добавим в список
		//
		all()->append(currentScenario);
	}

	return currentScenario;
}

void ScenarioStorage::storeScenario(Scenario* _scenario)
{
	Q_ASSERT(_scenario);

	MapperFacade::scenarioMapper()->update(_scenario);
}

void ScenarioStorage::clear()
{
	delete m_all;
	m_all = nullptr;

	MapperFacade::scenarioMapper()->clear();
}

ScenarioStorage::ScenarioStorage() :
	m_all(nullptr)
{
}
