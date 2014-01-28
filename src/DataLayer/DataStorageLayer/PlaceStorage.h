#ifndef PLACESTORAGE_H
#define PLACESTORAGE_H

#include "StorageFacade.h"

namespace Domain {
	class Place;
	class PlacesTable;
}

using namespace Domain;


namespace DataStorageLayer
{
	class PlaceStorage
	{
	public:
		/**
		 * @brief Получить все места
		 */
		PlacesTable* all();

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

	private:
		PlacesTable* m_all;

	private:
		PlaceStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // PLACESTORAGE_H
