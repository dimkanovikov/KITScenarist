#include "CharactersDataEditManager.h"

#include <Domain/Character.h>

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

void CharactersDataEditManager::editCharacter(Domain::Character* _character)
{
	clean();

	m_character = _character;

	if (m_character != 0) {
		m_editor->setEnabled(true);
		m_editor->setName(m_character->name());
		m_editor->setRealName(m_character->realName());
		m_editor->setDescription(m_character->description());
		m_editor->setPhotos(m_character->photos());
	} else {
		clean();
	}
}

void CharactersDataEditManager::aboutSave()
{
	//
	// Сохраним предыдущее название персонажа
	//
	QString previousName = m_character->name();

	//
	// Установим новые значения
	//
	m_character->setName(m_editor->name());
	m_character->setRealName(m_editor->realName());
	m_character->setDescription(m_editor->description());
	m_character->setPhotos(m_editor->photos());

	//
	// Сохраним изменения
	//
	DataStorageLayer::StorageFacade::characterStorage()->updateCharacter(m_character);

	//
	// Уведомим об изменении названия персонажа
	//
	emit characterNameChanged(previousName, m_character->name());

	//
	// Текущим становится обновлённый персонаж
	//
	editCharacter(m_character);
}

void CharactersDataEditManager::aboutDontSave()
{
	//
	// Перезагрузим исходные данных
	//
	editCharacter(m_character);
}

void CharactersDataEditManager::initView()
{
}

void CharactersDataEditManager::initConnections()
{
	connect(m_editor, SIGNAL(saveCharacter()), this, SLOT(aboutSave()));
	connect(m_editor, SIGNAL(reloadCharacter()), this, SLOT(aboutDontSave()));
}
