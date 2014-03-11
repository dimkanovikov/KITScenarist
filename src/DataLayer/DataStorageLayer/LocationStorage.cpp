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

Location* LocationStorage::updateLocation(const QString& _oldName, const QString& _newName)
{
	Location* locationToUpdate = 0;

	//
	// Если такая локация есть
	//
	if (hasLocation(_oldName)) {
		//
		// ... найдём её
		//
		foreach (DomainObject* domainObject, all()->toList()) {
			Location* location = dynamic_cast<Location*>(domainObject);
			if (location->name() == _oldName) {
				locationToUpdate = location;
				break;
			}
		}

		//
		// ... обновим
		//
		locationToUpdate->setName(_newName);

		//
		// ... и сохраним изменение в базе данных
		//
		MapperFacade::locationMapper()->update(locationToUpdate);

		//
		// ... уведомим об обновлении
		//
		int indexRow = all()->toList().indexOf(locationToUpdate);
		QModelIndex fromIndex = all()->index(indexRow, 0, QModelIndex());
		QModelIndex toIndex = all()->index(indexRow, all()->columnCount(fromIndex), QModelIndex());
		emit all()->dataChanged(fromIndex, toIndex);
	}

	return locationToUpdate;
}

void LocationStorage::removeLocation(const QString& _name)
{
	//
	// Если такой персонаж есть
	//
	if (hasLocation(_name)) {
		//
		// ... найдём его
		//
		Location* locationToDelete = 0;
		foreach (DomainObject* domainObject, all()->toList()) {
			Location* location = dynamic_cast<Location*>(domainObject);
			if (location->name() == _name) {
				locationToDelete = location;
				break;
			}
		}

		//
		// ... и удалим из локального списка и базы данных
		//
		all()->remove(locationToDelete);
		MapperFacade::locationMapper()->remove(locationToDelete);
	}
}

bool LocationStorage::hasLocation(const QString& _name)
{
	bool contains = false;
	foreach (DomainObject* domainObject, all()->toList()) {
		Location* location = dynamic_cast<Location*>(domainObject);
		if (location->name() == _name) {
			contains = true;
			break;
		}
	}
	return contains;
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
