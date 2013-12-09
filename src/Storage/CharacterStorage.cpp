#include "CharacterStorage.h"

#include <DataMapper/MapperFacade.h>
#include <DataMapper/CharacterMapper.h>

using namespace StorageLayer;
using namespace DataMappingLayer;


CharactersTable*CharacterStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::characterMapper()->findAll();
	}
	return m_all;
}

CharacterStorage::CharacterStorage() :
	m_all(0)
{
}
