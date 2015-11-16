#include "ScenarioData.h"

#include <QString>

using namespace Domain;


const QString ScenarioData::NAME_KEY = "name";
const QString ScenarioData::LOGLINE_KEY= "logline";
const QString ScenarioData::ADDITIONAL_INFO_KEY= "additional_info";
const QString ScenarioData::GENRE_KEY= "genre";
const QString ScenarioData::AUTHOR_KEY= "author";
const QString ScenarioData::CONTACTS_KEY= "contacts";
const QString ScenarioData::YEAR_KEY= "year";
const QString ScenarioData::SYNOPSIS_KEY= "synopsis";

ScenarioData::ScenarioData(const Domain::Identifier& _id, const QString& _name,
	const QString& _value) :
	DomainObject(_id),
	m_name(_name),
	m_value(_value)
{
	Q_ASSERT(!_name.isEmpty());
}

QString ScenarioData::name() const
{
	return m_name;
}

QString ScenarioData::value() const
{
	return m_value;
}

void ScenarioData::setValue(const QString& _value)
{
	if (m_value != _value) {
		m_value = _value;

		changesNotStored();
	}
}

// ****

namespace {
	const int COLUMN_COUNT = 2;
}

ScenarioDataTable::ScenarioDataTable(QObject* _parent) :
	DomainObjectsItemModel(_parent)
{
}

int ScenarioDataTable::columnCount(const QModelIndex&) const
{
	return COLUMN_COUNT;
}

QVariant ScenarioDataTable::data(const QModelIndex& _index, int _role) const
{
	QVariant resultData;

	if (_role ==  Qt::DisplayRole
		|| _role == Qt::EditRole) {
		DomainObject *domainObject = domainObjects().value(_index.row());
		ScenarioData* scenarioData = dynamic_cast<ScenarioData*>(domainObject);
		Column column = sectionToColumn(_index.column());
		switch (column) {
			case Name: {
				resultData = scenarioData->name();
				break;
			}
			case Value: {
				resultData = scenarioData->value();
				break;
			}
			default: {
				break;
			}
		}
	}

	return resultData;
}

ScenarioData* ScenarioDataTable::scenarioData(const QString& _name) const
{
	ScenarioData* result = 0;
	foreach (DomainObject* domainObject, domainObjects()) {
		ScenarioData* scenarioData = dynamic_cast<ScenarioData*>(domainObject);
		if (scenarioData->name() == _name) {
			result = scenarioData;
		}
	}
	return result;
}

ScenarioDataTable::Column ScenarioDataTable::sectionToColumn(int _section) const
{
	Column column = Undefined;

	switch (_section) {
		case 0: {
			column = Name;
			break;
		}
		case 1: {
			column = Value;
			break;
		}
		default: {
			break;
		}
	}

	return column;
}
