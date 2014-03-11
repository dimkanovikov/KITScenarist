#ifndef LOCATIONSTORAGE_H
#define LOCATIONSTORAGE_H

#include "StorageFacade.h"

#include <QMap>

namespace Domain {
	class Location;
	class LocationsTable;
}

using namespace Domain;

namespace DataStorageLayer
{
	class LocationStorage
	{
	public:
		/**
		 * @brief Получить все локации
		 */
		LocationsTable* all();

		/**
		 * @brief Сохранить основную локацию
		 */
		Location* storeLocation(const QString& _locationName);

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

	private:
		LocationsTable* m_all;

	private:
		LocationStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // LOCATIONSTORAGE_H
