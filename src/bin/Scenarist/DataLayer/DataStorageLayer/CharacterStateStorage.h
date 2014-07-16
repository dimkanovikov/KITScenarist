#ifndef CHARACTERSTATESTORAGE_H
#define CHARACTERSTATESTORAGE_H

#include "StorageFacade.h"

class QString;

namespace Domain {
	class CharacterState;
	class CharacterStatesTable;
}

using namespace Domain;


namespace DataStorageLayer
{
	class CharacterStateStorage
	{
	public:
		/**
		 * @brief Получить все места
		 */
		CharacterStatesTable* all();

		/**
		 * @brief Сохранить место
		 */
		CharacterState* storeCharacterState(const QString& _characterStateName);

		/**
		 * @brief Проверить наличие заданного места
		 */
		bool hasCharacterState(const QString& _name);

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

		/**
		 * @brief Ожидание завершения всех операций с БД
		 */
		void wait();

	private:
		CharacterStatesTable* m_all;

	private:
		CharacterStateStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // CHARACTERSTATESTORAGE_H
