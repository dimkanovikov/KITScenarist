#include "MapperFacade.h"

#include "PlaceMapper.h"
#include "ScenarioDayMapper.h"
#include "TimeMapper.h"
#include "CharacterStateMapper.h"
#include "ScenarioMapper.h"
#include "ScenarioChangeMapper.h"
#include "ScenarioDataMapper.h"
#include "ResearchMapper.h"
#include "SettingsMapper.h"
#include "DatabaseHistoryMapper.h"

using namespace DataMappingLayer;


PlaceMapper* MapperFacade::placeMapper()
{
    if (s_placeMapper == nullptr) {
		s_placeMapper = new PlaceMapper;
	}
	return s_placeMapper;
}

ScenarioDayMapper* MapperFacade::scenarioDayMapper()
{
    if (s_scenarioDayMapper == nullptr) {
		s_scenarioDayMapper = new ScenarioDayMapper;
	}
	return s_scenarioDayMapper;
}

TimeMapper* MapperFacade::timeMapper()
{
    if (s_timeMapper == nullptr) {
		s_timeMapper = new TimeMapper;
	}
	return s_timeMapper;
}

CharacterStateMapper* MapperFacade::characterStateMapper()
{
    if (s_characterStateMapper == nullptr) {
		s_characterStateMapper = new CharacterStateMapper;
	}
	return s_characterStateMapper;
}

ScenarioMapper* MapperFacade::scenarioMapper()
{
    if (s_scenarioMapper == nullptr) {
		s_scenarioMapper = new ScenarioMapper;
	}
	return s_scenarioMapper;
}

ScenarioChangeMapper* MapperFacade::scenarioChangeMapper()
{
    if (s_scenarioChangeMapper == nullptr) {
		s_scenarioChangeMapper = new ScenarioChangeMapper;
	}
	return s_scenarioChangeMapper;
}

ScenarioDataMapper* MapperFacade::scenarioDataMapper()
{
    if (s_scenarioDataMapper == nullptr) {
		s_scenarioDataMapper = new ScenarioDataMapper;
	}
	return s_scenarioDataMapper;
}

ResearchMapper* MapperFacade::researchMapper()
{
    if (s_researchMapper == nullptr) {
		s_researchMapper = new ResearchMapper;
	}
	return s_researchMapper;
}

SettingsMapper* MapperFacade::settingsMapper()
{
    if (s_settingsMapper == nullptr) {
		s_settingsMapper = new SettingsMapper;
	}
	return s_settingsMapper;
}

DatabaseHistoryMapper* MapperFacade::databaseHistoryMapper()
{
    if (s_databaseHistoryMapper == nullptr) {
		s_databaseHistoryMapper = new DatabaseHistoryMapper;
	}
	return s_databaseHistoryMapper;
}

PlaceMapper* MapperFacade::s_placeMapper = nullptr;
ScenarioDayMapper* MapperFacade::s_scenarioDayMapper = nullptr;
TimeMapper* MapperFacade::s_timeMapper = nullptr;
CharacterStateMapper* MapperFacade::s_characterStateMapper = nullptr;
ScenarioMapper* MapperFacade::s_scenarioMapper = nullptr;
ScenarioChangeMapper* MapperFacade::s_scenarioChangeMapper = nullptr;
ScenarioDataMapper* MapperFacade::s_scenarioDataMapper = nullptr;
ResearchMapper* MapperFacade::s_researchMapper = nullptr;
SettingsMapper* MapperFacade::s_settingsMapper = nullptr;
DatabaseHistoryMapper* MapperFacade::s_databaseHistoryMapper = nullptr;
