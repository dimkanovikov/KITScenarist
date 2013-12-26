#ifndef MAPPERFACADE_H
#define MAPPERFACADE_H


namespace DataMappingLayer
{
	class PlaceMapper;
	class LocationMapper;
	class ScenarioDayMapper;
	class TimeMapper;
	class CharacterMapper;
	class ScenarioMapper;

	class MapperFacade
	{
	public:
		static PlaceMapper* placeMapper();
		static LocationMapper* locationMapper();
		static ScenarioDayMapper* scenarioDayMapper();
		static TimeMapper* timeMapper();
		static CharacterMapper* characterMapper();
		static ScenarioMapper* scenarioMapper();

	private:
		static PlaceMapper* s_placeMapper;
		static LocationMapper* s_locationMapper;
		static ScenarioDayMapper* s_scenarioDayMapper;
		static TimeMapper* s_timeMapper;
		static CharacterMapper* s_characterMapper;
		static ScenarioMapper* s_scenarioMapper;
	};
}

#endif // MAPPERFACADE_H
