#include "SettingsStorage.h"

#include <DataMapper/MapperFacade.h>
#include <DataMapper/SettingsMapper.h>

using namespace StorageLayer;
using namespace DataMappingLayer;


void SettingsStorage::setValue(const QString& _key, const QString& _value)
{
	MapperFacade::settingsMapper()->setValue(_key, _value);
}

QString SettingsStorage::value(const QString& _key)
{
	return MapperFacade::settingsMapper()->value(_key);
}

SettingsStorage::SettingsStorage()
{
}
