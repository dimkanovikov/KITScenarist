#include "Location.h"

#include "LocationPhoto.h"

using namespace Domain;


Location::Location(
		const Identifier& _id,
		const QString& _name,
		const QString& _description,
		LocationPhotosTable* _photos
		) :
	DomainObject(_id),
	m_name(_name),
	m_description(_description),
	m_photos(_photos)
{
	_photos->setLocation(this);
}

QString Location::name() const
{
	return m_name;
}

void Location::setName(const QString& _name)
{
	if (m_name != _name) {
		m_name = _name;

		changesNotStored();
	}
}

QString Location::description() const
{
	return m_description;
}

void Location::setDescription(const QString& _description)
{
	if (m_description != _description) {
		m_description = _description;

		changesNotStored();
	}
}

LocationPhotosTable* Location::photosTable() const
{
	return m_photos;
}

QList<QPixmap> Location::photos() const
{
	QList<QPixmap> photos;
	foreach (DomainObject* domainObject, m_photos->toList()) {
		LocationPhoto* photo = dynamic_cast<LocationPhoto*>(domainObject);
		photos.insert(photo->sortOrder(), photo->photo());
	}
	return photos;
}

void Location::setPhotosTable(LocationPhotosTable* _photos)
{
	if (m_photos != _photos) {
		m_photos->clear();

		foreach (DomainObject* photo, _photos->toList()) {
			m_photos->append(photo);
		}

		changesNotStored();
	}
}

void Location::setPhotos(const QList<QPixmap>& _photos)
{
	m_photos->clear();

	for (int index = 0; index < _photos.count(); ++index) {
		LocationPhoto* newPhoto = new LocationPhoto(Identifier(), this, _photos.value(index), index);
		m_photos->append(newPhoto);
	}

	changesNotStored();
}

// ****


namespace {
	const int COLUMN_COUNT = 1;
}

LocationsTable::LocationsTable(QObject* _parent) :
	DomainObjectsItemModel(_parent)
{
}

int LocationsTable::columnCount(const QModelIndex&) const
{
	return COLUMN_COUNT;
}

QVariant LocationsTable::data(const QModelIndex& _index, int _role) const
{
	QVariant resultData;

	if (_role ==  Qt::DisplayRole
		|| _role == Qt::EditRole) {
		DomainObject *domainObject = domainObjects().value(_index.row());
		Location* location = dynamic_cast<Location*>(domainObject);
		Column column = sectionToColumn(_index.column());
		switch (column) {
			case Name: {
				resultData = location->name();
				break;
			}

			default: {
				break;
			}
		}
	}

	return resultData;
}

LocationsTable::Column LocationsTable::sectionToColumn(int _section) const
{
	Column column = Undefined;

	switch (_section) {
		case 0: {
			column = Name;
			break;
		}

		default: {
			break;
		}
	}

	return column;
}
