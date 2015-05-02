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

ScenarioChangesTable* ScenarioChangeStorage::allNew(const QString& _fromDatetime)
{
	ScenarioChangesTable* allNew = new ScenarioChangesTable;
	foreach (DomainObject* domainObject, all()->toList()) {
		ScenarioChange* change = dynamic_cast<ScenarioChange*>(domainObject);
		if (change->datetime().toString("yyyy-MM-dd hh:mm:ss") >= _fromDatetime) {
			allNew->append(change);
		}
	}
	return allNew;
}

ScenarioChange* ScenarioChangeStorage::append(const QString& _id, const QString& _datetime,
	const QString& _user, const QString& _undoPatch, const QString& _redoPatch, bool _isDraft)
{
	//
	// Формируем изменение
	//
	ScenarioChange* change =
			new ScenarioChange(Identifier(), QUuid(_id), QDateTime::fromString(_datetime, "yyyy-MM-dd hh:mm:ss"),
				_user, _undoPatch, _redoPatch, _isDraft);
	//
	// Добавляем его в список всех изменений
	//
	all()->append(change);

	//
	// Возвращаем клиенту
	//
	return change;
}

ScenarioChange* ScenarioChangeStorage::append(const QString& _user, const QString& _undoPatch,
	const QString& _redoPatch, bool _isDraft)
{
	return
			append(QUuid::createUuid().toString(), QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss"),
				_user, _undoPatch, _redoPatch, _isDraft);
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
