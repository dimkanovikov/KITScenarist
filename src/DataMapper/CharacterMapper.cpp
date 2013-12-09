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

QString CharacterMapper::insertStatement(DomainObject* _subject) const
{
	Character* character = dynamic_cast<Character*>(_subject );
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, name) "
					" VALUES(%1, '%2') "
					)
			.arg(character->id().value())
			.arg(character->name());
	return insertStatement;
}

QString CharacterMapper::updateStatement(DomainObject* _subject) const
{
	Character* character = dynamic_cast<Character*>(_subject);
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET name = '%1' "
					" WHERE id = %2 "
					)
			.arg(character->name())
			.arg(character->id().value());
	return updateStatement;
}

QString CharacterMapper::deleteStatement(DomainObject* _subject) const
{
	return "DELETE FROM " + TABLE_NAME + " WHERE id = " + QString::number(_subject->id().value());
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
