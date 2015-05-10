#ifndef SCENARIODATAMAPPER_H
#define SCENARIODATAMAPPER_H

#include "AbstractMapper.h"
#include "MapperFacade.h"

namespace Domain {
	class ScenarioData;
	class ScenarioDataTable;
}

using namespace Domain;


namespace DataMappingLayer
{
	class ScenarioDataMapper : public AbstractMapper
	{
	public:
		ScenarioData* find(const Identifier& _id);
		ScenarioDataTable* findAll();
		void insert(ScenarioData* _scenaryData);
		void update(ScenarioData* _scenaryData);

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
		ScenarioDataMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // SCENARIODATAMAPPER_H
