#include "DatabaseHistoryStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/DatabaseHistoryMapper.h>

#include <QString>

using DataStorageLayer::DatabaseHistoryStorage;
using DataMappingLayer::MapperFacade;
using DataMappingLayer::DatabaseHistoryMapper;


QList<QString> DatabaseHistoryStorage::history(const QString& _fromDatetime)
{
    return MapperFacade::databaseHistoryMapper()->history(_fromDatetime);
}

QMap<QString, QString> DatabaseHistoryStorage::last()
{
    return MapperFacade::databaseHistoryMapper()->last();
}

QMap<QString, QString> DatabaseHistoryStorage::historyRecord(const QString& _uuid)
{
    return MapperFacade::databaseHistoryMapper()->historyRecord(_uuid);
}

bool DatabaseHistoryStorage::contains(const QString& _uuid) const
{
    return MapperFacade::databaseHistoryMapper()->contains(_uuid);
}

void DatabaseHistoryStorage::storeAndApplyHistoryRecord(const QString& _uuid, const QString& _query,
    const QString& _queryValues, const QString& _username, const QString& _datetime)
{
    MapperFacade::databaseHistoryMapper()->storeHistoryRecord(_uuid, _query, _queryValues, _username, _datetime);
    MapperFacade::databaseHistoryMapper()->applyHistoryRecord(_query, _queryValues);
}

DatabaseHistoryStorage::DatabaseHistoryStorage()
{
}
