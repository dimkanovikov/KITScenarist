#ifndef LOCATIONPHOTOMAPPER_H
#define LOCATIONPHOTOMAPPER_H

#include "AbstractMapper.h"
#include "MapperFacade.h"

namespace Domain {
	class LocationPhoto;
	class LocationPhotosTable;
}

using namespace Domain;


namespace DataMappingLayer
{
	class LocationPhotoMapper : public AbstractMapper
	{
	public:
		LocationPhoto* find(const Identifier& _id);
		LocationPhotosTable* findAll();
		LocationPhotosTable* findAllForLocation(const Identifier& _locationIdentifier);
		void insert(LocationPhoto* _location);
		void update(LocationPhoto* _location);
		void remove(LocationPhoto* _location);

	protected:
		QString findStatement(const Identifier& _id) const;
		QString findAllStatement() const;
		QString insertStatement(DomainObject* _subject, QVariantList& _insertValues) const;
		QString updateStatement(DomainObject* _subject, QVariantList& _updateValues) const;
		QString deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const;

	protected:
		DomainObject* doLoad(const Identifier& _id, const QSqlRecord& _record);
		void doLoad(DomainObject* _domainObject, const QSqlRecord& _record);
		DomainObjectsItemModel* modelInstance();

	private:
		LocationPhotoMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // LOCATIONPHOTOMAPPER_H
