#ifndef ABSTRACTMAPPER_H
#define ABSTRACTMAPPER_H

#include <Domain/DomainObject.h>

#include <QMap>

//
// Используется включение, вместо предварительного объявления, чтобы использовать в наследниках
//
#include <QSqlRecord>

class QSqlQuery;

using namespace Domain;


namespace DataMappingLayer
{
	class AbstractMapper
	{
	public:
		/**
		 * @brief Очистить все загруженные ранее данные
		 */
		void clear();

	protected:
		virtual QString findStatement(const Identifier&) const = 0;
		virtual QString findAllStatement() const = 0;
		virtual QString insertStatement(DomainObject*, QVariantList&) const = 0;
		virtual QString updateStatement(DomainObject*, QVariantList&) const = 0;
		virtual QString deleteStatement(DomainObject*, QVariantList&) const = 0;

	protected:
		virtual DomainObject* doLoad(const Identifier& _id, const QSqlRecord& _record) = 0;
		virtual DomainObjectsItemModel* modelInstance() = 0;

	protected:
		DomainObject * abstractFind(const Identifier& _id);
		DomainObjectsItemModel * abstractFindAll(const QString& _filter = QString());
		void abstractInsert(DomainObject* _subject);
		void abstractUpdate(DomainObject* _subject);
		void abstractDelete(DomainObject* _subject);

	protected:
		AbstractMapper();

	private:
		/**
		 * @brief Загрузить объект из БД по его идентификатору
		 */
		DomainObject* loadObjectFromDatabase(const Identifier& _id);

		/**
		 * @brief Получить идентификатор для нового объекта на сохранение в БД
		 */
		Identifier findNextIdentifier();

		/**
		 * @brief Загрузить объект из записи в БД
		 */
		DomainObject* load(const QSqlRecord& _record);

		/**
		 * @brief Выполнить запрос
		 */
		void executeSql(QSqlQuery& _sqlQuery);

	private:
		/**
		 * @brief Загруженные объекты из базы данных
		 */
		QMap<Identifier, DomainObject*> m_loadedObjectsMap;
	};

}

#endif // ABSTRACTMAPPER_H
