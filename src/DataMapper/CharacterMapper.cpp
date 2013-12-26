#include "CharacterMapper.h"

#include <Domain/Character.h>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, name ";
	const QString TABLE_NAME = " characters ";
}

Character* CharacterMapper::find(const Identifier& _id)
{
	return dynamic_cast<Character*>(abstractFind(_id));
}

CharactersTable* CharacterMapper::findAll()
{
	return qobject_cast<CharactersTable*>(abstractFindAll());
}

void CharacterMapper::insert(Character* _character)
{
	abstractInsert(_character);
}

void CharacterMapper::update(Character* _character)
{
	abstractUpdate(_character);
}

QString CharacterMapper::findStatement(const Identifier& _id) const
{
	QString findStatement =
			QString("SELECT " + COLUMNS +
					" FROM " + TABLE_NAME +
					" WHERE id = %1 "
					)
			.arg(_id.value());
	return findStatement;
}

QString CharacterMapper::findAllStatement() const
{
	return "SELECT " + COLUMNS + " FROM  " + TABLE_NAME;
}

QString CharacterMapper::maxIdStatement() const
{
	return "SELECT MAX(id) FROM  " + TABLE_NAME;
}

QString CharacterMapper::insertStatement(DomainObject* _subject, QVariantList& _insertValues) const
{
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, name) "
					" VALUES(?, ?) "
					);

	Character* character = dynamic_cast<Character*>(_subject );
	_insertValues.clear();
	_insertValues.append(character->id().value());
	_insertValues.append(character->name());

	return insertStatement;
}

QString CharacterMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET name = ? "
					" WHERE id = ? "
					);

	Character* character = dynamic_cast<Character*>(_subject);
	_updateValues.clear();
	_updateValues.append(character->name());
	_updateValues.append(character->id().value());

	return updateStatement;
}

QString CharacterMapper::deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const
{
	QString deleteStatement = "DELETE FROM " + TABLE_NAME + " WHERE id = ?";

	_deleteValues.clear();
	_deleteValues.append(_subject->id().value());

	return deleteStatement;
}

DomainObject* CharacterMapper::doLoad(const Identifier& _id, const QSqlRecord& _record)
{
	QString name = _record.value("name").toString();

	return new Character(_id, name);
}

DomainObjectsItemModel* CharacterMapper::modelInstance()
{
	return new CharactersTable;
}

CharacterMapper::CharacterMapper()
{
}
