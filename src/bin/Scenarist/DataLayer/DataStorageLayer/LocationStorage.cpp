#include "LocationStorage.h"

#include "LocationPhotoStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/LocationMapper.h>

#include <Domain/Location.h>
#include <Domain/LocationPhoto.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


LocationsTable* LocationStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::locationMapper()->findAll();
	}
	return m_all;
}

Location* LocationStorage::location(const QString& _name)
{
	Location* resultLocation = 0;
	foreach (DomainObject* domainObject, all()->toList()) {
		Location* location = dynamic_cast<Location*>(domainObject);
		if (location->name() == _name) {
			resultLocation = location;
			break;
		}
	}
	return resultLocation;
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
			newLocation = new Location(Identifier(), locationName, QString(), new LocationPhotosTable);

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

void LocationStorage::updateLocation(Location* _location)
{
	//
	// Сохраним изменение в базе данных
	//
	MapperFacade::locationMapper()->update(_location);
	StorageFacade::locationPhotoStorage()->store(_location);

	//
	// Уведомим об обновлении
	//
	int indexRow = all()->toList().indexOf(_location);
	QModelIndex updateIndex = all()->index(indexRow, 0, QModelIndex());
	emit all()->dataChanged(updateIndex, updateIndex);
}

void LocationStorage::removeLocation(const QString& _name)
{
	//
	// Если такая локация есть
	//
	if (hasLocation(_name)) {
		//
		// ... найдём её
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
		// ... и удалим
		//
		// ...... фотографии
		StorageFacade::locationPhotoStorage()->remove(locationToDelete);
		// ...... из локального списка и базы данных
		all()->remove(locationToDelete);
		MapperFacade::locationMapper()->remove(locationToDelete);
	}
}

void LocationStorage::removeLocations(const QStringList& _names)
{
	foreach (const QString& name, _names) {
		removeLocation(name);
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

void LocationStorage::refresh()
{
	MapperFacade::locationMapper()->refresh(all());
}

LocationStorage::LocationStorage() :
	m_all(0)
{
}
