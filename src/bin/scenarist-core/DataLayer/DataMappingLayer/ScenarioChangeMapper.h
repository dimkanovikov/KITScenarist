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
		ScenarioChangesTable* findLastOne();
		ScenarioChangesTable* findAll(const QString& _queryFilter = QString::null);
		void insert(ScenarioChange* _change);
		void update(ScenarioChange* _change);

		/**
		 * @brief Существуюет ли изменение с заданным uuid
		 */
		bool containsUuid(const QString& _uuid);

		/**
		 * @brief Получить список uuid'ов всех локальных изменений
		 */
		QList<QString> uuids() const;

		/**
		 * @brief Получить изменение по uuid'у не загружая в кучу
		 */
		ScenarioChange change(const QString& _uuid) const;

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
		ScenarioChangeMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // SCENARIOCHANGEMAPPER_H
