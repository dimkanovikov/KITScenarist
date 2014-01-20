#include "SettingsStorage.h"

#include <DataMapper/MapperFacade.h>
#include <DataMapper/SettingsMapper.h>

#include <QSettings>

using namespace StorageLayer;
using namespace DataMappingLayer;


void SettingsStorage::setValue(const QString& _key, const QString& _value, Settingsplace _settingsPlace)
{
	if (_settingsPlace == ApplicationSettings) {
		QSettings().setValue(_key, _value);
	} else {
		MapperFacade::settingsMapper()->setValue(_key, _value);
	}
}

QString SettingsStorage::value(const QString& _key, Settingsplace _settingsPlace)
{
	QString value;
	if (_settingsPlace == ApplicationSettings) {
		value = QSettings().value(_key, QString::null).toString();
	} else {
		value = MapperFacade::settingsMapper()->value(_key);
	}
	return value;
}

SettingsStorage::SettingsStorage()
{
}
