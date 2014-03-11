#include "Location.h"

using namespace Domain;


Location::Location(const Identifier& _id, const QString& _name) :
	DomainObject(_id),
	m_name(_name)
{
}

QString Location::name() const
{
	return m_name;
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
