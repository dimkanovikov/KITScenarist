#include "LocationStorage.h"

#include <DataMapper/MapperFacade.h>
#include <DataMapper/LocationMapper.h>

#include <Domain/Location.h>

using namespace StorageLayer;
using namespace DataMappingLayer;


LocationsTable* LocationStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::locationMapper()->findAll();
	}
	return m_all;
}

LocationsTable* LocationStorage::allMainLocations()
{
	if (m_allMainLocations == 0) {
		m_allMainLocations = new LocationsTable;
		foreach (DomainObject* domainObject, all()->toList()) {
			Location* location = dynamic_cast<Location*>(domainObject);
			if (!DomainObject::isValid(location->parentLocation())) {
				m_allMainLocations->append(location);
			}
		}
	}
	return m_allMainLocations;
}

LocationsTable* LocationStorage::allSublocations()
{
	if (m_allSublocations == 0) {
		m_allSublocations = new LocationsTable;
		foreach (DomainObject* domainObject, all()->toList()) {
			Location* location = dynamic_cast<Location*>(domainObject);
			if (DomainObject::isValid(location->parentLocation())) {
				m_allSublocations->append(location);
			}
		}
	}
	return m_allSublocations;
}

LocationsTable* LocationStorage::sublocations(const QString& _locationName)
{
	//
	// Если не инициилизирован, то нужно это сделать
	//
	if (m_sublocations.count() != allMainLocations()->count()) {
		foreach (DomainObject* domainObject, allMainLocations()->toList()) {
			//
			// Сформируем список подлокаций для каждой локации
			//
			Location* location = dynamic_cast<Location*>(domainObject);
			LocationsTable* sublocations = new LocationsTable;
			foreach (DomainObject* subDomainObject, allSublocations()->toList()) {
				Location* sublocation = dynamic_cast<Location*>(subDomainObject);
				if (sublocation->parentLocation() == location) {
					sublocations->append(sublocation);
				}
			}

			//
			// Сохраним
			//
			m_sublocations.insert(location, sublocations);
		}
	}

	//
	// Определим локацию
	//
	Location* mainLocation = 0;
	foreach (DomainObject* domainObject, allMainLocations()->toList()) {
		Location* location = dynamic_cast<Location*>(domainObject);
		if (location->name().compare(_locationName, Qt::CaseInsensitive) == 0) {
			mainLocation = location;
			break;
		}
	}

	return m_sublocations.value(mainLocation, 0);
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
		foreach (DomainObject* domainObject, m_all->toList()) {
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
			newLocation = new Location(Identifier(), 0, locationName);

			//
			// ... в базе данных
			//
			MapperFacade::locationMapper()->insert(newLocation);

			//
			// ... в списках
			//
			all()->append(newLocation);
			allMainLocations()->append(newLocation);
			m_sublocations.insert(newLocation, new LocationsTable);
		}
	}

	return newLocation;
}

Location* LocationStorage::storeSublocation(Location* _parent, const QString& _sublocationName)
{
	Location* newSublocation = 0;

	QString sublocationName = _sublocationName.toUpper().simplified();

	//
	// Если подлокацию можно сохранить
	//
	if (DomainObject::isValid(_parent)
		&& !sublocationName.isEmpty()) {
		//
		// Проверяем наличии данной подлокации
		//
		foreach (DomainObject* domainObject, sublocations(_parent->name())->toList()) {
			Location* sublocation = dynamic_cast<Location*>(domainObject);
			if (sublocation->name() == sublocationName) {
				newSublocation = sublocation;
				break;
			}
		}

		//
		// Если такой подлокации ещё нет, то сохраним её
		//
		if (!DomainObject::isValid(newSublocation)) {
			newSublocation = new Location(Identifier(), _parent, sublocationName);

			//
			// ... в базе данных
			//
			MapperFacade::locationMapper()->insert(newSublocation);

			//
			// ... в списках
			//
			all()->append(newSublocation);
			allSublocations()->append(newSublocation);
			m_sublocations[_parent]->append(newSublocation);
		}
	}

	return newSublocation;
}

void LocationStorage::storeLocationWithSublocation(const QString& _locationName, const QString& _sublocationName)
{
	//
	// Сохраним основную локацию
	//
	Location* location = storeLocation(_locationName);

	//
	// Сохраним подлокацию
	//
	storeSublocation(location, _sublocationName);
}

void LocationStorage::clear()
{
	delete m_all;
	m_all = 0;

	delete m_allMainLocations;
	m_allMainLocations = 0;

	delete m_allSublocations;
	m_allSublocations = 0;

	m_sublocations.clear();

	MapperFacade::locationMapper()->clear();
}

LocationStorage::LocationStorage() :
	m_all(0),
	m_allMainLocations(0),
	m_allSublocations(0)
{
}
