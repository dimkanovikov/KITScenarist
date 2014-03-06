#ifndef CHARACTERSTORAGE_H
#define CHARACTERSTORAGE_H

#include "StorageFacade.h"

class QString;

namespace Domain {
	class Character;
	class CharactersTable;
}

using namespace Domain;


namespace DataStorageLayer
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
		 * @brief Обновить персонажа
		 */
		Character* updateCharacter(const QString& _oldName, const QString& _newName);

		/**
		 * @brief Удалить персонажа
		 */
		void removeCharacter(const QString& _name);

		/**
		 * @brief Проверить наличие заданного персонажа
		 */
		bool hasCharacter(const QString& _name);

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
