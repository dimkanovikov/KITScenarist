#include "CharactersManager.h"

#include "CharactersNavigatorManager.h"
#include "CharactersDataEditManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>

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

void CharactersManager::aboutAddCharacter(const QString& _name)
{
	DataStorageLayer::StorageFacade::characterStorage()->storeCharacter(_name);
	m_navigatorManager->chooseCharacter(_name);
}

void CharactersManager::aboutEditCharacter(const QString& _name)
{
	//
	// Загрузить в редактор данных данные
	//
	m_dataEditManager->editCharacter(_name);
}

void CharactersManager::aboutRemoveCharacter(const QString& _name)
{
	//
	// Если пользователь серьёзно намерен удалить персонажа
	//
	if (QMessageBox::question(
			m_view,
			tr("Remove Character"),
			tr("Are you shure to remove character?"),
			QMessageBox::Yes | QMessageBox::No) == 	QMessageBox::Yes) {
		//
		// ... удалим его
		//
		DataStorageLayer::StorageFacade::characterStorage()->removeCharacter(_name);

		//
		// ... очистим редактор
		//
		m_dataEditManager->clean();
	}
}

void CharactersManager::initView()
{
	QSplitter* splitter = new QSplitter(m_view);
	splitter->addWidget(m_navigatorManager->view());
	splitter->addWidget(m_dataEditManager->view());
	splitter->setStretchFactor(1, 1);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(splitter);

	m_view->setLayout(layout);
}

void CharactersManager::initConnections()
{
	connect(m_navigatorManager, SIGNAL(addCharacter(QString)), this, SLOT(aboutAddCharacter(QString)));
	connect(m_navigatorManager, SIGNAL(editCharacter(QString)), this, SLOT(aboutEditCharacter(QString)));
	connect(m_navigatorManager, SIGNAL(removeCharacter(QString)), this, SLOT(aboutRemoveCharacter(QString)));
	connect(m_navigatorManager, SIGNAL(refreshCharacters()), this, SIGNAL(refreshCharacters()));

	connect(m_dataEditManager, SIGNAL(characterNameChanged(QString,QString)), this, SIGNAL(characterNameChanged(QString,QString)));
}
