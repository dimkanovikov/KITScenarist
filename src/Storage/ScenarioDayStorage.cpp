#include "ScenarioDayStorage.h"

#include <DataMapper/MapperFacade.h>
#include <DataMapper/ScenarioDayMapper.h>

#include <Domain/ScenarioDay.h>

using namespace StorageLayer;
using namespace DataMappingLayer;


ScenarioDaysTable* ScenarioDayStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::scenarioDayMapper()->findAll();
	}
	return m_all;
}

ScenarioDay* ScenarioDayStorage::storeScenarioDay(const QString& _scenarioDayName)
{
	ScenarioDay* newScenarioDay = 0;

	QString scenarioDayName = _scenarioDayName.toUpper().simplified();

	//
	// Если сценарный день можно сохранить
	//
	if (!scenarioDayName.isEmpty()) {
		//
		// Проверяем наличии данного сценарного дня
		//
		foreach (DomainObject* domainObject, all()->toList()) {
			ScenarioDay* scenarioDay = dynamic_cast<ScenarioDay*>(domainObject);
			if (scenarioDay->name() == scenarioDayName) {
				newScenarioDay = scenarioDay;
				break;
			}
		}

		//
		// Если такого сценарного дня ещё нет, то сохраним его
		//
		if (!DomainObject::isValid(newScenarioDay)) {
			newScenarioDay = new ScenarioDay(Identifier(), scenarioDayName);

			//
			// ... в базе данных
			//
			MapperFacade::scenarioDayMapper()->insert(newScenarioDay);

			//
			// ... в списке
			//
			all()->append(newScenarioDay);
		}
	}

	return newScenarioDay;
}

void ScenarioDayStorage::clear()
{
	delete m_all;
	m_all = 0;

	MapperFacade::scenarioDayMapper()->clear();
}

ScenarioDayStorage::ScenarioDayStorage() :
	m_all(0)
{
}
