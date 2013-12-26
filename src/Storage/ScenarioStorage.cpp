#include "ScenarioStorage.h"

#include <DataMapper/MapperFacade.h>
#include <DataMapper/ScenarioMapper.h>

#include <Domain/Scenario.h>

using namespace StorageLayer;
using namespace DataMappingLayer;


ScenariosTable* ScenarioStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::scenarioMapper()->findAll();
	}
	return m_all;
}

Scenario* ScenarioStorage::current()
{
	//
	// TODO: fix to real find current scenario
	//

	Scenario* currentScenario = 0;
	if (all()->count() > 0) {
		currentScenario = dynamic_cast<Scenario*>(all()->toList().first());
	}
	return currentScenario;
}

Scenario* ScenarioStorage::storeScenario(const QString& _scenarioText)
{
	Scenario* scenario = current();

	//
	// Если сценарий сохраняется в первый раз
	//
	if (scenario == 0) {
		//
		// ... создаём сценарий
		//
		scenario = new Scenario(Identifier(), _scenarioText);

		//
		// ... сохраним сценарий в базе данных
		//
		MapperFacade::scenarioMapper()->insert(scenario);

		//
		// ... добавим в список
		//
		m_all->append(scenario);
	}

	//
	// Если сценарий уже был сохранён
	//
	else {
		//
		// ... обновим текст сценария
		//
		scenario->setText(_scenarioText);

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
