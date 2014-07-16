#include "CharacterStateStorage.h"

#include <DataLayer/DataMappingLayer/MapperFacade.h>
#include <DataLayer/DataMappingLayer/CharacterStateMapper.h>

#include <Domain/CharacterState.h>

using namespace DataStorageLayer;
using namespace DataMappingLayer;


CharacterStatesTable* CharacterStateStorage::all()
{
	if (m_all == 0) {
		m_all = MapperFacade::characterStateMapper()->findAll();
	}
	return m_all;
}

CharacterState* CharacterStateStorage::storeCharacterState(const QString& _characterStateName)
{
	CharacterState* newCharacterState = 0;

	QString characterStateName = _characterStateName.toUpper().simplified();

	//
	// Если состояние можно сохранить
	//
	if (!characterStateName.isEmpty()) {
		//
		// Проверяем наличие данного состояния
		//
		foreach (DomainObject* domainObject, all()->toList()) {
			CharacterState* characterState = dynamic_cast<CharacterState*>(domainObject);
			if (characterState->name() == characterStateName) {
				newCharacterState = characterState;
				break;
			}
		}

		//
		// Если такого состояния ещё нет, то сохраним его
		//
		if (!DomainObject::isValid(newCharacterState)) {
			newCharacterState = new CharacterState(Identifier(), characterStateName);

			//
			// ... в базе данных
			//
			MapperFacade::characterStateMapper()->insert(newCharacterState);

			//
			// ... в списке
			//
			all()->append(newCharacterState);
		}
	}

	return newCharacterState;
}

bool CharacterStateStorage::hasCharacterState(const QString& _name)
{
	bool contains = false;
	foreach (DomainObject* domainObject, all()->toList()) {
		CharacterState* characterState = dynamic_cast<CharacterState*>(domainObject);
		if (characterState->name() == _name) {
			contains = true;
			break;
		}
	}
	return contains;
}

void CharacterStateStorage::clear()
{
	delete m_all;
	m_all = 0;

	MapperFacade::characterStateMapper()->clear();
}

void CharacterStateStorage::wait()
{
	MapperFacade::characterStateMapper()->wait();
}

CharacterStateStorage::CharacterStateStorage() :
	m_all(0)
{
}
