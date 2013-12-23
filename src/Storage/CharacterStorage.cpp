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
		foreach (DomainObject* domainObject, m_all->toList()) {
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
			m_all->append(newCharacter);
		}
	}

	return newCharacter;
}

CharacterStorage::CharacterStorage() :
	m_all(0)
{
}
