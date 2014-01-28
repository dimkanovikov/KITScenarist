#include "CharacterStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/CharacterMapper.h>

#include <Domain/Character.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


CharactersTable* CharacterStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::characterMapper()->findAll();
	}
	return m_all;
}

Character* CharacterStorage::storeCharacter(const QString& _name)
{
	Character* newCharacter = 0;

	QString characterName = _name.toUpper().trimmed();

	//
	// Если персонажа можно сохранить
	//
	if (!characterName.isEmpty()) {
		//
		// Проверяем наличии данного персонажа
		//
		foreach (DomainObject* domainObject, all()->toList()) {
			Character* character = dynamic_cast<Character*>(domainObject);
			if (character->name() == characterName) {
				newCharacter = character;
				break;
			}
		}

		//
		// Если такого персонажа ещё нет, то сохраним его
		//
		if (!DomainObject::isValid(newCharacter)) {
			newCharacter = new Character(Identifier(), characterName);

			//
			// ... в базе данных
			//
			MapperFacade::characterMapper()->insert(newCharacter);

			//
			// ... в текущем списке персонажей
			//
			all()->append(newCharacter);
		}
	}

	return newCharacter;
}

void CharacterStorage::clear()
{
	delete m_all;
	m_all = 0;

	MapperFacade::characterMapper()->clear();
}

CharacterStorage::CharacterStorage() :
	m_all(0)
{
}
