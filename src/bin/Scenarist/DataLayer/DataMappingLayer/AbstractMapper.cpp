#include "AbstractMapper.h"

#include <DataLayer/Database/Database.h>

#include <3rd_party/Helpers/QVariantMapWriter.h>

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>


using namespace DataMappingLayer;
using namespace DatabaseLayer;


AbstractMapper::AbstractMapper()
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
	QSqlQuery query = Database::query();
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
	QSqlQuery q_insert = Database::query();
	q_insert.prepare(insertQuery);
	foreach (const QVariant& value, insertValues) {
		q_insert.addBindValue(value);
	}

	//
	// Добавим данные в базу
	//
	executeSql(q_insert);
}

void AbstractMapper::abstractUpdate(DomainObject* _subject)
{
	//
	// Если есть не сохранённые изменения
	//
	if (!_subject->isChangesStored()) {
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
		QSqlQuery q_update = Database::query();
		q_update.prepare(updateQuery);
		foreach (const QVariant& value, updateValues) {
			q_update.addBindValue(value);
		}

		//
		// Обновим данные в базе
		//
		executeSql(q_update);

		//
		// Изменения сохранены
		//
		_subject->changesStored();
	}
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
	QSqlQuery q_delete = Database::query();
	q_delete.prepare(deleteQuery);
	foreach (const QVariant& value, deleteValues) {
		q_delete.addBindValue(value);
	}

	//
	// Удалим данные из базы
	//
	executeSql(q_delete);

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
	QSqlQuery query = Database::query();
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

DomainObject* AbstractMapper::load(const QSqlRecord& _record )
{
	DomainObject* result = 0;

	int idValue = _record.value("id").toInt();
	if (idValue != 0) {
		Identifier id(idValue);
		result = doLoad(id, _record);
		m_loadedObjectsMap.insert(id, result);
	}

	return result;
}

void AbstractMapper::executeSql(QSqlQuery& _sqlQuery)
{
	//
	// Если запрос завершился с ошибкой, выводим отладочную информацию
	//
	if (!_sqlQuery.exec()) {
		qDebug() << _sqlQuery.lastError();
		qDebug() << _sqlQuery.lastQuery();
		qDebug() << _sqlQuery.boundValues();
	}
	//
	// Если всё завершилось успешно сохраняем запрос и данные в таблицу истории запросов
	//
	else {
		QSqlQuery q_history(_sqlQuery);
		q_history.prepare("INSERT INTO _database_history (query, query_values, datetime) VALUES(?, ?, ?);");
		//
		// ... запрос
		//
		q_history.addBindValue(_sqlQuery.lastQuery());
		//
		// ... данные
		//
		const QString valueString = QVariantMapWriter::mapToDataString(_sqlQuery.boundValues());
		q_history.addBindValue(valueString);
		//
		// ... время выполнения
		//
		q_history.addBindValue(QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss"));

		//
		// Сохраняем данные
		//
		q_history.exec();
	}
}
