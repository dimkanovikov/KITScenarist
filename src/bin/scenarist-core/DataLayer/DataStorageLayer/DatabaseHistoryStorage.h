#ifndef DATABASEHISTORYSTORAGE_H
#define DATABASEHISTORYSTORAGE_H

#include <QMap>


namespace DataStorageLayer
{
    /**
     * @brief Класс для доступа к изменениям данных сценария
     */
    class DatabaseHistoryStorage
    {
    public:
        /**
         * @brief Получить список uuid'ов изменений с указанной даты и времени
         */
        QList<QString> history(const QString& _fromDatetime);

        /**
         * @brief Получить последнюю запись из таблицы изменений
         */
        QMap<QString, QString> last();

        /**
         * @brief Получить запись из таблицы изменений по UUID
         */
        QMap<QString, QString> historyRecord(const QString& _uuid);

        /**
         * @brief Содержится ли изменение с заданным uuid'ом в БД
         */
        bool contains(const QString& _uuid) const;

        /**
         * @brief Сохранить изменение данных и применить его
         */
        void storeAndApplyHistoryRecord(const QString& _uuid, const QString& _query,
            const QString& _queryValues, const QString& _username, const QString& _datetime);

    private:
        DatabaseHistoryStorage();

        // Для доступа к конструктору
        friend class StorageFacade;
    };
}

#endif // DATABASEHISTORYSTORAGE_H
