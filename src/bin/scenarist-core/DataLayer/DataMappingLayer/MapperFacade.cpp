#include "MapperFacade.h"

#include "PlaceMapper.h"
#include "LocationMapper.h"
#include "LocationPhotoMapper.h"
#include "ScenarioDayMapper.h"
#include "TimeMapper.h"
#include "CharacterMapper.h"
#include "CharacterStateMapper.h"
#include "CharacterPhotoMapper.h"
#include "ScenarioMapper.h"
#include "ScenarioChangeMapper.h"
#include "ScenarioDataMapper.h"
#include "ResearchMapper.h"
#include "SettingsMapper.h"
#include "DatabaseHistoryMapper.h"

using namespace DataMappingLayer;


PlaceMapper* MapperFacade::placeMapper()
{
	if (s_placeMapper == 0) {
		s_placeMapper = new PlaceMapper;
	}
	return s_placeMapper;
}

LocationMapper* MapperFacade::locationMapper()
{
	if (s_locationMapper == 0) {
		s_locationMapper = new LocationMapper;
	}
	return s_locationMapper;
}

LocationPhotoMapper* MapperFacade::locationPhotoMapper()
{
	if (s_locationPhotoMapper == 0) {
		s_locationPhotoMapper = new LocationPhotoMapper;
	}
	return s_locationPhotoMapper;
}

ScenarioDayMapper* MapperFacade::scenarioDayMapper()
{
	if (s_scenarioDayMapper == 0) {
		s_scenarioDayMapper = new ScenarioDayMapper;
	}
	return s_scenarioDayMapper;
}

TimeMapper* MapperFacade::timeMapper()
{
	if (s_timeMapper == 0) {
		s_timeMapper = new TimeMapper;
	}
	return s_timeMapper;
}

CharacterMapper* MapperFacade::characterMapper()
{
	if (s_characterMapper == 0) {
		s_characterMapper = new CharacterMapper;
	}
	return s_characterMapper;
}

CharacterStateMapper* MapperFacade::characterStateMapper()
{
	if (s_characterStateMapper == 0) {
		s_characterStateMapper = new CharacterStateMapper;
	}
	return s_characterStateMapper;
}

CharacterPhotoMapper* MapperFacade::characterPhotoMapper()
{
	if (s_characterPhotoMapper == 0) {
		s_characterPhotoMapper = new CharacterPhotoMapper;
	}
	return s_characterPhotoMapper;
}

ScenarioMapper* MapperFacade::scenarioMapper()
{
	if (s_scenarioMapper == 0) {
		s_scenarioMapper = new ScenarioMapper;
	}
	return s_scenarioMapper;
}

ScenarioChangeMapper* MapperFacade::scenarioChangeMapper()
{
	if (s_scenarioChangeMapper == 0) {
		s_scenarioChangeMapper = new ScenarioChangeMapper;
	}
	return s_scenarioChangeMapper;
}

ScenarioDataMapper* MapperFacade::scenarioDataMapper()
{
	if (s_scenarioDataMapper == 0) {
		s_scenarioDataMapper = new ScenarioDataMapper;
	}
	return s_scenarioDataMapper;
}

ResearchMapper* MapperFacade::researchMapper()
{
	if (s_researchMapper == 0) {
		s_researchMapper = new ResearchMapper;
	}
	return s_researchMapper;
}

SettingsMapper* MapperFacade::settingsMapper()
{
	if (s_settingsMapper == 0) {
		s_settingsMapper = new SettingsMapper;
	}
	return s_settingsMapper;
}

DatabaseHistoryMapper* MapperFacade::databaseHistoryMapper()
{
	if (s_databaseHistoryMapper == 0) {
		s_databaseHistoryMapper = new DatabaseHistoryMapper;
	}
	return s_databaseHistoryMapper;
}

PlaceMapper* MapperFacade::s_placeMapper = 0;
LocationMapper* MapperFacade::s_locationMapper = 0;
LocationPhotoMapper* MapperFacade::s_locationPhotoMapper = 0;
ScenarioDayMapper* MapperFacade::s_scenarioDayMapper = 0;
TimeMapper* MapperFacade::s_timeMapper = 0;
CharacterMapper* MapperFacade::s_characterMapper = 0;
CharacterStateMapper* MapperFacade::s_characterStateMapper = 0;
CharacterPhotoMapper* MapperFacade::s_characterPhotoMapper = 0;
ScenarioMapper* MapperFacade::s_scenarioMapper = 0;
ScenarioChangeMapper* MapperFacade::s_scenarioChangeMapper = 0;
ScenarioDataMapper* MapperFacade::s_scenarioDataMapper = 0;
ResearchMapper* MapperFacade::s_researchMapper = 0;
SettingsMapper* MapperFacade::s_settingsMapper = 0;
DatabaseHistoryMapper* MapperFacade::s_databaseHistoryMapper = 0;
