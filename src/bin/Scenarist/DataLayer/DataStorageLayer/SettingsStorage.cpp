#include "SettingsStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/SettingsMapper.h>

#include <QSettings>
#include <QStringList>
#include <QUuid>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


void SettingsStorage::setValue(const QString& _key, const QString& _value, SettingsPlace _settingsPlace)
{
	if (_settingsPlace == ApplicationSettings) {
		QSettings().setValue(_key.toUtf8().toHex(), _value);
	} else {
		MapperFacade::settingsMapper()->setValue(_key, _value);
	}
}

void SettingsStorage::setValues(const QMap<QString, QString>& _values, const QString& _valuesGroup, SettingsStorage::SettingsPlace _settingsPlace)
{
	if (_settingsPlace == ApplicationSettings) {
		QSettings settings;

		//
		// Очистим группу
		//
		{
			settings.beginGroup(_valuesGroup);
			settings.remove("");
			settings.endGroup();
		}

		//
		// Откроем группу
		//
		settings.beginGroup(_valuesGroup);

		//
		// Сохраним значения
		//
		foreach (const QString& key, _values.keys()) {
			settings.setValue(key.toUtf8().toHex(), _values.value(key));
		}

		//
		// Закроем группу
		//
		settings.endGroup();
	}
	//
	// В базу данных карта параметров не умеет сохраняться
	//
	else {
		Q_ASSERT_X(0, Q_FUNC_INFO, "Database settings can't save group of settings");
	}

}

QString SettingsStorage::value(const QString& _key, SettingsPlace _settingsPlace)
{
	QString value;
	if (_settingsPlace == ApplicationSettings) {
		value = QSettings().value(_key.toUtf8().toHex(), QVariant()).toString();
	} else {
		value = MapperFacade::settingsMapper()->value(_key);
	}

	if (value.isNull()) {
		value = defaultValue(_key);
	}

	return value;
}

QMap<QString, QString> SettingsStorage::values(const QString& _valuesGroup, SettingsStorage::SettingsPlace _settingsPlace)
{
	QMap<QString, QString> settingsValues;

	if (_settingsPlace == ApplicationSettings) {
		QSettings settings;

		//
		// Откроем группу для считывания
		//
		settings.beginGroup(_valuesGroup);

		//
		// Получим все ключи
		//
		QStringList keys = settings.childKeys();

		//
		// Получим все значения
		//
		foreach (QString key, keys) {
			 settingsValues.insert(QByteArray::fromHex(key.toUtf8()), settings.value(key).toString());
		}

		//
		// Закроем группу
		//
		settings.endGroup();
	}
	//
	// Из базы данных карта параметров не умеет загружаться
	//
	else {
		Q_ASSERT_X(0, Q_FUNC_INFO, "Database settings can't load group of settings");
	}

	return settingsValues;
}

SettingsStorage::SettingsStorage()
{
	//
	// Настроим значения параметров по умолчанию
	//
	m_defaultValues.insert("application/uuid", QUuid::createUuid().toString());
	m_defaultValues.insert("application/use-dark-theme", "0");
	m_defaultValues.insert("application/autosave", "1");
	m_defaultValues.insert("application/autosave-interval", "5");

	m_defaultValues.insert("navigator/show-scene-description", "1");
	m_defaultValues.insert("navigator/scene-description-is-scene-text", "1");
	m_defaultValues.insert("navigator/scene-description-height", "1");

	m_defaultValues.insert("scenario-editor/zoom-range", "1");
	m_defaultValues.insert("scenario-editor/spell-checking", "0");
	m_defaultValues.insert("scenario-editor/text-color", "#000000");
	m_defaultValues.insert("scenario-editor/background-color", "#FEFEFE");
	m_defaultValues.insert("scenario-editor/nonprintable-text-color", "#0AC139");
	m_defaultValues.insert("scenario-editor/folder-text-color", "#FEFEFE");
	m_defaultValues.insert("scenario-editor/folder-background-color", "#CAC6C3");
	m_defaultValues.insert("scenario-editor/text-color-dark", "#EBEBEB");
	m_defaultValues.insert("scenario-editor/background-color-dark", "#3D3D3D");
	m_defaultValues.insert("scenario-editor/nonprintable-text-color-dark", "#0AC139");
	m_defaultValues.insert("scenario-editor/folder-text-color-dark", "#EBEBEB");
	m_defaultValues.insert("scenario-editor/folder-background-color-dark", "#8D2DC4");
	m_defaultValues.insert("scenario-editor/zoom-range", "0");

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
