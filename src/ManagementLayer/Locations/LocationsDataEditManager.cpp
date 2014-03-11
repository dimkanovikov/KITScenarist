#include "LocationsDataEditManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/LocationStorage.h>

#include <UserInterfaceLayer/Locations/LocationsDataEdit/LocationsDataEdit.h>

using ManagementLayer::LocationsDataEditManager;
using UserInterface::LocationsDataEdit;


LocationsDataEditManager::LocationsDataEditManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_editor(new LocationsDataEdit(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* LocationsDataEditManager::view() const
{
	return m_editor;
}

void LocationsDataEditManager::clean()
{
	m_editor->clean();
	m_editor->setEnabled(false);
}

void LocationsDataEditManager::editLocation(const QString& _name)
{
	m_locationName = _name;

	m_editor->setEnabled(true);
	m_editor->setName(m_locationName);
}

void LocationsDataEditManager::aboutSave()
{
	QString newName = m_editor->name().toUpper();

	//
	// Если название было изменено
	//
	if (newName != m_locationName) {
		//
		// ... сохраним изменения
		//
		DataStorageLayer::StorageFacade::locationStorage()->updateLocation(m_locationName, newName);

		//
		// ... уведомим об изменении названия локации
		//
		emit locationNameChanged(m_locationName, newName);

		//
		// ... текущим становится новое название
		//
		editLocation(newName);
	}
}

void LocationsDataEditManager::aboutDontSave()
{
	m_editor->setName(m_locationName);
}

void LocationsDataEditManager::initView()
{
}

void LocationsDataEditManager::initConnections()
{
	connect(m_editor, SIGNAL(saveLocation()), this, SLOT(aboutSave()));
	connect(m_editor, SIGNAL(reloadLocation()), this, SLOT(aboutDontSave()));
}
