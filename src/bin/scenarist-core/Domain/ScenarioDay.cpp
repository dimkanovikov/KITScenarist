#include "ScenarioDay.h"

using namespace Domain;


ScenarioDay::ScenarioDay(const Identifier& _id, const QString& _name) :
	DomainObject(_id),
	m_name(_name)
{
}

QString ScenarioDay::name() const
{
	return m_name;
}

void ScenarioDay::setName(const QString& _name)
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

ScenarioDaysTable::ScenarioDaysTable(QObject* _parent) :
	DomainObjectsItemModel(_parent)
{
}

int ScenarioDaysTable::columnCount(const QModelIndex&) const
{
	return COLUMN_COUNT;
}

QVariant ScenarioDaysTable::data(const QModelIndex& _index, int _role) const
{
	QVariant resultData;

	if (_role ==  Qt::DisplayRole
		|| _role == Qt::EditRole) {
		DomainObject *domainObject = domainObjects().value(_index.row());
		ScenarioDay* scenarioDay = dynamic_cast<ScenarioDay*>(domainObject);
		Column column = sectionToColumn(_index.column());
		switch (column) {
			case Name: {
				resultData = scenarioDay->name();
				break;
			}

			default: {
				break;
			}
		}
	}

	return resultData;
}

ScenarioDaysTable::Column ScenarioDaysTable::sectionToColumn(int _section) const
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
