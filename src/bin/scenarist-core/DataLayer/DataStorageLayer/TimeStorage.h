#ifndef TIMESTORAGE_H
#define TIMESTORAGE_H

#include "StorageFacade.h"

class QString;

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
		 * @brief Сохранить время
		 */
		Time* storeTime(const QString& _timeName);

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

		/**
		 * @brief Обновить хранилище
		 */
		void refresh();

	private:
		TimesTable* m_all;

	private:
		TimeStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // TIMESTORAGE_H
