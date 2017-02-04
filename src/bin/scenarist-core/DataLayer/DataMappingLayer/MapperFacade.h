#ifndef MAPPERFACADE_H
#define MAPPERFACADE_H


namespace DataMappingLayer
{
    class PlaceMapper;
	class ScenarioDayMapper;
    class TimeMapper;
    class CharacterStateMapper;
	class ScenarioMapper;
	class ScenarioChangeMapper;
	class ScenarioDataMapper;
	class ResearchMapper;
	class SettingsMapper;
	class DatabaseHistoryMapper;

	class MapperFacade
	{
	public:
        static PlaceMapper* placeMapper();
		static ScenarioDayMapper* scenarioDayMapper();
        static TimeMapper* timeMapper();
        static CharacterStateMapper* characterStateMapper();
		static ScenarioMapper* scenarioMapper();
		static ScenarioChangeMapper* scenarioChangeMapper();
		static ScenarioDataMapper* scenarioDataMapper();
		static ResearchMapper* researchMapper();
		static SettingsMapper* settingsMapper();
		static DatabaseHistoryMapper* databaseHistoryMapper();

	private:
        static PlaceMapper* s_placeMapper;
		static ScenarioDayMapper* s_scenarioDayMapper;
        static TimeMapper* s_timeMapper;
        static CharacterStateMapper* s_characterStateMapper;
		static ScenarioMapper* s_scenarioMapper;
		static ScenarioChangeMapper* s_scenarioChangeMapper;
		static ScenarioDataMapper* s_scenarioDataMapper;
		static ResearchMapper* s_researchMapper;
		static SettingsMapper* s_settingsMapper;
		static DatabaseHistoryMapper* s_databaseHistoryMapper;
	};
}

#endif // MAPPERFACADE_H
