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

LocationStorage::LocationStorage() :
	m_all(0),
	m_allMainLocations(0),
	m_allSublocations(0)
{
}
