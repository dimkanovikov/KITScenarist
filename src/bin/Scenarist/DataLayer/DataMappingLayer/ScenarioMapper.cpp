#include "ScenarioMapper.h"

#include <Domain/Scenario.h>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, name, additional_info, genre, author, contacts, year, synopsis, text, is_draft ";
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
					" (id, name, additional_info, genre, author, contacts, year, synopsis, text, is_draft) "
					" VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
					);

	Scenario* scenario = dynamic_cast<Scenario*>(_subject );
	_insertValues.clear();
	_insertValues.append(scenario->id().value());
	_insertValues.append(scenario->name());
	_insertValues.append(scenario->additionalInfo());
	_insertValues.append(scenario->genre());
	_insertValues.append(scenario->author());
	_insertValues.append(scenario->contacts());
	_insertValues.append(scenario->year());
	_insertValues.append(scenario->synopsis());
	_insertValues.append(scenario->text());
	_insertValues.append(scenario->isDraft() ? "1" : "0");

	return insertStatement;
}

QString ScenarioMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET name = ?, "
					" additional_info = ?, "
					" genre = ?, "
					" author = ?, "
					" contacts = ?, "
					" year = ?, "
					" synopsis = ?, "
					" text = ?, "
					" is_draft = ? "
					" WHERE id = ? "
					);

	Scenario* scenario = dynamic_cast<Scenario*>(_subject);
	_updateValues.clear();
	_updateValues.append(scenario->name());
	_updateValues.append(scenario->additionalInfo());
	_updateValues.append(scenario->genre());
	_updateValues.append(scenario->author());
	_updateValues.append(scenario->contacts());
	_updateValues.append(scenario->year());
	_updateValues.append(scenario->synopsis());
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
	QString name = _record.value("name").toString();
	QString additionalInfo = _record.value("additional_info").toString();
	QString genre = _record.value("genre").toString();
	QString author = _record.value("author").toString();
	QString contacts = _record.value("contacts").toString();
	QString year = _record.value("year").toString();
	QString synopsis = _record.value("synopsis").toString();
	QString text = _record.value("text").toString();
	bool isDraft = _record.value("is_draft").toInt();

	Scenario* scenario = new Scenario(_id, name, synopsis, text);
	scenario->setAdditionalInfo(additionalInfo);
	scenario->setGenre(genre);
	scenario->setAuthor(author);
	scenario->setContacts(contacts);
	scenario->setYear(year);
	scenario->setIsDraft(isDraft);

	return scenario;
}

DomainObjectsItemModel* ScenarioMapper::modelInstance()
{
	return new ScenariosTable;
}

ScenarioMapper::ScenarioMapper()
{
}
