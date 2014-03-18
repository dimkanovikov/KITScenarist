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

void PlaceStorage::clear()
{
	delete m_all;
	m_all = 0;

	MapperFacade::placeMapper()->clear();
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

PlaceStorage::PlaceStorage() :
	m_all(0)
{
}
