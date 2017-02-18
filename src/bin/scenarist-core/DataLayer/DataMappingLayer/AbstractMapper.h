#ifndef ABSTRACTMAPPER_H
#define ABSTRACTMAPPER_H

#include <Domain/DomainObject.h>

#include <QMap>
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

        /**
         * @brief Обновить элементы списка
         */
        void refresh(DomainObjectsItemModel* _model);

    protected:
        virtual QString findStatement(const Identifier&) const = 0;
        virtual QString findAllStatement() const = 0;
        virtual QString insertStatement(DomainObject*, QVariantList&) const = 0;
        virtual QString updateStatement(DomainObject*, QVariantList&) const = 0;
        virtual QString deleteStatement(DomainObject*, QVariantList&) const = 0;

    protected:
        /**
         * @brief Создать объект с заданным идентификатором из sql-записи
         */
        virtual DomainObject* doLoad(const Identifier& _id, const QSqlRecord& _record) = 0;

        /**
         * @brief Обновить параметры заданного объекта из sql-записи
         */
        virtual void doLoad(DomainObject* _domainObject, const QSqlRecord& _record) = 0;

        /**
         * @brief Получить экземпляр класса списка объектов
         * @note Шаблонный метод
         */
        virtual DomainObjectsItemModel* modelInstance() = 0;

    protected:
        DomainObject * abstractFind(const Identifier& _id);
        DomainObjectsItemModel * abstractFindAll(const QString& _filter = QString());
        void abstractInsert(DomainObject* _subject);
        bool abstractUpdate(DomainObject* _subject);
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
         * @brief Загрузить или обновить объект из записи в БД
         */
        DomainObject* load(const QSqlRecord& _record);

        /**
         * @brief Выполнить запрос
         */
        bool executeSql(QSqlQuery& _sqlQuery);

    private:
        /**
         * @brief Загруженные объекты из базы данных
         */
        QMap<Identifier, DomainObject*> m_loadedObjectsMap;
    };

}

#endif // ABSTRACTMAPPER_H
