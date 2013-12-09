#ifndef TIMEMAPPER_H
#define TIMEMAPPER_H

#include "AbstractMapper.h"
#include "MapperFacade.h"

namespace Domain {
	class Time;
	class TimesTable;
}

using namespace Domain;


namespace DataMappingLayer
{
	class TimeMapper : public AbstractMapper
	{
	public:
		Time* find(const Identifier& _id);
		TimesTable* findAll();
		void insert(Time* _time);
		void update(Time* _time);

	protected:
		QString findStatement(const Identifier& _id) const;
		QString findAllStatement() const;
		QString maxIdStatement() const;
		QString insertStatement(DomainObject* _subject) const;
		QString updateStatement(DomainObject* _subject) const;
		QString deleteStatement(DomainObject* _subject) const;

	protected:
		DomainObject* doLoad(const Identifier& _id, const QSqlRecord& _record);
		DomainObjectsItemModel* modelInstance();

	private:
		TimeMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // TIMEMAPPER_H
