#include "LocationMapper.h"

#include <Domain/Location.h>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, name, description ";
	const QString TABLE_NAME = " locations ";
}

Location* LocationMapper::find(const Identifier& _id)
{
	return dynamic_cast<Location*>(abstractFind(_id));
}

LocationsTable* LocationMapper::findAll()
{
	return qobject_cast<LocationsTable*>(abstractFindAll());
}

void LocationMapper::insert(Location* _location)
{
	abstractInsert(_location);
}

void LocationMapper::update(Location* _location)
{
	abstractUpdate(_location);
}

void LocationMapper::remove(Location* _location)
{
	abstractDelete(_location);
}

QString LocationMapper::findStatement(const Identifier& _id) const
{
	QString findStatement =
			QString("SELECT " + COLUMNS +
					" FROM " + TABLE_NAME +
					" WHERE id = %1 "
					)
			.arg(_id.value());
	return findStatement;
}

QString LocationMapper::findAllStatement() const
{
	return "SELECT " + COLUMNS + " FROM  " + TABLE_NAME;
}

QString LocationMapper::insertStatement(DomainObject* _subject, QVariantList& _insertValues) const
{
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, name, description) "
					" VALUES(?, ?, ?) "
					);

	Location* location = dynamic_cast<Location*>(_subject );
	_insertValues.clear();
	_insertValues.append(location->id().value());
	_insertValues.append(location->name());
	_insertValues.append(location->description());

	return insertStatement;
}

QString LocationMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET name = ?, "
					" description = ? "
					" WHERE id = ? "
					);

	Location* location = dynamic_cast<Location*>(_subject);
	_updateValues.clear();
	_updateValues.append(location->name());
	_updateValues.append(location->description());
	_updateValues.append(location->id().value());

	return updateStatement;
}

QString LocationMapper::deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const
{
	QString deleteStatement = "DELETE FROM " + TABLE_NAME + " WHERE id = ?";

	_deleteValues.clear();
	_deleteValues.append(_subject->id().value());

	return deleteStatement;
}

DomainObject* LocationMapper::doLoad(const Identifier& _id, const QSqlRecord& _record)
{
	QString name = _record.value("name").toString();
	QString description = _record.value("description").toString();

	return new Location(_id, name, description);
}

DomainObjectsItemModel* LocationMapper::modelInstance()
{
	return new LocationsTable;
}

LocationMapper::LocationMapper()
{
}
