#ifndef CHARACTERPHOTOMAPPER_H
#define CHARACTERPHOTOMAPPER_H

#include "AbstractMapper.h"
#include "MapperFacade.h"

namespace Domain {
	class CharacterPhoto;
	class CharacterPhotosTable;
}

using namespace Domain;


namespace DataMappingLayer
{
	class CharacterPhotoMapper : public AbstractMapper
	{
	public:
		CharacterPhoto* find(const Identifier& _id);
		CharacterPhotosTable* findAll();
		CharacterPhotosTable* findAllForCharacter(const Identifier& _characterIdentifier);
		void insert(CharacterPhoto* _character);
		void update(CharacterPhoto* _character);
		void remove(CharacterPhoto* _character);

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
		CharacterPhotoMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // CHARACTERPHOTOMAPPER_H
