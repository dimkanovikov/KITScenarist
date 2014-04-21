#include "CharacterPhotoStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/CharacterPhotoMapper.h>

#include <Domain/Character.h>
#include <Domain/CharacterPhoto.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


CharacterPhotosTable* CharacterPhotoStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::characterPhotoMapper()->findAll();
	}
	return m_all;
}

void CharacterPhotoStorage::store(Character* _character)
{
	//
	// Удалить все старые фотографии
	//
	remove(_character);

	//
	// Сохранить новые фотографии
	//
	CharacterPhotosTable* newPhotos = _character->photosTable();
	foreach (DomainObject* domainObject, newPhotos->toList()) {
		CharacterPhoto* newPhoto = dynamic_cast<CharacterPhoto*>(domainObject);

		MapperFacade::characterPhotoMapper()->insert(newPhoto);
		all()->append(newPhoto);
	}
}

void CharacterPhotoStorage::remove(Character* _character)
{
	CharacterPhotosTable* photos =
			MapperFacade::characterPhotoMapper()->findAllForCharacter(_character->id());
	foreach (DomainObject* domainObject, photos->toList()) {
		CharacterPhoto* oldPhoto = dynamic_cast<CharacterPhoto*>(domainObject);

		all()->remove(oldPhoto);
		MapperFacade::characterPhotoMapper()->remove(oldPhoto);
	}
	delete photos;
	photos = 0;
}

void CharacterPhotoStorage::clear()
{
	delete m_all;
	m_all = 0;

	MapperFacade::characterPhotoMapper()->clear();
}

CharacterPhotoStorage::CharacterPhotoStorage() :
	m_all(0)
{
}
