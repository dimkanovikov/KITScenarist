#include "PlaceMapper.h"

#include <Domain/Place.h>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, name ";
	const QString TABLE_NAME = " places ";
}

Place* PlaceMapper::find(const Identifier& _id)
{
	return dynamic_cast<Place*>(abstractFind(_id));
}

PlacesTable* PlaceMapper::findAll()
{
	return qobject_cast<PlacesTable*>(abstractFindAll());
}

void PlaceMapper::insert(Place* _place)
{
	abstractInsert(_place);
}

void PlaceMapper::update(Place* _place)
{
	abstractUpdate(_place);
}

QString PlaceMapper::findStatement(const Identifier& _id) const
{
	QString findStatement =
			QString("SELECT " + COLUMNS +
					" FROM " + TABLE_NAME +
					" WHERE id = %1 "
					)
			.arg(_id.value());
	return findStatement;
}

QString PlaceMapper::findAllStatement() const
{
	return "SELECT " + COLUMNS + " FROM  " + TABLE_NAME;
}

QString PlaceMapper::maxIdStatement() const
{
	return "SELECT MAX(id) FROM  " + TABLE_NAME;
}

QString PlaceMapper::insertStatement(DomainObject* _subject, QVariantList& _insertValues) const
{
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, name) "
					" VALUES(?, ?) "
					);

	Place* place = dynamic_cast<Place*>(_subject );
	_insertValues.clear();
	_insertValues.append(place->id().value());
	_insertValues.append(place->name());

	return insertStatement;
}

QString PlaceMapper::updateStatement(DomainObject* _subject, QVariantList& _updateValues) const
{
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET name = ? "
					" WHERE id = ? "
					);

	Place* place = dynamic_cast<Place*>(_subject);
	_updateValues.clear();
	_updateValues.append(place->name());
	_updateValues.append(place->id().value());

	return updateStatement;
}

QString PlaceMapper::deleteStatement(DomainObject* _subject, QVariantList& _deleteValues) const
{
	QString deleteStatement = "DELETE FROM " + TABLE_NAME + " WHERE id = ?";

	_deleteValues.clear();
	_deleteValues.append(_subject->id().value());

	return deleteStatement;
}

DomainObject* PlaceMapper::doLoad(const Identifier& _id, const QSqlRecord& _record)
{
	QString name = _record.value("name").toString();

	return new Place(_id, name);
}

DomainObjectsItemModel* PlaceMapper::modelInstance()
{
	return new PlacesTable;
}

PlaceMapper::PlaceMapper()
{
}
