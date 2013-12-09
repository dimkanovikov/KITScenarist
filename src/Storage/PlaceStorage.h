#ifndef PLACESTORAGE_H
#define PLACESTORAGE_H

#include "StorageFacade.h"

namespace Domain {
	class Place;
	class PlacesTable;
}

using namespace Domain;


namespace StorageLayer
{
	class PlaceStorage
	{
	public:
		PlacesTable* all();

	private:
		PlacesTable* m_all;

	private:
		PlaceStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // PLACESTORAGE_H
