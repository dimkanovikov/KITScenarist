#include "TimeStorage.h"

#include <DataMapper/MapperFacade.h>
#include <DataMapper/TimeMapper.h>

using namespace StorageLayer;
using namespace DataMappingLayer;


TimesTable*TimeStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::timeMapper()->findAll();
	}
	return m_all;
}

TimeStorage::TimeStorage() :
	m_all(0)
{
}
