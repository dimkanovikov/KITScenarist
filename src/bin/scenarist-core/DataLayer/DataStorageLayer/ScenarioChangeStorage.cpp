#include "ScenarioChangeStorage.h"

#include "SettingsStorage.h"

#include <DataLayer/Database/Database.h>
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
//		m_all = MapperFacade::scenarioChangeMapper()->findAll();
    }
    return m_all;
}

ScenarioChange* ScenarioChangeStorage::last()
{
    return all()->last();
}

ScenarioChange* ScenarioChangeStorage::append(const QString& _id, const QString& _datetime,
    const QString& _user, const QString& _undoPatch, const QString& _redoPatch, bool _isDraft)
{
    if (m_uuids.contains(_id)) {
        return nullptr;
    }

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
    // ... и на сохранение
    //
    allToSave()->append(change);

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
    DatabaseLayer::Database::transaction();
    foreach (DomainObject* domainObject, allToSave()->toList()) {
        ScenarioChange* change = dynamic_cast<ScenarioChange*>(domainObject);
        if (!change->id().isValid()) {
            MapperFacade::scenarioChangeMapper()->insert(change);
        }
    }
    DatabaseLayer::Database::commit();

    //
    // Очищаем список на сохранение
    //
    const bool DONT_REMOVE_ITEMS = false;
    allToSave()->clear(DONT_REMOVE_ITEMS);
}

void ScenarioChangeStorage::clear()
{
    delete m_all;
    m_all = 0;

    delete m_allToSave;
    m_allToSave = 0;

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
    const QString username = DataStorageLayer::StorageFacade::username();

    QList<QString> allNew;
    foreach (DomainObject* domainObject, all()->toList()) {
        ScenarioChange* change = dynamic_cast<ScenarioChange*>(domainObject);
        if (change->user() == username
            && change->datetime().toString("yyyy-MM-dd hh:mm:ss") > _fromDatetime) {
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

ScenarioChangesTable* ScenarioChangeStorage::allToSave()
{
    if (m_allToSave == 0) {
        m_allToSave = new ScenarioChangesTable;
    }
    return m_allToSave;
}

ScenarioChangeStorage::ScenarioChangeStorage() :
    m_all(0),
    m_allToSave(0)
{
}
