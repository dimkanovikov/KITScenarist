#ifndef PLACESTORAGE_H
#define PLACESTORAGE_H

#include "StorageFacade.h"

class QString;

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
		 * @brief Сохранить место
		 */
		Place* storePlace(const QString& _placeName);

        /**
         * @brief Проверить наличие заданного места
         */
        bool hasPlace(const QString& _name);

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

        /**
         * @brief Ожидание завершения всех операций с БД
         */
        void wait();

	private:
		PlacesTable* m_all;

	private:
		PlaceStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // PLACESTORAGE_H
