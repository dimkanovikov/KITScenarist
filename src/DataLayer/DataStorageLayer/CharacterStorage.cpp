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
		// Проверяем наличие данного персонажа
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

Character* CharacterStorage::updateCharacter(const QString& _oldName, const QString& _newName)
{
	Character* characterToUpdate = 0;

	//
	// Если такой персонаж есть
	//
	if (hasCharacter(_oldName)) {
		//
		// ... найдём его
		//
		foreach (DomainObject* domainObject, all()->toList()) {
			Character* character = dynamic_cast<Character*>(domainObject);
			if (character->name() == _oldName) {
				characterToUpdate = character;
				break;
			}
		}

		//
		// ... обновим
		//
		characterToUpdate->setName(_newName);

		//
		// ... и сохраним изменение в базе данных
		//
		MapperFacade::characterMapper()->update(characterToUpdate);

		//
		// ... уведомим об обновлении
		//
		int indexRow = all()->toList().indexOf(characterToUpdate);
		QModelIndex updateIndex = all()->index(indexRow, 0, QModelIndex());
		emit all()->dataChanged(updateIndex, updateIndex);
	}

	return characterToUpdate;
}

void CharacterStorage::removeCharacter(const QString& _name)
{
	//
	// Если такой персонаж есть
	//
	if (hasCharacter(_name)) {
		//
		// ... найдём его
		//
		Character* characterToDelete = 0;
		foreach (DomainObject* domainObject, all()->toList()) {
			Character* character = dynamic_cast<Character*>(domainObject);
			if (character->name() == _name) {
				characterToDelete = character;
				break;
			}
		}

		//
		// ... и удалим из локального списка и базы данных
		//
		all()->remove(characterToDelete);
		MapperFacade::characterMapper()->remove(characterToDelete);
	}
}

bool CharacterStorage::hasCharacter(const QString& _name)
{
	bool contains = false;
	foreach (DomainObject* domainObject, all()->toList()) {
		Character* character = dynamic_cast<Character*>(domainObject);
		if (character->name() == _name) {
			contains = true;
			break;
		}
	}
	return contains;
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
