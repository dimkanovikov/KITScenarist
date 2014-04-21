#include "StorageFacade.h"

#include "PlaceStorage.h"
#include "LocationStorage.h"
#include "LocationPhotoStorage.h"
#include "ScenarioDayStorage.h"
#include "TimeStorage.h"
#include "CharacterStorage.h"
#include "ScenarioStorage.h"
#include "SettingsStorage.h"

using namespace DataStorageLayer;


void StorageFacade::clearStorages()
{
	placeStorage()->clear();
	locationStorage()->clear();
	scenarioDayStorage()->clear();
	timeStorage()->clear();
	characterStorage()->clear();
	scenarioStorage()->clear();
}

PlaceStorage* StorageFacade::placeStorage()
{
	if (s_placeStorage == 0) {
		s_placeStorage = new PlaceStorage;
	}
	return s_placeStorage;
}

LocationStorage* StorageFacade::locationStorage()
{
	if (s_locationStorage == 0) {
		s_locationStorage = new LocationStorage;
	}
	return s_locationStorage;
}

LocationPhotoStorage* StorageFacade::locationPhotoStorage()
{
	if (s_locationPhotoStorage == 0) {
		s_locationPhotoStorage = new LocationPhotoStorage;
	}
	return s_locationPhotoStorage;
}

ScenarioDayStorage* StorageFacade::scenarioDayStorage()
{
	if (s_scenarioDayStorage == 0) {
		s_scenarioDayStorage = new ScenarioDayStorage;
	}
	return s_scenarioDayStorage;
}

TimeStorage* StorageFacade::timeStorage()
{
	if (s_timeStorage == 0) {
		s_timeStorage = new TimeStorage;
	}
	return s_timeStorage;
}

CharacterStorage* StorageFacade::characterStorage()
{
	if (s_characterStorage == 0) {
		s_characterStorage = new CharacterStorage;
	}
	return s_characterStorage;
}

ScenarioStorage* StorageFacade::scenarioStorage()
{
	if (s_scenarioStorage == 0) {
		s_scenarioStorage = new ScenarioStorage;
	}
	return s_scenarioStorage;
}

SettingsStorage* StorageFacade::settingsStorage()
{
	if (s_settingsStorage == 0) {
		s_settingsStorage = new SettingsStorage;
	}
	return s_settingsStorage;
}

PlaceStorage* StorageFacade::s_placeStorage = 0;
LocationStorage* StorageFacade::s_locationStorage = 0;
LocationPhotoStorage* StorageFacade::s_locationPhotoStorage = 0;
ScenarioDayStorage* StorageFacade::s_scenarioDayStorage = 0;
TimeStorage* StorageFacade::s_timeStorage = 0;
CharacterStorage* StorageFacade::s_characterStorage = 0;
ScenarioStorage* StorageFacade::s_scenarioStorage = 0;
SettingsStorage* StorageFacade::s_settingsStorage = 0;
