#include "AbstractMapper.h"

#include "AbstractMapperPrivate.h"

#include <DataLayer/Database/Database.h>

#include <QVariant>
#include <QtSql>


using namespace DataMappingLayer;
using namespace DatabaseLayer;


AbstractMapper::AbstractMapper() :
	m_sqlExecuter(new SqlExecutingQueuedThread)
{
}

AbstractMapper::~AbstractMapper()
{
	//
	// Ожидаем завершения всех операций
	//
	m_sqlExecuter->wait();

	delete m_sqlExecuter;
	m_sqlExecuter = 0;
}

void AbstractMapper::clear()
{
	foreach (DomainObject* domainObject, m_loadedObjectsMap.values()) {
		delete domainObject;
		domainObject = 0;
	}

	m_loadedObjectsMap.clear();
}

DomainObject * AbstractMapper::abstractFind(const Identifier& _id)
{
    DomainObject *result = m_loadedObjectsMap.value( _id, 0 );
    if ( !DomainObject::isValid( result ) ) {
        result = loadObjectFromDatabase( _id );
    }
	return result;
}

DomainObjectsItemModel * AbstractMapper::abstractFindAll(const QString& _filter)
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

void AbstractMapper::abstractInsert(DomainObject* _subject)
{
	//
	// Установим идентификатор для нового объекта
	//
	_subject->setId(findNextIdentifier());

	//
	// Добавим вновь созданный объект в список загруженных объектов
	//
	m_loadedObjectsMap.insert(_subject->id(), _subject);

	//
	// Получим данные для формирования запроса на их добавление
	//
	QVariantList insertValues;
	QString insertQuery = insertStatement(_subject, insertValues);

	//
	// Сформируем запрос на добавление данных в базу
	//
	QSqlQuery q_insert(Database::instanse());
	q_insert.prepare(insertQuery);
	foreach (const QVariant& value, insertValues) {
		q_insert.addBindValue(value);
	}

	//
	// Добавим данные в базу
	//
	m_sqlExecuter->executeSql(q_insert);
//	q_insert.exec();
//	qDebug() << q_insert.lastQuery();
//	qDebug() << q_insert.lastError();
}

void AbstractMapper::abstractUpdate(DomainObject* _subject)
{
	//
    // т.к. в m_loadedObjectsMap хранится список указателей, то после обновления элементов
    // обновлять элемент непосредственно в списке не нужно
	//

	//
	// Получим данные для формирования запроса на их обновление
	//
	QVariantList updateValues;
	QString updateQuery = updateStatement(_subject, updateValues);

	//
	// Сформируем запрос на обновление данных в базе
	//
	QSqlQuery q_update(Database::instanse());
	q_update.prepare(updateQuery);
	foreach (const QVariant& value, updateValues) {
		q_update.addBindValue(value);
	}

	//
	// Обновим данные в базе
	//
	m_sqlExecuter->executeSql(q_update);
//	q_update.exec();
//	qDebug() << q_update.lastQuery();
//	qDebug() << q_update.lastError();
}

void AbstractMapper::abstractDelete(DomainObject* _subject)
{
	//
	// Получим данные для формирования запроса на их удаление
	//
	QVariantList deleteValues;
	QString deleteQuery = deleteStatement(_subject, deleteValues);

	//
	// Сформируем запрос на удаление данных из базы
	//
	QSqlQuery q_delete(Database::instanse());
	q_delete.prepare(deleteQuery);
	foreach (const QVariant& value, deleteValues) {
		q_delete.addBindValue(value);
	}

	//
	// Удалим данные из базы
	//
	m_sqlExecuter->executeSql(q_delete);
//	q_delete.exec();
//	qDebug() << q_delete.lastQuery();
//	qDebug() << q_delete.lastError();

	//
	// Удалим объекст из списка загруженных
	//
	m_loadedObjectsMap.remove(_subject->id());
	delete _subject;
	_subject = 0;
}

DomainObject* AbstractMapper::loadObjectFromDatabase(const Identifier& _id)
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
	Identifier maxId(0);
	if (!m_loadedObjectsMap.isEmpty()) {
		QMap<Identifier, DomainObject*>::const_iterator iter = m_loadedObjectsMap.end();
		--iter;
		maxId = iter.key();
	}
    return maxId.next();
}

DomainObject * AbstractMapper::load(const QSqlRecord& _record )
{
	//
	// FIXME: Очистить от артефактов недвижимости
	//

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


/**
 * Реализация поточного выполнения запросов
 */


SqlExecutingQueuedThread::SqlExecutingQueuedThread(QObject *_parent) :
	QThread(_parent)
{
}

void SqlExecutingQueuedThread::executeSql(const QSqlQuery &_sqlQuery)
{
	m_mutex.lock();
	m_sqlQueue.enqueue(_sqlQuery);
	m_mutex.unlock();

	if (!isRunning()) {
		start();
	}
}

void SqlExecutingQueuedThread::run()
{
	Database::instanse().transaction();

	while (!m_sqlQueue.isEmpty()) {
		QSqlQuery queryToExecute = m_sqlQueue.dequeue();
		queryToExecute.exec();

		qDebug() << queryToExecute.lastQuery();
		qDebug() << queryToExecute.lastError();
		qDebug() << queryToExecute.boundValues();
	}

	Database::instanse().commit();
}
