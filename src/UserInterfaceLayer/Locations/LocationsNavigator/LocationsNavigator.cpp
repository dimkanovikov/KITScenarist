#include "LocationsNavigator.h"

#include <QLabel>
#include <QToolButton>
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>

using UserInterface::LocationsNavigator;


LocationsNavigator::LocationsNavigator(QWidget *parent) :
	QWidget(parent),
	m_addLocation(new QToolButton(this)),
	m_editLocation(new QToolButton(this)),
	m_removeLocation(new QToolButton(this)),
	m_refreshLocations(new QToolButton(this)),
	m_navigator(new QListView(this))
{
	initView();
	initConnections();
}

void LocationsNavigator::setModel(QAbstractItemModel* _model)
{
	m_navigator->setModel(_model);
}

void LocationsNavigator::selectFirstLocation()
{
	selectLocation(m_navigator->model()->index(0, 0));
}

void LocationsNavigator::selectLocation(const QString& _name)
{
	const QModelIndex matchStartFrom = m_navigator->model()->index(0, 0);
	QModelIndexList matches = m_navigator->model()->match(matchStartFrom, Qt::DisplayRole, _name);
	if (matches.size() > 0) {
		selectLocation(matches.first());
	}
}

void LocationsNavigator::selectLocation(const QModelIndex& _index)
{
	if (m_navigator->model() != 0
		&& m_navigator->model()->rowCount() > 0) {
		//
		// Выделим первую локацию
		//
		m_navigator->setCurrentIndex(_index);

		//
		// Готовим к изменению
		//
		aboutEditLocation();
	}
}

void LocationsNavigator::aboutEditLocation()
{
	emit editLocation(selectedUserName());
}

void LocationsNavigator::aboutRemoveLocation()
{
	emit removeLocation(selectedUserName());
}

QString LocationsNavigator::selectedUserName() const
{
	QModelIndex currentSelected = m_navigator->selectionModel()->currentIndex();
	QString userName = m_navigator->model()->data(currentSelected).toString();
	return userName;
}

void LocationsNavigator::initView()
{
	QLabel* title = new QLabel(tr("Locations"), this);

	m_addLocation->setIcon(QIcon(":/Graphics/Icons/Editing/add.png"));
	m_editLocation->setIcon(QIcon(":/Graphics/Icons/Editing/edit.png"));
	m_removeLocation->setIcon(QIcon(":/Graphics/Icons/Editing/delete.png"));
	m_refreshLocations->setIcon(QIcon(":/Graphics/Icons/Editing/refresh.png"));

	m_navigator->setAlternatingRowColors(true);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(title);
	topLayout->addStretch();
	topLayout->addWidget(m_addLocation);
	topLayout->addWidget(m_editLocation);
	topLayout->addWidget(m_removeLocation);
	topLayout->addWidget(m_refreshLocations);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addLayout(topLayout);
	layout->addWidget(m_navigator);

	setLayout(layout);
}

void LocationsNavigator::initConnections()
{
	connect(m_addLocation, SIGNAL(clicked()), this, SIGNAL(addLocation()));
	connect(m_editLocation, SIGNAL(clicked()), this, SLOT(aboutEditLocation()));
	connect(m_navigator, SIGNAL(activated(QModelIndex)), this, SLOT(aboutEditLocation()));
	connect(m_removeLocation, SIGNAL(clicked()), this, SLOT(aboutRemoveLocation()));
	connect(m_refreshLocations, SIGNAL(clicked()), this, SIGNAL(refreshLocations()));
}
