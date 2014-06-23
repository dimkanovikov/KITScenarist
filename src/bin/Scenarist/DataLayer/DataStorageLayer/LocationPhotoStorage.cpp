#include "LocationPhotoStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/LocationPhotoMapper.h>

#include <Domain/Location.h>
#include <Domain/LocationPhoto.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


LocationPhotosTable* LocationPhotoStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::locationPhotoMapper()->findAll();
	}
	return m_all;
}

void LocationPhotoStorage::store(Location* _location)
{
	//
	// Удалить все старые фотографии
	//
	remove(_location);

	//
	// Сохранить новые фотографии
	//
	LocationPhotosTable* newPhotos = _location->photosTable();
	foreach (DomainObject* domainObject, newPhotos->toList()) {
		LocationPhoto* newPhoto = dynamic_cast<LocationPhoto*>(domainObject);

		//
		// Новое фото
		//
		if (!newPhoto->id().isValid()) {
			MapperFacade::locationPhotoMapper()->insert(newPhoto);
			all()->append(newPhoto);
		}
		//
		// Старое фото
		//
		else {
			MapperFacade::locationPhotoMapper()->update(newPhoto);
		}
	}
}

void LocationPhotoStorage::remove(Location* _location)
{
	LocationPhotosTable* newPhotos = _location->photosTable();

	//
	// Если не все фотографии изменились, то старые не нужно удалять
	//
	LocationPhotosTable* photos =
			MapperFacade::locationPhotoMapper()->findAllForLocation(_location->id());
	foreach (DomainObject* domainObject, photos->toList()) {
		LocationPhoto* oldPhoto = dynamic_cast<LocationPhoto*>(domainObject);

		if (!newPhotos->contains(oldPhoto)) {
			all()->remove(oldPhoto);
			MapperFacade::locationPhotoMapper()->remove(oldPhoto);
		}
	}
	delete photos;
	photos = 0;
}

void LocationPhotoStorage::clear()
{
	delete m_all;
	m_all = 0;

    MapperFacade::locationPhotoMapper()->clear();
}

void LocationPhotoStorage::wait()
{
    MapperFacade::locationPhotoMapper()->wait();
}

LocationPhotoStorage::LocationPhotoStorage() :
	m_all(0)
{
}
