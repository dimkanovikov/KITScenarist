#ifndef RESEARCHMAPPER_H
#define RESEARCHMAPPER_H

#include "AbstractMapper.h"
#include "MapperFacade.h"

namespace Domain {
	class Research;
	class ResearchTable;
}

using namespace Domain;


namespace DataMappingLayer
{
	class ResearchMapper : public AbstractMapper
	{
	public:
		Research* find(const Identifier& _id);
		ResearchTable* findAll();
		void insert(Research* _place);
		void update(Research* _place);
		void remove(Research* _place);

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
		ResearchMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // RESEARCHMAPPER_H
