#include "LocationStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/LocationMapper.h>

#include <Domain/Location.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


LocationsTable* LocationStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::locationMapper()->findAll();
	}
	return m_all;
}

Location* LocationStorage::storeLocation(const QString& _locationName)
{
	Location* newLocation = 0;

	QString locationName = _locationName.toUpper().simplified();

	//
	// Если локацию можно сохранить
	//
	if (!locationName.isEmpty()) {
		//
		// Проверяем наличии данной локации
		//
		foreach (DomainObject* domainObject, all()->toList()) {
			Location* location = dynamic_cast<Location*>(domainObject);
			if (location->name() == locationName) {
				newLocation = location;
				break;
			}
		}

		//
		// Если такой локации ещё нет, то сохраним её
		//
		if (!DomainObject::isValid(newLocation)) {
			newLocation = new Location(Identifier(), locationName);

			//
			// ... в базе данных
			//
			MapperFacade::locationMapper()->insert(newLocation);

			//
			// ... в списках
			//
			all()->append(newLocation);
		}
	}

	return newLocation;
}

void LocationStorage::clear()
{
	delete m_all;
	m_all = 0;

	MapperFacade::locationMapper()->clear();
}

LocationStorage::LocationStorage() :
	m_all(0)
{
}
