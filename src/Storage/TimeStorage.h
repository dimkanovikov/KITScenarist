#ifndef TIMESTORAGE_H
#define TIMESTORAGE_H

#include "StorageFacade.h"

namespace Domain {
	class Time;
	class TimesTable;
}

using namespace Domain;


namespace StorageLayer
{
	class TimeStorage
	{
	public:
		TimesTable* all();

	private:
		TimesTable* m_all;

	private:
		TimeStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // TIMESTORAGE_H
