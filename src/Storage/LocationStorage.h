#ifndef LOCATIONSTORAGE_H
#define LOCATIONSTORAGE_H

#include "StorageFacade.h"

#include <QMap>

namespace Domain {
	class Location;
	class LocationsTable;
}

using namespace Domain;

namespace StorageLayer
{
	class LocationStorage
	{
	public:
		/**
		 * @brief Получить все локации
		 */
		LocationsTable* all();

		/**
		 * @brief Получить все основные локации
		 */
		LocationsTable* allMainLocations();

		/**
		 * @brief Получить все подлокации
		 */
		LocationsTable* allSublocations();

		/**
		 * @brief Получить все подлокации заданной локации
		 */
		LocationsTable* sublocations(const QString& _locationName);

	private:
		LocationsTable* m_all;
		LocationsTable* m_allMainLocations;
		LocationsTable* m_allSublocations;
		QMap<Location*, LocationsTable*> m_sublocations;

	private:
		LocationStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // LOCATIONSTORAGE_H
