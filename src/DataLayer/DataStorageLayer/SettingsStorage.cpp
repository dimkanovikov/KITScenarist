#include "SettingsStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/SettingsMapper.h>

#include <QSettings>

using namespace DataStorageLayer;
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
		value = QSettings().value(_key, QVariant()).toString();
	} else {
		value = MapperFacade::settingsMapper()->value(_key);
	}

	if (value.isNull()) {
		value = defaultValue(_key);
	}

	return value;
}

SettingsStorage::SettingsStorage()
{
	//
	// Настроим значения параметров по умолчанию
	//
	m_defaultValues.insert("text-editor/spell-checking", "0");
	m_defaultValues.insert("chronometry/current-chronometer-type", "pages-chronometer");
	m_defaultValues.insert("chronometry/pages/seconds", "60");
	m_defaultValues.insert("chronometry/characters/characters", "1000");
	m_defaultValues.insert("chronometry/characters/seconds", "60");
	m_defaultValues.insert("chronometry/configurable/seconds-for-paragraph/time-and-place", "2");
	m_defaultValues.insert("chronometry/configurable/seconds-for-every-50/time-and-place", "0");
	m_defaultValues.insert("chronometry/configurable/seconds-for-paragraph/action", "1");
	m_defaultValues.insert("chronometry/configurable/seconds-for-every-50/action", "1.5");
	m_defaultValues.insert("chronometry/configurable/seconds-for-paragraph/dialog", "2");
	m_defaultValues.insert("chronometry/configurable/seconds-for-every-50/dialog", "2.4");
}

QString SettingsStorage::defaultValue(const QString& _key) const
{
	return m_defaultValues.value(_key, QString());
}
