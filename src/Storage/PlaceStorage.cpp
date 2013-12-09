#include "PlaceStorage.h"

#include <DataMapper/MapperFacade.h>
#include <DataMapper/PlaceMapper.h>

using namespace StorageLayer;
using namespace DataMappingLayer;


PlacesTable* PlaceStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::placeMapper()->findAll();
	}
	return m_all;
}

PlaceStorage::PlaceStorage() :
	m_all(0)
{
}
