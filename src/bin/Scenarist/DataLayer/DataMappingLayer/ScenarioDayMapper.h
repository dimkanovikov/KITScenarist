#ifndef SCENARIODAYMAPPER_H
#define SCENARIODAYMAPPER_H

#include "AbstractMapper.h"
#include "MapperFacade.h"

namespace Domain {
	class ScenarioDay;
	class ScenarioDaysTable;
}

using namespace Domain;


namespace DataMappingLayer
{
	class ScenarioDayMapper : public AbstractMapper
	{
	public:
		ScenarioDay* find(const Identifier& _id);
		ScenarioDaysTable* findAll();
		void insert(ScenarioDay* _scenaryDay);
		void update(ScenarioDay* _scenaryDay);

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
		ScenarioDayMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // SCENARIODAYMAPPER_H
