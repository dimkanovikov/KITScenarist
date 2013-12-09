#ifndef PLACEMAPPER_H
#define PLACEMAPPER_H

#include "AbstractMapper.h"
#include "MapperFacade.h"

namespace Domain {
	class Place;
	class PlacesTable;
}

using namespace Domain;


namespace DataMappingLayer
{
	class PlaceMapper : public AbstractMapper
	{
	public:
		Place* find(const Identifier& _id);
		PlacesTable* findAll();
		void insert(Place* _place);
		void update(Place* _place);

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
		PlaceMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // PLACEMAPPER_H
