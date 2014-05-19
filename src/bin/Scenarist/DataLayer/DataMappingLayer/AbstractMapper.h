#ifndef ABSTRACTMAPPER_H
#define ABSTRACTMAPPER_H

#include <Domain/Identifier.h>
#include <Domain/DomainObject.h>

#include <QHash>
#include <QSqlRecord>

using namespace Domain;


namespace DataMappingLayer
{
	class SqlExecutingQueuedThread;

	class AbstractMapper
	{
	public:
		/**
		 * @brief Очистить все загруженные ранее данные
		 */
		void clear();

        /**
         * @brief Ожидание завершения всех операций с БД
         */
        void wait();

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
		virtual ~AbstractMapper();

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

	private:
		/**
		 * @brief Загруженные объекты из базы данных
		 */
		QMap<Identifier, DomainObject*> m_loadedObjectsMap;

		/**
		 * @brief Поток для выполнения операций сохранения, обновления и удаления записей из БД
		 */
		SqlExecutingQueuedThread* m_sqlExecuter;
	};

}

#endif // ABSTRACTMAPPER_H
