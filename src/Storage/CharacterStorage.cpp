#include "CharacterStorage.h"

#include <DataMapper/MapperFacade.h>
#include <DataMapper/CharacterMapper.h>

#include <Domain/Character.h>

using namespace StorageLayer;
using namespace DataMappingLayer;


CharactersTable* CharacterStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::characterMapper()->findAll();
	}
	return m_all;
}

void CharacterStorage::storeCharacter(const QString& _name)
{
	QString characterName = _name.toUpper();

	//
	// Проверяем наличии данного персонажа
	//
	bool characterExists = false;
	foreach (DomainObject* domainObject, m_all->toList()) {
		Character* character = dynamic_cast<Character*>(domainObject);
		if (character->name() == characterName) {
			characterExists = true;
			break;
		}
	}

	//
	// Если такого персонажа ещё нет, то сохраним его
	//
	if (!characterExists) {
		Character* character = new Character(Identifier(), characterName);

		//
		// ... в базе данных
		//
		MapperFacade::characterMapper()->insert(character);

		//
		// ... в текущем списке персонажей
		//
		m_all->append(character);
	}
}

CharacterStorage::CharacterStorage() :
	m_all(0)
{
}
