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
		 * @brief Получить локацию по названию
		 */
		Location* location(const QString& _name);

		/**
		 * @brief Сохранить локацию
		 */
		Location* storeLocation(const QString& _locationName);

		/**
		 * @brief Обновить локацию
		 */
		void updateLocation(Location* _location);

		/**
		 * @brief Удалить локацию
		 */
		void removeLocation(const QString& _name);

		/**
		 * @brief Проверить наличие заданной локации
		 */
		bool hasLocation(const QString& _name);

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
