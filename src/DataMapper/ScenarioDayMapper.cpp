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

QString ScenarioDayMapper::insertStatement(DomainObject* _subject) const
{
	ScenarioDay* scenaryDay = dynamic_cast<ScenarioDay*>(_subject );
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, name) "
					" VALUES(%1, '%2') "
					)
			.arg(scenaryDay->id().value())
			.arg(scenaryDay->name());
	return insertStatement;
}

QString ScenarioDayMapper::updateStatement(DomainObject* _subject) const
{
	ScenarioDay* scenaryDay = dynamic_cast<ScenarioDay*>(_subject);
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET name = '%1' "
					" WHERE id = %2 "
					)
			.arg(scenaryDay->name())
			.arg(scenaryDay->id().value());
	return updateStatement;
}

QString ScenarioDayMapper::deleteStatement(DomainObject* _subject) const
{
	return "DELETE FROM " + TABLE_NAME + " WHERE id = " + QString::number(_subject->id().value());
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
