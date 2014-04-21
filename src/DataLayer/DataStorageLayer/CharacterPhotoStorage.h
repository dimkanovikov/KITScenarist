#ifndef CHARACTERPHOTOSTORAGE_H
#define CHARACTERPHOTOSTORAGE_H

#include "StorageFacade.h"

namespace Domain {
	class Character;
	class CharacterPhotosTable;
}

using namespace Domain;


namespace DataStorageLayer
{
	class CharacterPhotoStorage
	{
	public:
		/**
		 * @brief Получить все фотографии локаций
		 */
		CharacterPhotosTable* all();

		/**
		 * @brief Сохранить фотографии локации
		 */
		void store(Character* _character);

		/**
		 * @brief Удалить фотографии локации
		 */
		void remove(Character* _character);

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

	private:
		CharacterPhotosTable* m_all;

	private:
		CharacterPhotoStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // CHARACTERPHOTOSTORAGE_H
