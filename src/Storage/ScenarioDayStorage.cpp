#include "ScenarioDayStorage.h"

#include <DataMapper/MapperFacade.h>
#include <DataMapper/ScenarioDayMapper.h>

using namespace StorageLayer;
using namespace DataMappingLayer;


ScenarioDaysTable*ScenarioDayStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::scenarioDayMapper()->findAll();
	}
	return m_all;
}

ScenarioDayStorage::ScenarioDayStorage() :
	m_all(0)
{
}
