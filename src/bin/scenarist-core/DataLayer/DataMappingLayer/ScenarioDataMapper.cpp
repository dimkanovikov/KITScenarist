#include "ScenarioDataMapper.h"

#include <Domain/ScenarioData.h>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, data_name, data_value ";
	const QString TABLE_NAME = " scenario_data ";
}

ScenarioData* ScenarioDataMapper::find(const Identifier& _id)
{
	return dynamic_cast<ScenarioData*>(abstractFind(_id));
}

ScenarioDataTable* ScenarioDataMapper::findAll()
{
	return qobject_cast<ScenarioDataTable*>(abstractFindAll());
}

void ScenarioDataMapper::insert(ScenarioData* _scenaryData)
{
	abstractInsert(_scenaryData);
}

void ScenarioDataMapper::update(ScenarioData* _scenaryData)
{
	abstractUpdate(_scenaryData);
}

QString ScenarioDataMapper::findStatement(const Identifier& _id) const
{
	QString findStatement =
			QString("SELECT " + COLUMNS +
					" FROM " + TABLE_NAME +
					" WHERE id = %1 "
					)
			.arg(_id.value());
	return findStatement;
}

QString ScenarioDataMapper::findAllStatement() const
{
	return "SELECT " + COLUMNS + " FROM  " + TABLE_NAME;
}

QString ScenarioDataMapper::insertStatement(DomainObject* _subject, QVariantList& _insertValues) const
{
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, data_name, data_value) "
					" VALUES(?, ?, ?) "
					);

	ScenarioData* scenaryData = dynamic_cast<ScenarioData*>(_subject );
	_insertValues.clear();
	_insertValues.append(scenaryData->id().value());
	_insertValues.append(scenaryData->name());
	_insertValues.append(scenaryData->value());

	return insertStatement;
}

QString ScenarioDataMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET data_value = ? "
					" WHERE data_name = ? "
					);

	ScenarioData* scenaryData = dynamic_cast<ScenarioData*>(_subject);
	_updateValues.clear();
	_updateValues.append(scenaryData->value());
	_updateValues.append(scenaryData->name());

	return updateStatement;
}

QString ScenarioDataMapper::deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const
{
	QString deleteStatement = "DELETE FROM " + TABLE_NAME + " WHERE id = ?";

	_deleteValues.clear();
	_deleteValues.append(_subject->id().value());

	return deleteStatement;
}

DomainObject* ScenarioDataMapper::doLoad(const Identifier& _id, const QSqlRecord& _record)
{
	const QString name = _record.value("data_name").toString();
	const QString value = _record.value("data_value").toString();

	return new ScenarioData(_id, name, value);
}

void ScenarioDataMapper::doLoad(DomainObject* _domainObject, const QSqlRecord& _record)
{
	if (ScenarioData* data = dynamic_cast<ScenarioData*>(_domainObject)) {
		const QString value = _record.value("data_value").toString();
		data->setValue(value);
	}
}

DomainObjectsItemModel* ScenarioDataMapper::modelInstance()
{
	return new ScenarioDataTable;
}

ScenarioDataMapper::ScenarioDataMapper()
{
}
