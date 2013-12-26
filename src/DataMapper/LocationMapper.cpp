#include "LocationMapper.h"

#include <Domain/Location.h>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, parent_id, name ";
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

QString LocationMapper::maxIdStatement() const
{
	return "SELECT MAX(id) FROM  " + TABLE_NAME;
}

QString LocationMapper::insertStatement(DomainObject* _subject, QVariantList& _insertValues) const
{
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, parent_id, name) "
					" VALUES(?, ?, ?) "
					);

	Location* location = dynamic_cast<Location*>(_subject );
	_insertValues.clear();
	_insertValues.append(location->id().value());
	_insertValues.append(DomainObject::isValid(location->parentLocation())
						 ? QString::number(location->parentLocation()->id().value())
						 : "null");
	_insertValues.append(location->name());

	return insertStatement;
}

QString LocationMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET name = ?, "
					" parent_id = ? "
					" WHERE id = ? "
					);

	Location* location = dynamic_cast<Location*>(_subject);
	_updateValues.clear();
	_updateValues.append(location->name());
	_updateValues.append(DomainObject::isValid(location->parentLocation())
						 ? QString::number(location->parentLocation()->id().value())
						 : "null");
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
	Location* parentLocation = 0;
	bool haveParentLocation = !_record.value("parent_id").isNull();
	if (haveParentLocation) {
		Identifier parentLocationId(_record.value("parent_id").toInt());
		parentLocation = find(parentLocationId);
	}
	QString name = _record.value("name").toString();

	return new Location(_id, parentLocation, name);
}

DomainObjectsItemModel* LocationMapper::modelInstance()
{
	return new LocationsTable;
}

LocationMapper::LocationMapper()
{
}
