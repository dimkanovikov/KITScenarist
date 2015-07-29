#include "LocationsManager.h"

#include "LocationsNavigatorManager.h"
#include "LocationsDataEditManager.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/LocationStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <Domain/Location.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <QWidget>
#include <QSplitter>
#include <QHBoxLayout>

using ManagementLayer::LocationsManager;
using ManagementLayer::LocationsNavigatorManager;
using ManagementLayer::LocationsDataEditManager;


LocationsManager::LocationsManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new QWidget(_parentWidget)),
	m_navigatorManager(new LocationsNavigatorManager(this, m_view)),
	m_dataEditManager(new LocationsDataEditManager(this, m_view))
{
	initView();
	initConnections();
}

QWidget* LocationsManager::view() const
{
	return m_view;
}

void LocationsManager::loadCurrentProject()
{
	m_dataEditManager->clean();
	m_navigatorManager->loadLocations();
}

void LocationsManager::loadViewState()
{
	m_viewSplitter->restoreGeometry(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/locations/geometry",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
	m_viewSplitter->restoreState(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/locations/state",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
}

void LocationsManager::saveViewState()
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/locations/geometry", m_viewSplitter->saveGeometry().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/locations/state", m_viewSplitter->saveState().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
}

void LocationsManager::saveLocations()
{
	foreach (Domain::DomainObject* locationObject,
			 DataStorageLayer::StorageFacade::locationStorage()->all()->toList()) {
		Domain::Location* location = dynamic_cast<Domain::Location*>(locationObject);
		DataStorageLayer::StorageFacade::locationStorage()->updateLocation(location);
	}
}

void LocationsManager::setCommentOnly(bool _isCommentOnly)
{
	m_navigatorManager->setCommentOnly(_isCommentOnly);
	m_dataEditManager->setCommentOnly(_isCommentOnly);
}

void LocationsManager::aboutAddLocation(const QString& _name)
{
	DataStorageLayer::StorageFacade::locationStorage()->storeLocation(_name);
	m_navigatorManager->chooseLocation(_name);
}

void LocationsManager::aboutEditLocation(const QString& _name)
{
	//
	// Найдём локацию
	//
	Location* location = DataStorageLayer::StorageFacade::locationStorage()->location(_name);

	//
	// Загрузить в редактор данных данные
	//
	m_dataEditManager->editLocation(location);
}

void LocationsManager::aboutRemoveLocations(const QStringList& _names)
{
	//
	// Если пользователь серьёзно намерен удалить локации
	//
	if (QLightBoxMessage::question(m_view, QString::null,
			tr("Are you shure to remove locations: <b>%1</b>?").arg(_names.join(", ")),
			QDialogButtonBox::Yes | QDialogButtonBox::No)
		== QDialogButtonBox::Yes) {
		//
		// ... удалим
		//
		DataStorageLayer::StorageFacade::locationStorage()->removeLocations(_names);

		//
		// ... очистим редактор
		//
		m_dataEditManager->clean();
	}
}

void LocationsManager::initView()
{
	m_viewSplitter = new QSplitter(m_view);
	m_viewSplitter->setHandleWidth(1);
	m_viewSplitter->addWidget(m_navigatorManager->view());
	m_viewSplitter->addWidget(m_dataEditManager->view());
	m_viewSplitter->setStretchFactor(1, 1);
	m_viewSplitter->setOpaqueResize(false);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(m_viewSplitter);

	m_view->setLayout(layout);
}

void LocationsManager::initConnections()
{
	connect(m_navigatorManager, SIGNAL(addLocation(QString)), this, SLOT(aboutAddLocation(QString)));
	connect(m_navigatorManager, SIGNAL(editLocation(QString)), this, SLOT(aboutEditLocation(QString)));
	connect(m_navigatorManager, SIGNAL(removeLocations(QStringList)), this, SLOT(aboutRemoveLocations(QStringList)));
	connect(m_navigatorManager, SIGNAL(refreshLocations()), this, SIGNAL(refreshLocations()));

	connect(m_dataEditManager, SIGNAL(locationChanged()), this, SIGNAL(locationChanged()));
	connect(m_dataEditManager, SIGNAL(locationNameChanged(QString,QString)), this, SIGNAL(locationNameChanged(QString,QString)));
}
