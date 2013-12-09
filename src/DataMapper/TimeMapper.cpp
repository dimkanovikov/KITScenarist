#include "TimeMapper.h"

#include <Domain/Time.h>

using namespace DataMappingLayer;


namespace {
	const QString COLUMNS = " id, name ";
	const QString TABLE_NAME = " times ";
}

Time* TimeMapper::find(const Identifier& _id)
{
	return dynamic_cast<Time*>(abstractFind(_id));
}

TimesTable* TimeMapper::findAll()
{
	return qobject_cast<TimesTable*>(abstractFindAll());
}

void TimeMapper::insert(Time* _time)
{
	abstractInsert(_time);
}

void TimeMapper::update(Time* _time)
{
	abstractUpdate(_time);
}

QString TimeMapper::findStatement(const Identifier& _id) const
{
	QString findStatement =
			QString("SELECT " + COLUMNS +
					" FROM " + TABLE_NAME +
					" WHERE id = %1 "
					)
			.arg(_id.value());
	return findStatement;
}

QString TimeMapper::findAllStatement() const
{
	return "SELECT " + COLUMNS + " FROM  " + TABLE_NAME;
}

QString TimeMapper::maxIdStatement() const
{
	return "SELECT MAX(id) FROM  " + TABLE_NAME;
}

QString TimeMapper::insertStatement(DomainObject* _subject) const
{
	Time* time = dynamic_cast<Time*>(_subject );
	QString insertStatement =
			QString("INSERT INTO " + TABLE_NAME +
					" (id, name) "
					" VALUES(%1, '%2') "
					)
			.arg(time->id().value())
			.arg(time->name());
	return insertStatement;
}

QString TimeMapper::updateStatement(DomainObject* _subject) const
{
	Time* time = dynamic_cast<Time*>(_subject);
	QString updateStatement =
			QString("UPDATE " + TABLE_NAME +
					" SET name = '%1' "
					" WHERE id = %2 "
					)
			.arg(time->name())
			.arg(time->id().value());
	return updateStatement;
}

QString TimeMapper::deleteStatement(DomainObject* _subject) const
{
	return "DELETE FROM " + TABLE_NAME + " WHERE id = " + QString::number(_subject->id().value());
}

DomainObject* TimeMapper::doLoad(const Identifier& _id, const QSqlRecord& _record)
{
	QString name = _record.value("name").toString();

	return new Time(_id, name);
}

DomainObjectsItemModel* TimeMapper::modelInstance()
{
	return new TimesTable;
}

TimeMapper::TimeMapper()
{
}
