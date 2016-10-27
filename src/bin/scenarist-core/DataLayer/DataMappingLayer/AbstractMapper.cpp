#include "AbstractMapper.h"

#include <DataLayer/Database/Database.h>
#include <DataLayer/Database/DatabaseHelper.h>

#include <3rd_party/Helpers/QVariantMapWriter.h>

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QUuid>


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

void AbstractMapper::refresh(DomainObjectsItemModel* _model)
{
	QSqlQuery query = Database::query();
	query.prepare(findAllStatement());
	query.exec();
	QSet<Identifier> updatedObjectsIds;
	//
	// Для каждого объекта из БД
	//
	while (query.next()) {
		const QSqlRecord record = query.record();
		//
		// ... загружаем или обновляем
		//
		DomainObject *domainObject = load(record);
		//
		// ... сохраняем идентификатор обновлённого объекта
		//
		updatedObjectsIds.insert(domainObject->id());
		//
		// ... добавляем объект в список для обновления
		//
		if (!_model->contains(domainObject)) {
			_model->append(domainObject);
		}
		//
		// ... уведомляем клиентов модели, что объект обновлися
		//
		else {
			const QModelIndex domainObjectIndex = _model->indexForItem(domainObject);
			emit _model->dataChanged(domainObjectIndex, domainObjectIndex);
		}
	}

	//
	// Удаляем все объекты, которых нет в БД
	//
	foreach (DomainObject* domainObject, _model->toList()) {
		if (!updatedObjectsIds.contains(domainObject->id())) {
			//
			// ... из списка для обновления
			//
			_model->remove(domainObject);
			//
			// ... из карты загруженных объектов
			//
			m_loadedObjectsMap.remove(domainObject->id());
			//
			// ... сам объект
			//
			delete domainObject;
			domainObject = 0;
		}
	}
}

DomainObject* AbstractMapper::abstractFind(const Identifier& _id)
{
	DomainObject* result = m_loadedObjectsMap.value(_id, 0);
	if (!DomainObject::isValid(result)) {
		result = loadObjectFromDatabase(_id);
	}
	return result;
}

DomainObjectsItemModel* AbstractMapper::abstractFindAll(const QString& _filter)
{
	QSqlQuery query = Database::query();
	query.prepare(findAllStatement() + _filter);
	query.exec();
	DomainObjectsItemModel* result = modelInstance();
	while (query.next()) {
		QSqlRecord record = query.record();
		DomainObject* domainObject = load(record);
		result->append(domainObject);
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
		if (executeSql(q_update)) {
			//
			// Изменения сохранены
			//
			_subject->changesStored();
		}
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
	if (executeSql(q_delete)) {
		//
		// Удалим объекст из списка загруженных
		//
		m_loadedObjectsMap.remove(_subject->id());
		delete _subject;
		_subject = 0;
	}
}

DomainObject* AbstractMapper::loadObjectFromDatabase(const Identifier& _id)
{
	QSqlQuery query = Database::query();
	query.prepare( findStatement( _id ) );
	query.exec();
	query.next();
	QSqlRecord record = query.record();
	DomainObject* result = load( record );
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

	const int idValue = _record.value("id").toInt();
	if (idValue != 0) {
		Identifier id(idValue);
		//
		// Если объект загружен, обновляем его и используем указатель на него
		//
		if (m_loadedObjectsMap.contains(id)) {
			doLoad(m_loadedObjectsMap.value(id), _record);
			result = m_loadedObjectsMap.value(id);
		}
		//
		// В противном случае создаём новый объект и сохраняем указатель на него
		//
		else {
			result = doLoad(id, _record);
			m_loadedObjectsMap.insert(id, result);
		}
	}

	return result;
}

bool AbstractMapper::executeSql(QSqlQuery& _sqlQuery)
{
	//
	// Если запрос завершился с ошибкой, выводим отладочную информацию
	//
	if (!_sqlQuery.exec()) {
		Database::setLastError(_sqlQuery.lastError().text());

		qDebug() << _sqlQuery.lastError();
		qDebug() << _sqlQuery.lastQuery();
		qDebug() << _sqlQuery.boundValues();

		return false;
	}
	//
	// Если всё завершилось успешно сохраняем запрос и данные в таблицу истории запросов
	//
	else {
		Database::setLastError(QString::null);

		//
		// NOTE: Оптимизация размера файла проекта
		// Сохраняем всё, кроме изменений сценария и текста самого сценария
		//
		if (!_sqlQuery.lastQuery().contains(" scenario_changes ")
			&& !_sqlQuery.lastQuery().contains(" scenario ")) {

			QSqlQuery q_history(_sqlQuery);
			q_history.prepare("INSERT INTO _database_history (id, query, query_values, datetime) VALUES(?, ?, ?, ?);");
			//
			// ... uuid
			//
			q_history.addBindValue(QUuid::createUuid().toString());
			//
			// ... запрос
			//
			q_history.addBindValue(_sqlQuery.lastQuery());
			//
			// ... данные в сжатом виде
			//
			QString valueString = QVariantMapWriter::mapToDataString(_sqlQuery.boundValues());
			valueString = DatabaseHelper::compress(valueString);
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

	return true;
}
