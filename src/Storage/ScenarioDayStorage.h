#ifndef SCENARIODAYSTORAGE_H
#define SCENARIODAYSTORAGE_H

#include "StorageFacade.h"

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
		ScenarioDaysTable* all();

	private:
		ScenarioDaysTable* m_all;

	private:
		ScenarioDayStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // SCENARIODAYSTORAGE_H
