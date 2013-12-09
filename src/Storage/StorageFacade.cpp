#include "StorageFacade.h"

#include "PlaceStorage.h"
#include "LocationStorage.h"
#include "ScenarioDayStorage.h"
#include "TimeStorage.h"
#include "CharacterStorage.h"

using namespace StorageLayer;


PlaceStorage* StorageFacade::placeStorage()
{
	if (s_placeStorage == 0) {
		s_placeStorage = new PlaceStorage;
	}
	return s_placeStorage;
}

LocationStorage*StorageFacade::locationStorage()
{
	if (s_locationStorage == 0) {
		s_locationStorage = new LocationStorage;
	}
	return s_locationStorage;
}

ScenarioDayStorage*StorageFacade::scenarioDayStorage()
{
	if (s_scenarioDayStorage == 0) {
		s_scenarioDayStorage = new ScenarioDayStorage;
	}
	return s_scenarioDayStorage;
}

TimeStorage*StorageFacade::timeStorage()
{
	if (s_timeStorage == 0) {
		s_timeStorage = new TimeStorage;
	}
	return s_timeStorage;
}

CharacterStorage*StorageFacade::characterStorage()
{
	if (s_characterStorage == 0) {
		s_characterStorage = new CharacterStorage;
	}
	return s_characterStorage;
}

PlaceStorage* StorageFacade::s_placeStorage = 0;
LocationStorage* StorageFacade::s_locationStorage = 0;
ScenarioDayStorage* StorageFacade::s_scenarioDayStorage = 0;
TimeStorage* StorageFacade::s_timeStorage = 0;
CharacterStorage* StorageFacade::s_characterStorage = 0;
