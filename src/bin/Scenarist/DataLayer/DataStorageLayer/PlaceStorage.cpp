#include "PlaceStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/PlaceMapper.h>

#include <Domain/Place.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


PlacesTable* PlaceStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::placeMapper()->findAll();
	}
	return m_all;
}

Place* PlaceStorage::storePlace(const QString& _placeName)
{
	Place* newPlace = 0;

	QString placeName = _placeName.toUpper().simplified();

	//
	// Если место можно сохранить
	//
	if (!placeName.isEmpty()) {
		//
		// Проверяем наличие данного места
		//
		foreach (DomainObject* domainObject, all()->toList()) {
			Place* place = dynamic_cast<Place*>(domainObject);
			if (place->name() == placeName) {
				newPlace = place;
				break;
			}
		}

		//
		// Если такого места ещё нет, то сохраним его
		//
		if (!DomainObject::isValid(newPlace)) {
			newPlace = new Place(Identifier(), placeName);

			//
			// ... в базе данных
			//
			MapperFacade::placeMapper()->insert(newPlace);

			//
			// ... в списке
			//
			all()->append(newPlace);
		}
	}

	return newPlace;
}

bool PlaceStorage::hasPlace(const QString& _name)
{
	bool contains = false;
	foreach (DomainObject* domainObject, all()->toList()) {
		Place* place = dynamic_cast<Place*>(domainObject);
		if (place->name() == _name) {
			contains = true;
			break;
		}
	}
	return contains;
}

void PlaceStorage::clear()
{
	delete m_all;
	m_all = 0;

	MapperFacade::placeMapper()->clear();
}

PlaceStorage::PlaceStorage() :
	m_all(0)
{
}
