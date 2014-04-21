#include "LocationsDataEditManager.h"

#include <Domain/Location.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/LocationStorage.h>

#include <UserInterfaceLayer/Locations/LocationsDataEdit/LocationsDataEdit.h>

using Domain::Location;
using ManagementLayer::LocationsDataEditManager;
using UserInterface::LocationsDataEdit;


LocationsDataEditManager::LocationsDataEditManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_editor(new LocationsDataEdit(_parentWidget)),
	m_location(0)
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

void LocationsDataEditManager::editLocation(Location* _location)
{
	clean();

	m_location = _location;

	if (m_location != 0) {
		m_editor->setEnabled(true);
		m_editor->setName(m_location->name());
		m_editor->setDescription(m_location->description());
		m_editor->setPhotos(m_location->photos());
	} else {
		clean();
	}
}

void LocationsDataEditManager::aboutSave()
{
	//
	// Сохраним предыдущее название локации
	//
	QString previousName = m_location->name();

	//
	// Установим новые значения
	//
	m_location->setName(m_editor->name());
	m_location->setDescription(m_editor->description());
	m_location->setPhotos(m_editor->photos());

	//
	// Сохраним изменения
	//
	DataStorageLayer::StorageFacade::locationStorage()->updateLocation(m_location);

	//
	// Уведомим об изменении названия локации
	//
	emit locationNameChanged(previousName, m_location->name());

	//
	// Текущей становится обновлённая локация
	//
	editLocation(m_location);
}

void LocationsDataEditManager::aboutDontSave()
{
	//
	// Перезагрузим исходные данных
	//
	editLocation(m_location);
}

void LocationsDataEditManager::initView()
{
}

void LocationsDataEditManager::initConnections()
{
	connect(m_editor, SIGNAL(saveLocation()), this, SLOT(aboutSave()));
	connect(m_editor, SIGNAL(reloadLocation()), this, SLOT(aboutDontSave()));
}
