#include "Time.h"

using namespace Domain;


Time::Time(const Identifier& _id, const QString& _name) :
	DomainObject(_id),
	m_name(_name)
{
}

QString Time::name() const
{
	return m_name;
}

void Time::setName(const QString& _name)
{
	if (m_name != _name) {
		m_name = _name;

		changesNotStored();
	}
}

// ****

namespace {
	const int COLUMN_COUNT = 1;
}

TimesTable::TimesTable(QObject* _parent) :
	DomainObjectsItemModel(_parent)
{
}

int TimesTable::columnCount(const QModelIndex&) const
{
	return COLUMN_COUNT;
}

QVariant TimesTable::data(const QModelIndex& _index, int _role) const
{
	QVariant resultData;

	if (_role ==  Qt::DisplayRole
		|| _role == Qt::EditRole) {
		DomainObject *domainObject = domainObjects().value(_index.row());
		Time* time = dynamic_cast<Time*>(domainObject);
		Column column = sectionToColumn(_index.column());
		switch (column) {
			case Name: {
				resultData = time->name();
				break;
			}

			default: {
				break;
			}
		}
	}

	return resultData;
}

TimesTable::Column TimesTable::sectionToColumn(int _section) const
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
