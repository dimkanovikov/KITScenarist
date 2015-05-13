#include "CharacterStateMapper.h"

#include <Domain/CharacterState.h>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, name ";
	const QString TABLE_NAME = " character_states ";
}

CharacterState* CharacterStateMapper::find(const Identifier& _id)
{
	return dynamic_cast<CharacterState*>(abstractFind(_id));
}

CharacterStatesTable* CharacterStateMapper::findAll()
{
	return qobject_cast<CharacterStatesTable*>(abstractFindAll());
}

void CharacterStateMapper::insert(CharacterState* _characterState)
{
	abstractInsert(_characterState);
}

void CharacterStateMapper::update(CharacterState* _characterState)
{
	abstractUpdate(_characterState);
}

QString CharacterStateMapper::findStatement(const Identifier& _id) const
{
	QString findStatement =
			QString("SELECT " + COLUMNS +
					" FROM " + TABLE_NAME +
					" WHERE id = %1 "
					)
			.arg(_id.value());
	return findStatement;
}

QString CharacterStateMapper::findAllStatement() const
{
	return "SELECT " + COLUMNS + " FROM  " + TABLE_NAME;
}

QString CharacterStateMapper::insertStatement(DomainObject* _subject, QVariantList& _insertValues) const
{
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, name) "
					" VALUES(?, ?) "
					);

	CharacterState* characterState = dynamic_cast<CharacterState*>(_subject );
	_insertValues.clear();
	_insertValues.append(characterState->id().value());
	_insertValues.append(characterState->name());

	return insertStatement;
}

QString CharacterStateMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET name = ? "
					" WHERE id = ? "
					);

	CharacterState* characterState = dynamic_cast<CharacterState*>(_subject);
	_updateValues.clear();
	_updateValues.append(characterState->name());
	_updateValues.append(characterState->id().value());

	return updateStatement;
}

QString CharacterStateMapper::deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const
{
	QString deleteStatement = "DELETE FROM " + TABLE_NAME + " WHERE id = ?";

	_deleteValues.clear();
	_deleteValues.append(_subject->id().value());

	return deleteStatement;
}

DomainObject* CharacterStateMapper::doLoad(const Identifier& _id, const QSqlRecord& _record)
{
	const QString name = _record.value("name").toString();

	return new CharacterState(_id, name);
}

void CharacterStateMapper::doLoad(DomainObject* _domainObject, const QSqlRecord& _record)
{
	if (CharacterState* state = dynamic_cast<CharacterState*>(_domainObject)) {
		const QString name = _record.value("name").toString();
		state->setName(name);
	}
}

DomainObjectsItemModel* CharacterStateMapper::modelInstance()
{
	return new CharacterStatesTable;
}

CharacterStateMapper::CharacterStateMapper()
{
}
