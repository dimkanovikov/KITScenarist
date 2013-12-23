#ifndef SCENARIODAYSTORAGE_H
#define SCENARIODAYSTORAGE_H

#include "StorageFacade.h"

class QString;

namespace Domain {
	class ScenarioDay;
	class ScenarioDaysTable;
}

using namespace Domain;


namespace StorageLayer
{
	class ScenarioDayStorage
	{
	public:
		/**
		 * @brief Все дни сценария
		 */
		ScenarioDaysTable* all();

		/**
		 * @brief Сохранить сценарный день
		 */
		ScenarioDay* storeScenarioDay(const QString& _scenarioDayName) const;

	private:
		ScenarioDaysTable* m_all;

	private:
		ScenarioDayStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // SCENARIODAYSTORAGE_H
