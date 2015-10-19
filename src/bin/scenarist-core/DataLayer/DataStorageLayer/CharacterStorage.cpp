#include "CharacterStorage.h"

#include "CharacterPhotoStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/CharacterMapper.h>

#include <Domain/Character.h>
#include <Domain/CharacterPhoto.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


CharactersTable* CharacterStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::characterMapper()->findAll();
	}
	return m_all;
}

Character* CharacterStorage::character(const QString& _name)
{
	Character* resultCharacter = 0;
	foreach (DomainObject* domainObject, all()->toList()) {
		Character* character = dynamic_cast<Character*>(domainObject);
		if (character->name() == _name) {
			resultCharacter = character;
			break;
		}
	}
	return resultCharacter;
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
			newCharacter = new Character(Identifier(), characterName, QString(), QString(), new CharacterPhotosTable);

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

void CharacterStorage::updateCharacter(Character* _character)
{
	//
	// Сохраним изменение в базе данных
	//
	MapperFacade::characterMapper()->update(_character);
	StorageFacade::characterPhotoStorage()->store(_character);

	//
	// Уведомим об обновлении
	//
	int indexRow = all()->toList().indexOf(_character);
	QModelIndex updateIndex = all()->index(indexRow, 0, QModelIndex());
	emit all()->dataChanged(updateIndex, updateIndex);
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
		// ... и удалим
		//
		// ...... фотографии
		StorageFacade::characterPhotoStorage()->remove(characterToDelete);
		// ...... из локального списка и базы данных
		all()->remove(characterToDelete);
		MapperFacade::characterMapper()->remove(characterToDelete);
	}
}

void CharacterStorage::removeCharacters(const QStringList& _names)
{
	foreach (const QString& name, _names) {
		removeCharacter(name);
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

void CharacterStorage::refresh()
{
	MapperFacade::characterMapper()->refresh(all());
}

CharacterStorage::CharacterStorage() :
	m_all(0)
{
}
