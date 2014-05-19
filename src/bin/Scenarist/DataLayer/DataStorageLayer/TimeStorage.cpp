#include "TimeStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/TimeMapper.h>

#include <Domain/Time.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


TimesTable* TimeStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::timeMapper()->findAll();
	}
	return m_all;
}

void TimeStorage::clear()
{
	delete m_all;
	m_all = 0;

    MapperFacade::timeMapper()->clear();
}

void TimeStorage::wait()
{
    MapperFacade::timeMapper()->wait();
}

TimeStorage::TimeStorage() :
	m_all(0)
{
}
