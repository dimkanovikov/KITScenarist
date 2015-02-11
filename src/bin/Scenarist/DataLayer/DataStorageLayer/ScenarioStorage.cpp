#include "ScenarioStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/ScenarioMapper.h>

#include <Domain/Scenario.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


ScenariosTable* ScenarioStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::scenarioMapper()->findAll();
	}
	return m_all;
}

Scenario* ScenarioStorage::current(bool _isDraft)
{
	Scenario* currentScenario = 0;
	if (all()->count() > 0) {
		foreach (DomainObject* domainObject, all()->toList()) {
			if (Scenario* scenario = dynamic_cast<Scenario*>(domainObject)) {
				if (scenario->isDraft() == _isDraft) {
					currentScenario = scenario;
					break;
				}
			}
		}
	}
	return currentScenario;
}

Scenario* ScenarioStorage::storeScenario(const QString& _name, const QString& _synopsis,
	const QString& _text, bool _isDraft)
{
	Scenario* scenario = current(_isDraft);

	//
	// Если сценарий сохраняется в первый раз
	//
	if (scenario == 0) {
		//
		// ... создаём сценарий
		//
		scenario = new Scenario(Identifier(), _name, _synopsis, _text);
		scenario->setIsDraft(_isDraft);

		//
		// ... сохраним сценарий в базе данных
		//
		MapperFacade::scenarioMapper()->insert(scenario);

		//
		// ... добавим в список
		//
		all()->append(scenario);
	}

	//
	// Если сценарий уже был сохранён
	//
	else {
		//
		// ... обновим сценарий
		//
		scenario->setName(_name);
		scenario->setSynopsis(_synopsis);
		scenario->setText(_text);

		//
		// ... зафиксируем обновлённый текст в базе данных
		//
		MapperFacade::scenarioMapper()->update(scenario);
	}

	return scenario;
}

void ScenarioStorage::clear()
{
	delete m_all;
	m_all = 0;

	MapperFacade::scenarioMapper()->clear();
}

ScenarioStorage::ScenarioStorage() :
	m_all(0)
{
}
