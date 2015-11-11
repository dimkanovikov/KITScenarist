#include "CharactersNavigatorManager.h"

#include <Domain/Character.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>

#include <UserInterfaceLayer/Characters/CharactersNavigator/CharactersNavigator.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

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
	//
	// Загрузить персонажей
	//
	m_navigator->setModel(DataStorageLayer::StorageFacade::characterStorage()->all());

	//
	// Выделить первого из списка
	//
	m_navigator->selectFirstCharacter();
}

void CharactersNavigatorManager::chooseCharacter(const QString& _name)
{
	m_navigator->selectCharacter(_name);
}

void CharactersNavigatorManager::setCommentOnly(bool _isCommentOnly)
{
	m_navigator->setCommentOnly(_isCommentOnly);
}

void CharactersNavigatorManager::aboutAddCharacter()
{
	QString characterName;
	bool needReply = false;
	do {
		needReply = false;
		characterName = QLightBoxInputDialog::getText(m_navigator, tr("Add Character"), tr("Name"), characterName);
		if (!characterName.isEmpty()) {
			if (DataStorageLayer::StorageFacade::characterStorage()->hasCharacter(characterName.toUpper())) {
				QLightBoxMessage::critical(m_navigator, tr("Add Character Error"),
					tr("Character with same name already exist in project"));
				needReply = true;
			} else {
				emit addCharacter(characterName.toUpper());
			}
		}
	} while (needReply);
}

void CharactersNavigatorManager::initView()
{
	m_navigator->setObjectName("CharactersNavigator");
}

void CharactersNavigatorManager::initConnections()
{
	connect(m_navigator, SIGNAL(addCharacter()), this, SLOT(aboutAddCharacter()));
	connect(m_navigator, SIGNAL(editCharacter(QString)), this, SIGNAL(editCharacter(QString)));
	connect(m_navigator, SIGNAL(removeCharacters(QStringList)), this, SIGNAL(removeCharacters(QStringList)));
	connect(m_navigator, SIGNAL(refreshCharacters()), this, SIGNAL(refreshCharacters()));
}
