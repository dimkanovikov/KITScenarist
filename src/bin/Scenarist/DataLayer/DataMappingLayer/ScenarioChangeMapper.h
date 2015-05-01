#ifndef SCENARIOCHANGEMAPPER_H
#define SCENARIOCHANGEMAPPER_H

#include "AbstractMapper.h"
#include "MapperFacade.h"

namespace Domain {
	class ScenarioChange;
	class ScenarioChangesTable;
}

using namespace Domain;


namespace DataMappingLayer
{
	class ScenarioChangeMapper : public AbstractMapper
	{
	public:
		ScenarioChange* find(const Identifier& _id);
		ScenarioChangesTable* findAll(const QString& _queryFilter = QString::null);
		void insert(ScenarioChange* _change);
		void update(ScenarioChange* _change);

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
		ScenarioChangeMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // SCENARIOCHANGEMAPPER_H
