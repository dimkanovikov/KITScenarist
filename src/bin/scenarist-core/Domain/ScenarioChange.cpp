#include "ScenarioChange.h"

using namespace Domain;


ScenarioChange::ScenarioChange(const Identifier& _id, const QUuid& _uuid,
	const QDateTime& _datetime, const QString& _user, const QString& _undoPatch,
	const QString& _redoPatch, bool _isDraft) :
	DomainObject(_id),
	m_uuid(_uuid),
	m_datetime(_datetime),
	m_user(_user),
	m_undoPatch(_undoPatch),
	m_redoPatch(_redoPatch),
	m_isDraft(_isDraft)
{
}

QUuid ScenarioChange::uuid() const
{
	return m_uuid;
}

void ScenarioChange::setUuid(const QUuid& _uuid)
{
	if (m_uuid != _uuid) {
		m_uuid = _uuid;

		changesNotStored();
	}
}

QDateTime ScenarioChange::datetime() const
{
	return m_datetime;
}

void ScenarioChange::setDatetime(const QDateTime& _datetime)
{
	if (m_datetime != _datetime) {
		m_datetime = _datetime;

		changesNotStored();
	}
}

QString ScenarioChange::user() const
{
	return m_user;
}

void ScenarioChange::setUser(const QString& _user)
{
	if (m_user != _user) {
		m_user = _user;

		changesNotStored();
	}
}

QString ScenarioChange::undoPatch() const
{
	return m_undoPatch;
}

void ScenarioChange::setUndoPatch(const QString& _patch)
{
	if (m_undoPatch != _patch) {
		m_undoPatch = _patch;

		changesNotStored();
	}
}

QString ScenarioChange::redoPatch() const
{
	return m_redoPatch;
}

void ScenarioChange::setRedoPatch(const QString& _patch)
{
	if (m_redoPatch != _patch) {
		m_redoPatch = _patch;

		changesNotStored();
	}
}

bool ScenarioChange::isDraft() const
{
	return m_isDraft;
}

void ScenarioChange::setIsDraft(bool _isDraft)
{
	if (m_isDraft != _isDraft) {
		m_isDraft = _isDraft;

		changesNotStored();
	}
}

// ****

namespace {
	const int COLUMN_COUNT = 1;
}

ScenarioChangesTable::ScenarioChangesTable(QObject* _parent) :
	DomainObjectsItemModel(_parent)
{
}

int ScenarioChangesTable::columnCount(const QModelIndex&) const
{
	return COLUMN_COUNT;
}

QVariant ScenarioChangesTable::data(const QModelIndex& _index, int _role) const
{
	QVariant resultData;

	if (_role ==  Qt::DisplayRole
		|| _role == Qt::EditRole) {
		DomainObject *domainObject = domainObjects().value(_index.row());
		ScenarioChange* scenarioChange = dynamic_cast<ScenarioChange*>(domainObject);
		Column column = sectionToColumn(_index.column());
		switch (column) {
			case Uuid: {
				resultData = scenarioChange->uuid().toString();
				break;
			}

			default: {
				break;
			}
		}
	}

	return resultData;
}

ScenarioChangesTable::Column ScenarioChangesTable::sectionToColumn(int _section) const
{
	Column column = Undefined;

	switch (_section) {
		case 0: {
			column = Uuid;
			break;
		}
		default: {
			break;
		}
	}

	return column;
}
