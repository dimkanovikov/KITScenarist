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
		 * @brief Получить таблицу изменений с указанной даты и времени
		 */
		QList<QMap<QString, QString> > history(const QString& _fromDatetime);

	private:
		DatabaseHistoryStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // DATABASEHISTORYSTORAGE_H
