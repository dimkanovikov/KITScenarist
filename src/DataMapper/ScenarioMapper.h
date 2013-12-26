#ifndef SCENARIOMAPPER_H
#define SCENARIOMAPPER_H

#include "AbstractMapper.h"
#include "MapperFacade.h"

namespace Domain {
	class Scenario;
	class ScenariosTable;
}

using namespace Domain;


namespace DataMappingLayer
{
	class ScenarioMapper : public AbstractMapper
	{
	public:
		Scenario* find(const Identifier& _id);
		ScenariosTable* findAll();
		void insert(Scenario* _scenario);
		void update(Scenario* _scenario);

	protected:
		QString findStatement(const Identifier& _id) const;
		QString findAllStatement() const;
		QString maxIdStatement() const;
		QString insertStatement(DomainObject* _subject, QVariantList& _insertValues) const;
		QString updateStatement(DomainObject* _subject, QVariantList& _updateValues) const;
		QString deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const;

	protected:
		DomainObject* doLoad(const Identifier& _id, const QSqlRecord& _record);
		DomainObjectsItemModel* modelInstance();

	private:
		ScenarioMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // SCENARIOMAPPER_H
