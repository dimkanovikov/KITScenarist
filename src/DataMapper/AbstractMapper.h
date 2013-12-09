#ifndef ABSTRACTMAPPER_H
#define ABSTRACTMAPPER_H

#include "../Domain/Identifier.h"
#include "../Domain/DomainObject.h"

#include <QHash>
#include <QSqlRecord>

using namespace Domain;

namespace DataMappingLayer
{

class AbstractMapper
{
protected:
	virtual QString findStatement(const Identifier&) const = 0;
    virtual QString findAllStatement() const = 0;
    virtual QString maxIdStatement() const = 0;
	virtual QString insertStatement(DomainObject*) const = 0;
	virtual QString updateStatement(DomainObject*) const = 0;
	virtual QString deleteStatement(DomainObject*) const = 0;

protected:
	virtual DomainObject* doLoad(const Identifier& _id, const QSqlRecord& _record) = 0;
	virtual DomainObjectsItemModel* modelInstance() = 0;

protected:
	DomainObject * abstractFind(const Identifier& _id);
	DomainObjectsItemModel * abstractFindAll(const QString& _filter = QString());
	void abstractInsert(DomainObject* subject);
	void abstractUpdate(DomainObject* subject);
	void abstractRemove(DomainObject* subject);

protected:
    AbstractMapper();
	virtual ~AbstractMapper();

private:
	DomainObject* loadObjectFromDatabase(const Identifier& _id);
    Identifier findNextIdentifier();
	DomainObject* load(const QSqlRecord& _record);

private:
	QMap<Identifier, DomainObject*> m_loadedObjectsMap;
};

}

#endif // ABSTRACTMAPPER_H
