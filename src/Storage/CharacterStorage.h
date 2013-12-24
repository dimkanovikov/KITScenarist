#ifndef CHARACTERSTORAGE_H
#define CHARACTERSTORAGE_H

#include "StorageFacade.h"

class QString;

namespace Domain {
	class Character;
	class CharactersTable;
}

using namespace Domain;


namespace StorageLayer
{
	class CharacterStorage
	{
	public:
		/**
		 * @brief Все персонажи сценария
		 */
		CharactersTable* all();

		/**
		 * @brief Сохранить персонажа
		 */
		Character* storeCharacter(const QString& _name);

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

	private:
		CharactersTable* m_all;

	private:
		CharacterStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // CHARACTERSTORAGE_H
