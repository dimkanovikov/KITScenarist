#include "CharactersDataEditManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>

#include <UserInterfaceLayer/Characters/CharactersDataEdit/CharactersDataEdit.h>

using ManagementLayer::CharactersDataEditManager;
using UserInterface::CharactersDataEdit;


CharactersDataEditManager::CharactersDataEditManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_editor(new CharactersDataEdit(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* CharactersDataEditManager::view() const
{
	return m_editor;
}

void CharactersDataEditManager::clean()
{
	m_editor->clean();
	m_editor->setEnabled(false);
}

void CharactersDataEditManager::editCharacter(const QString& _name)
{
	m_characterName = _name;

	m_editor->setEnabled(true);
	m_editor->setName(m_characterName);
}

void CharactersDataEditManager::aboutSave()
{
	QString newName = m_editor->name().toUpper();

	//
	// Если имя было изменено
	//
	if (newName != m_characterName) {
		//
		// ... сохраним изменения
		//
		DataStorageLayer::StorageFacade::characterStorage()->updateCharacter(m_characterName, newName);

		//
		// ... уведомим об изменении имени персонажа
		//
		emit characterNameChanged(m_characterName, newName);

		//
		// ... текущим становится новое имя
		//
		editCharacter(newName);
	}
}

void CharactersDataEditManager::aboutDontSave()
{
	m_editor->setName(m_characterName);
}

void CharactersDataEditManager::initView()
{
}

void CharactersDataEditManager::initConnections()
{
	connect(m_editor, SIGNAL(saveCharacter()), this, SLOT(aboutSave()));
	connect(m_editor, SIGNAL(reloadCharacter()), this, SLOT(aboutDontSave()));
}
