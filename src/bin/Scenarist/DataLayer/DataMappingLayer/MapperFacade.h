#ifndef MAPPERFACADE_H
#define MAPPERFACADE_H


namespace DataMappingLayer
{
	class PlaceMapper;
	class LocationMapper;
	class LocationPhotoMapper;
	class ScenarioDayMapper;
	class TimeMapper;
	class CharacterMapper;
	class CharacterStateMapper;
	class CharacterPhotoMapper;
	class ScenarioMapper;
	class SettingsMapper;

	class MapperFacade
	{
	public:
		static PlaceMapper* placeMapper();
		static LocationMapper* locationMapper();
		static LocationPhotoMapper* locationPhotoMapper();
		static ScenarioDayMapper* scenarioDayMapper();
		static TimeMapper* timeMapper();
		static CharacterMapper* characterMapper();
		static CharacterStateMapper* characterStateMapper();
		static CharacterPhotoMapper* characterPhotoMapper();
		static ScenarioMapper* scenarioMapper();
		static SettingsMapper* settingsMapper();

	private:
		static PlaceMapper* s_placeMapper;
		static LocationMapper* s_locationMapper;
		static LocationPhotoMapper* s_locationPhotoMapper;
		static ScenarioDayMapper* s_scenarioDayMapper;
		static TimeMapper* s_timeMapper;
		static CharacterMapper* s_characterMapper;
		static CharacterStateMapper* s_characterStateMapper;
		static CharacterPhotoMapper* s_characterPhotoMapper;
		static ScenarioMapper* s_scenarioMapper;
		static SettingsMapper* s_settingsMapper;
	};
}

#endif // MAPPERFACADE_H
