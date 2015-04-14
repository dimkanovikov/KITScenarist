#include "DatabaseHistoryStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/DatabaseHistoryMapper.h>

#include <QString>

using DataStorageLayer::DatabaseHistoryStorage;
using DataMappingLayer::MapperFacade;
using DataMappingLayer::DatabaseHistoryMapper;


QList<QMap<QString, QString> > DatabaseHistoryStorage::history(const QString& _fromDatetime)
{
	return MapperFacade::databaseHistoryMapper()->history(_fromDatetime);
}

DatabaseHistoryStorage::DatabaseHistoryStorage()
{
}
