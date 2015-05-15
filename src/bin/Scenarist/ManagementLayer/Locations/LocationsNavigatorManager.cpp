#include "LocationsNavigatorManager.h"

#include <Domain/Location.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/LocationStorage.h>

#include <UserInterfaceLayer/Locations/LocationsNavigator/LocationsNavigator.h>

#include <QInputDialog>
#include <QMessageBox>

using ManagementLayer::LocationsNavigatorManager;
using UserInterface::LocationsNavigator;


LocationsNavigatorManager::LocationsNavigatorManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_navigator(new LocationsNavigator(_parentWidget))
{
	initView();
	initConnections();
}

QWidget* LocationsNavigatorManager::view() const
{
	return m_navigator;
}

void LocationsNavigatorManager::loadLocations()
{
	//
	// Загрузить персонажей
	//
	m_navigator->setModel(DataStorageLayer::StorageFacade::locationStorage()->all());

	//
	// Выделить первого из списка
	//
	m_navigator->selectFirstLocation();
}

void LocationsNavigatorManager::chooseLocation(const QString& _name)
{
	m_navigator->selectLocation(_name);
}

void LocationsNavigatorManager::setCommentOnly(bool _isCommentOnly)
{
	m_navigator->setCommentOnly(_isCommentOnly);
}

void LocationsNavigatorManager::aboutAddLocation()
{
	QString locationName;
	bool needReply = false;
	do {
		needReply = false;
		locationName = QInputDialog::getText(m_navigator, tr("Add Location"), tr("Name"), QLineEdit::Normal, locationName);
		if (!locationName.isEmpty()) {
			if (DataStorageLayer::StorageFacade::locationStorage()->hasLocation(locationName.toUpper())) {
				QMessageBox::critical(
							m_navigator,
							tr("Add Location Error"),
							tr("Location with same name already exist in project")
							);
				needReply = true;
			} else {
				emit addLocation(locationName.toUpper());
			}
		}
	} while (needReply);
}

void LocationsNavigatorManager::initView()
{

}

void LocationsNavigatorManager::initConnections()
{
	connect(m_navigator, SIGNAL(addLocation()), this, SLOT(aboutAddLocation()));
	connect(m_navigator, SIGNAL(editLocation(QString)), this, SIGNAL(editLocation(QString)));
	connect(m_navigator, SIGNAL(removeLocation(QString)), this, SIGNAL(removeLocation(QString)));
	connect(m_navigator, SIGNAL(refreshLocations()), this, SIGNAL(refreshLocations()));
}
