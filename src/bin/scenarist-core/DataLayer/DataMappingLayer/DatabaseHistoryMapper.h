#ifndef DATABASEHISTORYMAPPER_H
#define DATABASEHISTORYMAPPER_H

#include <QMap>


namespace DataMappingLayer
{
	class DatabaseHistoryMapper
	{
	public:
		/**
		 * @brief Получить список uuid'ов изменений с указанной даты и времени
		 */
		QList<QString> history(const QString& _fromDatetime);

		/**
		 * @brief Получить запись из таблицы изменений по UUID
		 */
		QMap<QString, QString> historyRecord(const QString& _uuid);

		/**
		 * @brief Содержится ли изменение с заданным uuid'ом в БД
		 */
		bool contains(const QString& _uuid) const;

		/**
		 * @brief Сохранить изменение данных
		 */
		void storeHistoryRecord(const QString& _uuid, const QString& _query,
            const QString& _queryValues, const QString& _username, const QString& _datetime);

		/**
		 * @brief Применить изменение данных
		 */
		void applyHistoryRecord(const QString& _query, const QString& _queryValues);

	private:
		DatabaseHistoryMapper();

		// Для доступа к конструктору
		friend class MapperFacade;
	};
}

#endif // DATABASEHISTORYMAPPER_H
