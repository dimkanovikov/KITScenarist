#include "ScenarioChangeStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/ScenarioChangeMapper.h>

#include <Domain/ScenarioChange.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


ScenarioChangesTable* ScenarioChangeStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::scenarioChangeMapper()->findAll();
	}
	return m_all;
}

ScenarioChange* ScenarioChangeStorage::append(const QString& _user, const QString& _undoPatch,
	const QString& _redoPatch, bool _isDraft)
{
	//
	// Формируем изменение
	//
	ScenarioChange* change =
			new ScenarioChange(Identifier(), QUuid::createUuid(),
				QDateTime::currentDateTimeUtc(), _user, _undoPatch, _redoPatch, _isDraft);
	//
	// Добавляем его в список всех изменений
	//
	all()->append(change);

	//
	// Возвращаем клиенту
	//
	return change;
}

void ScenarioChangeStorage::store()
{
	//
	// Сохраняем все несохранённые изменения
	//
	foreach (DomainObject* domainObject, all()->toList()) {
		ScenarioChange* change = dynamic_cast<ScenarioChange*>(domainObject);
		if (!change->id().isValid()) {
			MapperFacade::scenarioChangeMapper()->insert(change);
		}
	}
}

void ScenarioChangeStorage::clear()
{
	delete m_all;
	m_all = 0;

	MapperFacade::scenarioChangeMapper()->clear();
}

ScenarioChangeStorage::ScenarioChangeStorage() :
	m_all(0)
{
}
