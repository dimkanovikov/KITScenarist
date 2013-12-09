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

QString PlaceMapper::insertStatement(DomainObject* _subject) const
{
	Place* place = dynamic_cast<Place*>(_subject );
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, name) "
					" VALUES(%1, '%2') "
					)
			.arg(place->id().value())
			.arg(place->name());
	return insertStatement;
}

QString PlaceMapper::updateStatement(DomainObject* _subject) const
{
	Place* place = dynamic_cast<Place*>(_subject);
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET name = '%1' "
					" WHERE id = %2 "
					)
			.arg(place->name())
			.arg(place->id().value());
	return updateStatement;
}

QString PlaceMapper::deleteStatement(DomainObject* _subject) const
{
	return "DELETE FROM " + TABLE_NAME + " WHERE id = " + QString::number(_subject->id().value());
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
