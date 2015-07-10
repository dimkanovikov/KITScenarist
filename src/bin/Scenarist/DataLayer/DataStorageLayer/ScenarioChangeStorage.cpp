#include "ScenarioChangeStorage.h"

#include "SettingsStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/ScenarioChangeMapper.h>

#include <Domain/ScenarioChange.h>

#include <3rd_party/Helpers/PasswordStorage.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


ScenarioChangesTable* ScenarioChangeStorage::all()
{
	//
	// Не загружаем старые значения, работаем только с новыми,
	// сделанными с момента открытия проекта
	//
	if (m_all == 0) {
		m_all = MapperFacade::scenarioChangeMapper()->findLastOne();
	}
	return m_all;
}

ScenarioChange* ScenarioChangeStorage::append(const QString& _id, const QString& _datetime,
	const QString& _user, const QString& _undoPatch, const QString& _redoPatch, bool _isDraft)
{
	QDateTime changeDatetime = QDateTime::fromString(_datetime, "yyyy-MM-dd hh:mm:ss");

	//
	// Формируем изменение
	//
	ScenarioChange* change =
			new ScenarioChange(Identifier(), QUuid(_id), changeDatetime,
				_user, _undoPatch, _redoPatch, _isDraft);
	//
	// Добавляем его в список всех изменений
	//
	all()->append(change);

	//
	// Сохраняем идентификатор в списке
	//
	m_uuids.insert(_id);

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

bool ScenarioChangeStorage::contains(const QString& _uuid)
{
	bool contains = false;

	//
	// Проверяем в новых изменениях ещё не сохранённых в БД
	//
	contains = m_uuids.contains(_uuid);

	//
	// Проверяем в БД
	//
	if (!contains) {
		contains = MapperFacade::scenarioChangeMapper()->containsUuid(_uuid);
	}

	return contains;
}

QList<QString> ScenarioChangeStorage::uuids() const
{
	return MapperFacade::scenarioChangeMapper()->uuids();
}

QList<QString> ScenarioChangeStorage::newUuids(const QString& _fromDatetime)
{
	//
	// Отправляем изменения только от локального пользователя
	//
	const QString username =
			StorageFacade::settingsStorage()->value(
				"application/user-name", SettingsStorage::ApplicationSettings);

	QList<QString> allNew;
	foreach (DomainObject* domainObject, all()->toList()) {
		ScenarioChange* change = dynamic_cast<ScenarioChange*>(domainObject);
		if (change->user() == username
			&& change->datetime().toString("yyyy-MM-dd hh:mm:ss") >= _fromDatetime) {
			allNew.append(change->uuid().toString());
		}
	}
	return allNew;
}

ScenarioChange ScenarioChangeStorage::change(const QString& _uuid)
{
	//
	// Если изменение ещё не сохранено, берём его из списке несохранённых
	//
	if (m_uuids.contains(_uuid)) {
		foreach (DomainObject* domainObject, all()->toList()) {
			ScenarioChange* change = dynamic_cast<ScenarioChange*>(domainObject);
			if (change->uuid().toString() == _uuid) {
				return *change;
			}
		}
	}

	//
	// А если сохранено, то из БД
	//
	return MapperFacade::scenarioChangeMapper()->change(_uuid);
}

ScenarioChangeStorage::ScenarioChangeStorage() :
	m_all(0)
{
}
