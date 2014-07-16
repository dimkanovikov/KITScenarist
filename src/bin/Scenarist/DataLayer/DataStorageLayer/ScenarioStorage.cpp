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

Scenario* ScenarioStorage::storeScenario(const QString& _name, const QString& _additionalInfo,
	const QString& _genre, const QString& _author, const QString& _contacts, const QString& _year,
	const QString& _synopsis, const QString& _text)
{
	Scenario* scenario = current();

	//
	// Если сценарий сохраняется в первый раз
	//
	if (scenario == 0) {
		//
		// ... создаём сценарий
		//
		scenario =
				new Scenario(Identifier(), _name, _additionalInfo, _genre, _author, _contacts,
					_year, _synopsis, _text);

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
		scenario->setAdditionalInfo(_additionalInfo);
		scenario->setGenre(_genre);
		scenario->setAuthor(_author);
		scenario->setContacts(_contacts);
		scenario->setYear(_year);
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

void ScenarioStorage::wait()
{
    MapperFacade::scenarioMapper()->wait();
}

ScenarioStorage::ScenarioStorage() :
	m_all(0)
{
}
