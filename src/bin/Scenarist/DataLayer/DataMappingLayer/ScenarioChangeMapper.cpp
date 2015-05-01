#include "ScenarioChangeMapper.h"

#include <Domain/ScenarioChange.h>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, uuid, datetime, username, undo_patch, redo_patch, is_draft ";
	const QString TABLE_NAME = " scenario_changes ";
}

ScenarioChange* ScenarioChangeMapper::find(const Identifier& _id)
{
	return dynamic_cast<ScenarioChange*>(abstractFind(_id));
}

ScenarioChangesTable* ScenarioChangeMapper::findAll(const QString& _queryFilter)
{
	return qobject_cast<ScenarioChangesTable*>(abstractFindAll(_queryFilter));
}

void ScenarioChangeMapper::insert(ScenarioChange* _change)
{
	abstractInsert(_change);
}

void ScenarioChangeMapper::update(ScenarioChange* _change)
{
	abstractUpdate(_change);
}

QString ScenarioChangeMapper::findStatement(const Identifier& _id) const
{
	QString findStatement =
			QString("SELECT " + COLUMNS +
					" FROM " + TABLE_NAME +
					" WHERE id = %1 "
					)
			.arg(_id.value());
	return findStatement;
}

QString ScenarioChangeMapper::findAllStatement() const
{
	return "SELECT " + COLUMNS + " FROM  " + TABLE_NAME;
}

QString ScenarioChangeMapper::insertStatement(DomainObject* _subject, QVariantList& _insertValues) const
{
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (" + COLUMNS + ") "
					" VALUES(?, ?, ?, ?, ?, ?, ?) "
					);

	ScenarioChange* change = dynamic_cast<ScenarioChange*>(_subject );
	_insertValues.clear();
	_insertValues.append(change->id().value());
	_insertValues.append(change->uuid().toString());
	_insertValues.append(change->datetime().toString("yyyy-MM-dd hh:mm:ss"));
	_insertValues.append(change->user());
	_insertValues.append(change->undoPatch());
	_insertValues.append(change->redoPatch());
	_insertValues.append(change->isDraft() ? "1" : "0");

	return insertStatement;
}

QString ScenarioChangeMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET uuid = ?, "
					" datetime = ?, "
					" username = ?, "
					" undo_patch = ?, "
					" redo_patch = ?, "
					" is_draft = ? "
					" WHERE id = ? "
					);

	ScenarioChange* change = dynamic_cast<ScenarioChange*>(_subject);
	_updateValues.clear();
	_updateValues.append(change->uuid().toString());
	_updateValues.append(change->datetime().toString("yyyy-MM-dd hh:mm:ss"));
	_updateValues.append(change->user());
	_updateValues.append(change->undoPatch());
	_updateValues.append(change->redoPatch());
	_updateValues.append(change->isDraft() ? "1" : "0");
	_updateValues.append(change->id().value());

	return updateStatement;
}

QString ScenarioChangeMapper::deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const
{
	QString deleteStatement = "DELETE FROM " + TABLE_NAME + " WHERE id = ?";

	_deleteValues.clear();
	_deleteValues.append(_subject->id().value());

	return deleteStatement;
}

DomainObject* ScenarioChangeMapper::doLoad(const Identifier& _id, const QSqlRecord& _record)
{
	const QUuid uuid = QUuid(_record.value("uuid").toString());
	const QDateTime datetime = QDateTime::fromString(_record.value("datetime").toString(), "yyyy-MM-dd hh:mm:ss");
	const QString user = _record.value("username").toString();
	const QString undoPatch = _record.value("undo_patch").toString();
	const QString redoPatch = _record.value("redo_patch").toString();
	const bool isDraft = _record.value("is_draft").toInt();

	return new ScenarioChange(_id, uuid, datetime, user, undoPatch, redoPatch, isDraft);
}

DomainObjectsItemModel* ScenarioChangeMapper::modelInstance()
{
	return new ScenarioChangesTable;
}

ScenarioChangeMapper::ScenarioChangeMapper()
{
}
