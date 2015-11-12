#include "ScenarioDataStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/ScenarioDataMapper.h>

#include <Domain/ScenarioData.h>

using DataStorageLayer::ScenarioDataStorage;
using namespace DataMappingLayer;

namespace {
	/**
	 * @brief Ключи для доступа к данным
	 */
	/** @{ */
	const QString NAME_KEY = "name";
	const QString LOGLINE_KEY = "logline";
	const QString ADDITIONAL_INFO_KEY = "additional_info";
	const QString GENRE_KEY = "genre";
	const QString AUTHOR_KEY = "author";
	const QString CONTACTS_KEY = "contacts";
	const QString YEAR_KEY = "year";
	const QString SYNOPSIS_KEY = "synopsis";
	/** @} */
}


void ScenarioDataStorage::clear()
{
	delete m_all;
	m_all = 0;

	MapperFacade::scenarioDataMapper()->clear();
}

void ScenarioDataStorage::refresh()
{
	MapperFacade::scenarioDataMapper()->refresh(all());
}

QString ScenarioDataStorage::name() const
{
	return data(NAME_KEY)->value();
}

void ScenarioDataStorage::setName(const QString& _name)
{
	saveData(NAME_KEY, _name);
}

QString ScenarioDataStorage::logline() const
{
	return data(LOGLINE_KEY)->value();
}

void ScenarioDataStorage::setLogline(const QString& _logline)
{
	saveData(LOGLINE_KEY, _logline);
}

QString ScenarioDataStorage::additionalInfo() const
{
	return data(ADDITIONAL_INFO_KEY)->value();
}

void ScenarioDataStorage::setAdditionalInfo(const QString& _additionalInfo)
{
	saveData(ADDITIONAL_INFO_KEY, _additionalInfo);
}

QString ScenarioDataStorage::genre() const
{
	return data(GENRE_KEY)->value();
}

void ScenarioDataStorage::setGenre(const QString& _genre)
{
	saveData(GENRE_KEY, _genre);
}

QString ScenarioDataStorage::author() const
{
	return data(AUTHOR_KEY)->value();
}

void ScenarioDataStorage::setAuthor(const QString _author)
{
	saveData(AUTHOR_KEY, _author);
}

QString ScenarioDataStorage::contacts() const
{
	return data(CONTACTS_KEY)->value();
}

void ScenarioDataStorage::setContacts(const QString& _contacts)
{
	saveData(CONTACTS_KEY, _contacts);
}

QString ScenarioDataStorage::year() const
{
	return data(YEAR_KEY)->value();
}

void ScenarioDataStorage::setYear(const QString& _year)
{
	saveData(YEAR_KEY, _year);
}

QString ScenarioDataStorage::synopsis() const
{
	return data(SYNOPSIS_KEY)->value();
}

void ScenarioDataStorage::setSynopsis(const QString& _synopsis)
{
	saveData(SYNOPSIS_KEY, _synopsis);
}

ScenarioDataTable* ScenarioDataStorage::all() const
{
	if (m_all == 0) {
		m_all = MapperFacade::scenarioDataMapper()->findAll();
	}
	return m_all;
}

ScenarioData* ScenarioDataStorage::data(const QString& _name) const
{
	//
	// Ищем среди загруженных
	//
	ScenarioData* data = all()->scenarioData(_name);

	//
	// Если не нашли
	//
	if (data == 0) {
		//
		// ... создаём новый
		//
		data = new ScenarioData(Identifier(), _name, QString::null);
		//
		// ... сохраняем
		//
		MapperFacade::scenarioDataMapper()->insert(data);
		//
		// ... и запоминаем локально
		//
		all()->append(data);
	}

	return data;
}

void ScenarioDataStorage::saveData(const QString& _name, const QString& _newValue)
{
	ScenarioData* dataForUpdate = data(_name);
	if (dataForUpdate->value() != _newValue) {
		dataForUpdate->setValue(_newValue);
		MapperFacade::scenarioDataMapper()->update(dataForUpdate);
	}
}

ScenarioDataStorage::ScenarioDataStorage() :
	m_all(0)
{
}

