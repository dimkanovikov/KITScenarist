#ifndef LOCATIONPHOTOSTORAGE_H
#define LOCATIONPHOTOSTORAGE_H

#include "StorageFacade.h"

namespace Domain {
	class Location;
	class LocationPhotosTable;
}

using namespace Domain;


namespace DataStorageLayer
{
	class LocationPhotoStorage
	{
	public:
		/**
		 * @brief Получить все фотографии локаций
		 */
		LocationPhotosTable* all();

		/**
		 * @brief Сохранить фотографии локации
		 */
		void store(Location* _location);

		/**
		 * @brief Удалить фотографии локации
		 */
		void remove(Location* _location);

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

	private:
		LocationPhotosTable* m_all;

	private:
		LocationPhotoStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // LOCATIONPHOTOSTORAGE_H
