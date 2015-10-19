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

Time* TimeStorage::storeTime(const QString& _timeName)
{
	Time* newTime = 0;

	QString timeName = _timeName.toUpper().simplified();

	//
	// Если время можно сохранить
	//
	if (!timeName.isEmpty()) {
		//
		// Проверяем наличие данного времени
		//
		foreach (DomainObject* domainObject, all()->toList()) {
			Time* time = dynamic_cast<Time*>(domainObject);
			if (time->name() == timeName) {
				newTime = time;
				break;
			}
		}

		//
		// Если такого времени ещё нет, то сохраним его
		//
		if (!DomainObject::isValid(newTime)) {
			newTime = new Time(Identifier(), timeName);

			//
			// ... в базе данных
			//
			MapperFacade::timeMapper()->insert(newTime);

			//
			// ... в списке
			//
			all()->append(newTime);
		}
	}

	return newTime;
}

void TimeStorage::clear()
{
	delete m_all;
	m_all = 0;

	MapperFacade::timeMapper()->clear();
}

void TimeStorage::refresh()
{
	MapperFacade::timeMapper()->refresh(all());
}

TimeStorage::TimeStorage() :
	m_all(0)
{
}
