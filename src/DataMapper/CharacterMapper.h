#ifndef CHARACTERMAPPER_H
#define CHARACTERMAPPER_H

#include "AbstractMapper.h"
#include "MapperFacade.h"

namespace Domain {
	class Character;
	class CharactersTable;
}

using namespace Domain;


namespace DataMappingLayer
{
	class CharacterMapper : public AbstractMapper
	{
	public:
		Character* find(const Identifier& _id);
		CharactersTable* findAll();
		void insert(Character* _character);
		void update(Character* _character);

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
		CharacterMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // CHARACTERMAPPER_H
