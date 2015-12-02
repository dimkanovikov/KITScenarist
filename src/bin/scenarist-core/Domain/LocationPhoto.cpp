#include "LocationPhoto.h"

#include "Location.h"

#include <3rd_party/Helpers/ImageHelper.h>

using namespace Domain;


LocationPhoto::LocationPhoto(
		const Identifier& _id,
		Location* _location,
		const QPixmap& _photo,
		int _sortOrder
		) :
	DomainObject(_id),
	m_location(_location),
	m_photo(_photo),
	m_sortOrder(_sortOrder)
{
}

Location* LocationPhoto::location() const
{
	return m_location;
}

void LocationPhoto::setLocation(Location* _location, bool _onLoad)
{
	if (m_location != _location) {
		m_location = _location;

		//
		// Помечается, как изменённый, только если это не загрузка объекта из БД
		//
		if (!_onLoad) {
			changesNotStored();
		}
	}
}

QPixmap LocationPhoto::photo() const
{
	return m_photo;
}

void LocationPhoto::setPhoto(const QPixmap& _photo)
{
	if (!ImageHelper::isImagesEqual(m_photo, _photo)) {
		m_photo = _photo;

		changesNotStored();
	}
}

int LocationPhoto::sortOrder() const
{
	return m_sortOrder;
}

void LocationPhoto::setSortOrder(int _sortOrder)
{
	if (m_sortOrder != _sortOrder) {
		m_sortOrder = _sortOrder;

		changesNotStored();
	}
}

// ****

namespace {
	const int COLUMN_COUNT = 2;
	const bool ON_LOAD = true;
}

LocationPhotosTable::LocationPhotosTable(QObject* _parent) :
	DomainObjectsItemModel(_parent)
{

}

void LocationPhotosTable::setLocation(Domain::Location* _location)
{
	foreach(DomainObject* domainObject, domainObjects()) {
		LocationPhoto* photo = dynamic_cast<LocationPhoto*>(domainObject);
		photo->setLocation(_location, ON_LOAD);
	}
}

int LocationPhotosTable::columnCount(const QModelIndex&) const
{
	return COLUMN_COUNT;
}

QVariant LocationPhotosTable::data(const QModelIndex& _index, int _role) const
{
	QVariant resultData;

	if (_role ==  Qt::DisplayRole
		|| _role == Qt::EditRole) {
		DomainObject* domainObject = domainObjects().value(_index.row());
		LocationPhoto* photo = dynamic_cast<LocationPhoto*>(domainObject);
		Column column = sectionToColumn(_index.column());
		switch (column) {
			case Photo: {
				resultData = photo->photo();
				break;
			}

			case SortOrder: {
				resultData = photo->sortOrder();
				break;
			}

			default: {
				break;
			}
		}
	}

	return resultData;
}

LocationPhotosTable::Column LocationPhotosTable::sectionToColumn(const int _section) const
{
	Column column = Undefined;

	switch (_section) {
		case 0: {
			column = Photo;
			break;
		}

		case 1: {
			column = SortOrder;
			break;
		}

		default: {
			break;
		}
	}

	return column;
}
