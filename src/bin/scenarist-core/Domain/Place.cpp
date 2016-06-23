#include "Place.h"

using namespace Domain;


Place::Place(const Identifier& _id, const QString& _name) :
	DomainObject(_id),
	m_name(_name)
{
	if (!m_name.endsWith(".")) {
		m_name.append(".");
	}
}

QString Place::name() const
{
	return m_name;
}

void Place::setName(const QString& _name)
{
	if (m_name != _name) {
		m_name = _name;

		if (!m_name.endsWith(".")) {
			m_name.append(".");
		}

		changesNotStored();
	}
}

bool Place::equal(const QString& _name) const
{
	bool equal = false;
	if (_name.endsWith(".")) {
		equal = m_name == _name;
	} else {
		equal = m_name == (_name + ".");
	}
	return equal;
}

// ****

namespace {
	const int COLUMN_COUNT = 1;
}

PlacesTable::PlacesTable(QObject* _parent) :
	DomainObjectsItemModel(_parent)
{
}

int PlacesTable::columnCount(const QModelIndex&) const
{
	return COLUMN_COUNT;
}

QVariant PlacesTable::data(const QModelIndex& _index, int _role) const
{
	QVariant resultData;

	if (_role ==  Qt::DisplayRole
		|| _role == Qt::EditRole) {
		DomainObject *domainObject = domainObjects().value(_index.row());
		Place* place = dynamic_cast<Place*>(domainObject);
		Column column = sectionToColumn(_index.column());
		switch (column) {
			case Name: {
				resultData = place->name();
				break;
			}

			default: {
				break;
			}
		}
	}

	return resultData;
}

PlacesTable::Column PlacesTable::sectionToColumn(int _section) const
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
