#ifndef RESEARCHSTORAGE_H
#define RESEARCHSTORAGE_H

#include "StorageFacade.h"

#include <QMap>

namespace Domain {
	class Research;
	class ResearchTable;
}

using namespace Domain;


namespace DataStorageLayer
{
	class ResearchStorage
	{
	public:
		/**
		 * @brief Получить все разработки
		 */
		ResearchTable* all();

		/**
		 * @brief Получить разработку по названию
		 */
		Research* research(const QString& _name);

		/**
		 * @brief Сохранить разработку
		 */
		Research* storeResearch(Research* _parent, int _researchType, const QString& _researchName, int _sortOrder);

		/**
		 * @brief Обновить разработку
		 */
		void updateResearch(Research* _research);

		/**
		 * @brief Удалить разработку
		 */
		void removeResearch(Research* _research);

		/**
		 * @brief Проверить наличие заданной разработки
		 */
		bool hasResearch(Research* _research);

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

		/**
		 * @brief Обновить хранилище
		 */
		void refresh();

	private:
		ResearchTable* m_all;

	private:
		ResearchStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // RESEARCHSTORAGE_H
