#include "CharactersNavigatorManager.h"

#include <Domain/Character.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>

#include <UserInterfaceLayer/Characters/CharactersNavigator/CharactersNavigator.h>

using ManagementLayer::CharactersNavigatorManager;
using UserInterface::CharactersNavigator;


CharactersNavigatorManager::CharactersNavigatorManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_navigator(new CharactersNavigator(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* CharactersNavigatorManager::view() const
{
	return m_navigator;
}

void CharactersNavigatorManager::loadCharacters()
{
	m_navigator->setModel(DataStorageLayer::StorageFacade::characterStorage()->all());
}

void CharactersNavigatorManager::initView()
{

}

void CharactersNavigatorManager::initConnections()
{

}
