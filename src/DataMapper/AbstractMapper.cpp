#include "AbstractMapper.h"

#include "../Database/Database.h"

#include <QVariant>
#include <QtSql>


using namespace DataMappingLayer;
using namespace DatabaseLayer;


AbstractMapper::AbstractMapper()
{

}

AbstractMapper::~AbstractMapper()
{

}

void AbstractMapper::clear()
{
	foreach (DomainObject* domainObject, m_loadedObjectsMap.values()) {
		delete domainObject;
		domainObject = 0;
	}

	m_loadedObjectsMap.clear();
}

DomainObject * AbstractMapper::abstractFind(const Identifier& _id )
{
    DomainObject *result = m_loadedObjectsMap.value( _id, 0 );
    if ( !DomainObject::isValid( result ) ) {
        result = loadObjectFromDatabase( _id );
    }
	return result;
}

DomainObjectsItemModel * AbstractMapper::abstractFindAll(const QString& _filter )
{
    QSqlQuery query ( Database::instanse() );
    query.prepare( findAllStatement() + _filter );
    query.exec();
    DomainObjectsItemModel * result = modelInstance();
	while ( query.next() ) {
        QSqlRecord record = query.record();
        DomainObject *domainObject = load( record );
		result->append( domainObject );
    }
    return result;
}

void AbstractMapper::abstractInsert( DomainObject *subject )
{
    subject->setId( findNextIdentifier() );
    QSqlQuery query( Database::instanse() );
    query.prepare( insertStatement( subject ) );
    query.exec();
    m_loadedObjectsMap.insert( subject->id(), subject );

    qDebug() << query.lastQuery();
    qDebug() << query.lastError();
}

void AbstractMapper::abstractUpdate( DomainObject *subject )
{
    // т.к. в m_loadedObjectsMap хранится список указателей, то после обновления элементов
    // обновлять элемент непосредственно в списке не нужно

	// Если во время обновления изменился идентификатор объекта,
	// нужно обновить идентификатор в списке загруженных
	Identifier previousId = subject->id();
	QSqlQuery query( Database::instanse() );
    query.prepare( updateStatement( subject ) );
    query.exec();
	if ( previousId != subject->id() ) {
		m_loadedObjectsMap.insert( subject->id(), subject );
	}

    qDebug() << query.lastQuery();
    qDebug() << query.lastError();
}

void AbstractMapper::abstractRemove( DomainObject *subject )
{
    QSqlQuery query( Database::instanse() );
    query.prepare( deleteStatement( subject ) );
    query.exec();
    m_loadedObjectsMap.remove( subject->id() );
    delete subject;
    subject = 0;

    qDebug() << query.lastQuery();
    qDebug() << query.lastError();
}

DomainObject *AbstractMapper::loadObjectFromDatabase(const Identifier& _id )
{
    DomainObject *result = 0;
    QSqlQuery query( Database::instanse() );
    query.prepare( findStatement( _id ) );
    query.exec();
    query.next();
    QSqlRecord record = query.record();
    result = load( record );
    return result;
}

Identifier AbstractMapper::findNextIdentifier()
{
    QSqlQuery query( Database::instanse() );
    query.prepare( maxIdStatement() );
    query.exec();
    query.next();
    Identifier maxId( query.value(0).toInt() );
    return maxId.next();
}

DomainObject * AbstractMapper::load(const QSqlRecord& _record )
{
    int value   = _record.value( "id" ).toInt();
    int version = _record.value( "version" ).toInt();

    if ( value == 0 )
        return 0;
//    Q_ASSERT_X( value != 0,
//                "Не установлен идентификатор при выборке объектов из базы данных",
//                findAllStatement().toUtf8() );

    Identifier id( value, version );
    DomainObject *result = 0;
	// Объекты домена зависящие от объекта недвижимости - особый случай
	bool isDependFromObject = _record.contains( "fk_object" ) && _record.contains( "fk_object_version" );
	if ( !isDependFromObject
		 && m_loadedObjectsMap.contains( id ) ) {
        result = m_loadedObjectsMap.value( id );
    } else {
        result = doLoad( id, _record );
        m_loadedObjectsMap.insert( id, result );
    }
    return result;
}
