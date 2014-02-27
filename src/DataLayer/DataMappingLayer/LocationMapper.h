#ifndef LOCATIONMAPPER_H
#define LOCATIONMAPPER_H

#include "AbstractMapper.h"
#include "MapperFacade.h"

namespace Domain {
	class Location;
	class LocationsTable;
}

using namespace Domain;


namespace DataMappingLayer
{
	class LocationMapper : public AbstractMapper
	{
	public:
		Location* find(const Identifier& _id);
		LocationsTable* findAll();
		void insert(Location* _location);
		void update(Location* _location);

	protected:
		QString findStatement(const Identifier& _id) const;
		QString findAllStatement() const;
		QString insertStatement(DomainObject* _subject, QVariantList& _insertValues) const;
		QString updateStatement(DomainObject* _subject, QVariantList& _updateValues) const;
		QString deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const;

	protected:
		DomainObject* doLoad(const Identifier& _id, const QSqlRecord& _record);
		DomainObjectsItemModel* modelInstance();

	private:
		LocationMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // LOCATIONMAPPER_H
