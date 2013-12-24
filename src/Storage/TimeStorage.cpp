#include "TimeStorage.h"

#include <DataMapper/MapperFacade.h>
#include <DataMapper/TimeMapper.h>

#include <Domain/Time.h>

using namespace StorageLayer;
using namespace DataMappingLayer;


TimesTable*TimeStorage::all()
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

TimeStorage::TimeStorage() :
	m_all(0)
{
}
