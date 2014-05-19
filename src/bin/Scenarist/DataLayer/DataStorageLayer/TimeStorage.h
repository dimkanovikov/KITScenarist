#ifndef TIMESTORAGE_H
#define TIMESTORAGE_H

#include "StorageFacade.h"

namespace Domain {
	class Time;
	class TimesTable;
}

using namespace Domain;


namespace DataStorageLayer
{
	class TimeStorage
	{
	public:
		/**
		 * @brief Получить все времена
		 */
		TimesTable* all();

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

        /**
         * @brief Ожидание завершения всех операций с БД
         */
        void wait();

	private:
		TimesTable* m_all;

	private:
		TimeStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // TIMESTORAGE_H
