#include "ScenarioDayMapper.h"

#include <Domain/ScenarioDay.h>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, name ";
	const QString TABLE_NAME = " scenary_days ";
}

ScenarioDay* ScenarioDayMapper::find(const Identifier& _id)
{
	return dynamic_cast<ScenarioDay*>(abstractFind(_id));
}

ScenarioDaysTable* ScenarioDayMapper::findAll()
{
	return qobject_cast<ScenarioDaysTable*>(abstractFindAll());
}

void ScenarioDayMapper::insert(ScenarioDay* _scenaryDay)
{
	abstractInsert(_scenaryDay);
}

void ScenarioDayMapper::update(ScenarioDay* _scenaryDay)
{
	abstractUpdate(_scenaryDay);
}

QString ScenarioDayMapper::findStatement(const Identifier& _id) const
{
	QString findStatement =
			QString("SELECT " + COLUMNS +
					" FROM " + TABLE_NAME +
					" WHERE id = %1 "
					)
			.arg(_id.value());
	return findStatement;
}

QString ScenarioDayMapper::findAllStatement() const
{
	return "SELECT " + COLUMNS + " FROM  " + TABLE_NAME;
}

QString ScenarioDayMapper::maxIdStatement() const
{
	return "SELECT MAX(id) FROM  " + TABLE_NAME;
}

QString ScenarioDayMapper::insertStatement(DomainObject* _subject, QVariantList& _insertValues) const
{
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, name) "
					" VALUES(?, ?) "
					);

	ScenarioDay* scenaryDay = dynamic_cast<ScenarioDay*>(_subject );
	_insertValues.clear();
	_insertValues.append(scenaryDay->id().value());
	_insertValues.append(scenaryDay->name());

	return insertStatement;
}

QString ScenarioDayMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET name = ? "
					" WHERE id = ? "
					);

	ScenarioDay* scenaryDay = dynamic_cast<ScenarioDay*>(_subject);
	_updateValues.clear();
	_updateValues.append(scenaryDay->name());
	_updateValues.append(scenaryDay->id().value());

	return updateStatement;
}

QString ScenarioDayMapper::deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const
{
	QString deleteStatement = "DELETE FROM " + TABLE_NAME + " WHERE id = ?";

	_deleteValues.clear();
	_deleteValues.append(_subject->id().value());

	return deleteStatement;
}

DomainObject* ScenarioDayMapper::doLoad(const Identifier& _id, const QSqlRecord& _record)
{
	QString name = _record.value("name").toString();

	return new ScenarioDay(_id, name);
}

DomainObjectsItemModel* ScenarioDayMapper::modelInstance()
{
	return new ScenarioDaysTable;
}

ScenarioDayMapper::ScenarioDayMapper()
{
}
