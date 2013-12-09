#include "MapperFacade.h"

#include "PlaceMapper.h"
#include "LocationMapper.h"
#include "ScenarioDayMapper.h"
#include "TimeMapper.h"
#include "CharacterMapper.h"

using namespace DataMappingLayer;


PlaceMapper* MapperFacade::placeMapper()
{
	if (s_placeMapper == 0) {
		s_placeMapper = new PlaceMapper;
	}
	return s_placeMapper;
}

LocationMapper*MapperFacade::locationMapper()
{
	if (s_locationMapper == 0) {
		s_locationMapper = new LocationMapper;
	}
	return s_locationMapper;
}

ScenarioDayMapper*MapperFacade::scenarioDayMapper()
{
	if (s_scenarioDayMapper == 0) {
		s_scenarioDayMapper = new ScenarioDayMapper;
	}
	return s_scenarioDayMapper;
}

TimeMapper*MapperFacade::timeMapper()
{
	if (s_timeMapper == 0) {
		s_timeMapper = new TimeMapper;
	}
	return s_timeMapper;
}

CharacterMapper*MapperFacade::characterMapper()
{
	if (s_characterMapper == 0) {
		s_characterMapper = new CharacterMapper;
	}
	return s_characterMapper;
}

PlaceMapper* MapperFacade::s_placeMapper = 0;
LocationMapper* MapperFacade::s_locationMapper = 0;
ScenarioDayMapper* MapperFacade::s_scenarioDayMapper = 0;
TimeMapper* MapperFacade::s_timeMapper = 0;
CharacterMapper* MapperFacade::s_characterMapper = 0;
