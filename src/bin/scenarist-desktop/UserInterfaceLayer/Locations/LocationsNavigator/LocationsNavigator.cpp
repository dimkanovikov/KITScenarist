#include "LocationsNavigator.h"

#include "LocationsNavigatorItemDelegate.h"

#include <3rd_party/Widgets/FlatButton/FlatButton.h>

#include <QLabel>
#include <QToolButton>
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QShortcut>
#include <QSortFilterProxyModel>

using UserInterface::LocationsNavigator;
using UserInterface::LocationsNavigatorItemDelegate;


LocationsNavigator::LocationsNavigator(QWidget *parent) :
	QWidget(parent),
	m_title(new QLabel(this)),
	m_addLocation(new FlatButton(this)),
	m_removeLocation(new FlatButton(this)),
	m_refreshLocations(new FlatButton(this)),
	m_navigator(new QListView(this)),
	m_navigatorProxyModel(new QSortFilterProxyModel(m_navigator))
{
	initView();
	initConnections();
	initStyleSheet();
}

void LocationsNavigator::setModel(QAbstractItemModel* _model)
{
	m_navigatorProxyModel->setSourceModel(_model);
	m_navigator->setModel(m_navigatorProxyModel);

	//
	// Настраиваем запрос на изменение при изменении текущего элемента
	//
	connect(m_navigator->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
			this, SLOT(aboutEditLocation()));
}

void LocationsNavigator::selectFirstLocation()
{
	selectLocation(m_navigatorProxyModel->index(0, 0));
}

void LocationsNavigator::selectLocation(const QString& _name)
{
	const QModelIndex matchStartFrom = m_navigatorProxyModel->index(0, 0);
	QModelIndexList matches = m_navigatorProxyModel->match(matchStartFrom, Qt::DisplayRole, _name);
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

void LocationsNavigator::setCommentOnly(bool _isCommentOnly)
{
	m_addLocation->setVisible(!_isCommentOnly);
	m_removeLocation->setVisible(!_isCommentOnly);
	m_refreshLocations->setVisible(!_isCommentOnly);
}

void LocationsNavigator::aboutEditLocation()
{
	const QStringList locations = selectedLocationsNames();
	if (!locations.isEmpty()) {
		emit editLocation(locations.first());
	}
}

void LocationsNavigator::aboutRemoveLocation()
{
	emit removeLocations(selectedLocationsNames());
}

QStringList LocationsNavigator::selectedLocationsNames() const
{
	QStringList locationsNames;
	foreach (QModelIndex locationIndex, m_navigator->selectionModel()->selectedIndexes()) {
		locationsNames.append(m_navigatorProxyModel->data(locationIndex).toString());
	}
	return locationsNames;
}

void LocationsNavigator::initView()
{
	setFocusProxy(m_navigator);

	m_title->setText(tr("Locations"));
	m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_addLocation->setIcons(QIcon(":/Graphics/Icons/Editing/add.png"));
	m_addLocation->setShortcut(QKeySequence::New);
	m_addLocation->setToolTip(
			QString("%1 (%2)")
				.arg(tr("Create New Location"))
				.arg(m_addLocation->shortcut().toString(QKeySequence::NativeText)));

	m_removeLocation->setIcons(QIcon(":/Graphics/Icons/Editing/delete.png"));
	m_removeLocation->setToolTip(tr("Remove Selected Location") + " (Del)");
	m_removeLocation->setShortcut(QKeySequence("Delete"));

	m_refreshLocations->setIcons(QIcon(":/Graphics/Icons/Editing/refresh.png"));
	m_refreshLocations->setToolTip(tr("Find All Locations From Scenario"));

	m_navigatorProxyModel->sort(0);

	m_navigator->setAlternatingRowColors(true);
	m_navigator->setItemDelegate(new LocationsNavigatorItemDelegate(m_navigator));
	m_navigator->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_navigator->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(m_title);
	topLayout->addWidget(m_addLocation);
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
	connect(m_removeLocation, SIGNAL(clicked()), this, SLOT(aboutRemoveLocation()));
	QShortcut* removeLocationShortcut = new QShortcut(QKeySequence("Backspace"), m_navigator);
	connect(removeLocationShortcut, &QShortcut::activated, m_removeLocation, &FlatButton::click);
	connect(m_refreshLocations, SIGNAL(clicked()), this, SIGNAL(refreshLocations()));
}

void LocationsNavigator::initStyleSheet()
{
	m_title->setProperty("inTopPanel", true);
	m_title->setProperty("topPanelTopBordered", true);

	m_addLocation->setProperty("inTopPanel", true);
	m_removeLocation->setProperty("inTopPanel", true);
	m_refreshLocations->setProperty("inTopPanel", true);

	m_navigator->setProperty("mainContainer", true);
}
