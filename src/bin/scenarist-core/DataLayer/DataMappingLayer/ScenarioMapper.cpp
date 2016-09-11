#include "ScenarioMapper.h"

#include <Domain/Scenario.h>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, scheme, text, is_draft ";
	const QString TABLE_NAME = " scenario ";
}

Scenario* ScenarioMapper::find(const Identifier& _id)
{
	return dynamic_cast<Scenario*>(abstractFind(_id));
}

ScenariosTable* ScenarioMapper::findAll()
{
	return qobject_cast<ScenariosTable*>(abstractFindAll());
}

void ScenarioMapper::insert(Scenario* _scenario)
{
	abstractInsert(_scenario);
}

void ScenarioMapper::update(Scenario* _scenario)
{
	abstractUpdate(_scenario);
}

QString ScenarioMapper::findStatement(const Identifier& _id) const
{
	QString findStatement =
			QString("SELECT " + COLUMNS +
					" FROM " + TABLE_NAME +
					" WHERE id = %1 "
					)
			.arg(_id.value());
	return findStatement;
}

QString ScenarioMapper::findAllStatement() const
{
	return "SELECT " + COLUMNS + " FROM  " + TABLE_NAME;
}

QString ScenarioMapper::insertStatement(DomainObject* _subject, QVariantList& _insertValues) const
{
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (" + COLUMNS + ") "
					" VALUES(?, ?, ?, ?) "
					);

	Scenario* scenario = dynamic_cast<Scenario*>(_subject );
	_insertValues.clear();
	_insertValues.append(scenario->id().value());
	_insertValues.append(scenario->scheme());
	_insertValues.append(scenario->text());
	_insertValues.append(scenario->isDraft() ? "1" : "0");

	return insertStatement;
}

QString ScenarioMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET scheme = ?, "
					" text = ?, "
					" is_draft = ? "
					" WHERE id = ? "
					);

	Scenario* scenario = dynamic_cast<Scenario*>(_subject);
	_updateValues.clear();
	_updateValues.append(scenario->scheme());
	_updateValues.append(scenario->text());
	_updateValues.append(scenario->isDraft() ? "1" : "0");
	_updateValues.append(scenario->id().value());

	return updateStatement;
}

QString ScenarioMapper::deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const
{
	QString deleteStatement = "DELETE FROM " + TABLE_NAME + " WHERE id = ?";

	_deleteValues.clear();
	_deleteValues.append(_subject->id().value());

	return deleteStatement;
}

DomainObject* ScenarioMapper::doLoad(const Identifier& _id, const QSqlRecord& _record)
{
	const QString scheme = _record.value("scheme").toString();
	const QString text = _record.value("text").toString();
	const bool isDraft = _record.value("is_draft").toInt();

	return new Scenario(_id, scheme, text, isDraft);
}

void ScenarioMapper::doLoad(DomainObject* _domainObject, const QSqlRecord& _record)
{
	if (Scenario* scenario = dynamic_cast<Scenario*>(_domainObject)) {
		const QString scheme = _record.value("scheme").toString();
		scenario->setScheme(scheme);

		const QString text = _record.value("text").toString();
		scenario->setText(text);

		const bool isDraft = _record.value("is_draft").toInt();
		scenario->setIsDraft(isDraft);
	}
}

DomainObjectsItemModel* ScenarioMapper::modelInstance()
{
	return new ScenariosTable;
}

ScenarioMapper::ScenarioMapper()
{
}
