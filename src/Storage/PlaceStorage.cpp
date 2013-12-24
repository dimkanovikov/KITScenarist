#include "PlaceStorage.h"

#include <DataMapper/MapperFacade.h>
#include <DataMapper/PlaceMapper.h>

#include <Domain/Place.h>

using namespace StorageLayer;
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

PlaceStorage::PlaceStorage() :
	m_all(0)
{
}
