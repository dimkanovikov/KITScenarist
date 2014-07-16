#ifndef CHARACTERSTATEMAPPER_H
#define CHARACTERSTATEMAPPER_H

#include "AbstractMapper.h"
#include "MapperFacade.h"

namespace Domain {
	class CharacterState;
	class CharacterStatesTable;
}

using namespace Domain;


namespace DataMappingLayer
{
	class CharacterStateMapper : public AbstractMapper
	{
	public:
		CharacterState* find(const Identifier& _id);
		CharacterStatesTable* findAll();
		void insert(CharacterState* _characterState);
		void update(CharacterState* _characterState);

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
		CharacterStateMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // CHARACTERSTATEMAPPER_H
