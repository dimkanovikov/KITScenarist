#include "CharactersManager.h"

#include "CharactersNavigatorManager.h"
#include "CharactersDataEditManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <Domain/Character.h>

#include <QWidget>
#include <QSplitter>
#include <QHBoxLayout>
#include <QMessageBox>

using ManagementLayer::CharactersManager;
using ManagementLayer::CharactersNavigatorManager;
using ManagementLayer::CharactersDataEditManager;


CharactersManager::CharactersManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new QWidget(_parentWidget)),
	m_navigatorManager(new CharactersNavigatorManager(this, m_view)),
	m_dataEditManager(new CharactersDataEditManager(this, m_view))
{
	initView();
	initConnections();
}

QWidget* CharactersManager::view() const
{
	return m_view;
}

void CharactersManager::loadCurrentProject()
{
	m_dataEditManager->clean();
	m_navigatorManager->loadCharacters();
}

void CharactersManager::loadViewState()
{
	m_viewSplitter->restoreGeometry(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/characters/geometry",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
	m_viewSplitter->restoreState(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/characters/state",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
}

void CharactersManager::saveViewState()
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/characters/geometry", m_viewSplitter->saveGeometry().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/characters/state", m_viewSplitter->saveState().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
}

void CharactersManager::saveCharacters()
{
	foreach (Domain::DomainObject* characterObject,
			 DataStorageLayer::StorageFacade::characterStorage()->all()->toList()) {
		Domain::Character* character = dynamic_cast<Domain::Character*>(characterObject);
		DataStorageLayer::StorageFacade::characterStorage()->updateCharacter(character);
	}
}

void CharactersManager::aboutAddCharacter(const QString& _name)
{
	DataStorageLayer::StorageFacade::characterStorage()->storeCharacter(_name);
	m_navigatorManager->chooseCharacter(_name);
}

void CharactersManager::aboutEditCharacter(const QString& _name)
{
	//
	// Найдём персонажа
	//
	Character* character = DataStorageLayer::StorageFacade::characterStorage()->character(_name);

	//
	// Загрузить в редактор данных данные
	//
	m_dataEditManager->editCharacter(character);
}

void CharactersManager::aboutRemoveCharacters(const QStringList& _names)
{
	//
	// Если пользователь серьёзно намерен удалить персонажа
	//
	if (QMessageBox::question(
			m_view,
			tr("Remove Characters"),
			tr("Are you shure to remove selected characters?"),
			QMessageBox::Yes | QMessageBox::No) == 	QMessageBox::Yes) {
		//
		// ... удалим его
		//
		DataStorageLayer::StorageFacade::characterStorage()->removeCharacters(_names);

		//
		// ... очистим редактор
		//
		m_dataEditManager->clean();
	}
}

void CharactersManager::initView()
{
	m_viewSplitter = new QSplitter(m_view);
	m_viewSplitter->setHandleWidth(1);
	m_viewSplitter->addWidget(m_navigatorManager->view());
	m_viewSplitter->addWidget(m_dataEditManager->view());
	m_viewSplitter->setStretchFactor(1, 1);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(m_viewSplitter);

	m_view->setLayout(layout);
}

void CharactersManager::initConnections()
{
	connect(m_navigatorManager, SIGNAL(addCharacter(QString)), this, SLOT(aboutAddCharacter(QString)));
	connect(m_navigatorManager, SIGNAL(editCharacter(QString)), this, SLOT(aboutEditCharacter(QString)));
	connect(m_navigatorManager, SIGNAL(removeCharacters(QStringList)), this, SLOT(aboutRemoveCharacters(QStringList)));
	connect(m_navigatorManager, SIGNAL(refreshCharacters()), this, SIGNAL(refreshCharacters()));

	connect(m_dataEditManager, SIGNAL(characterChanged()), this, SIGNAL(characterChanged()));
	connect(m_dataEditManager, SIGNAL(characterNameChanged(QString,QString)), this, SIGNAL(characterNameChanged(QString,QString)));
}
