#ifndef STORAGEFACADE_H
#define STORAGEFACADE_H


namespace DataStorageLayer
{
	class PlaceStorage;
	class LocationStorage;
	class LocationPhotoStorage;
	class ScenarioDayStorage;
	class TimeStorage;
	class CharacterStorage;
	class CharacterStateStorage;
	class CharacterPhotoStorage;
	class ScenarioStorage;
	class ScenarioChangeStorage;
	class ScenarioDataStorage;
	class SettingsStorage;
	class DatabaseHistoryStorage;

	class StorageFacade
	{
	public:
		/**
		 * @brief Очистить все хранилища
		 */
		static void clearStorages();

	public:
		static PlaceStorage* placeStorage();
		static LocationStorage* locationStorage();
		static LocationPhotoStorage* locationPhotoStorage();
		static ScenarioDayStorage* scenarioDayStorage();
		static TimeStorage* timeStorage();
		static CharacterStorage* characterStorage();
		static CharacterStateStorage* characterStateStorage();
		static CharacterPhotoStorage* characterPhotoStorage();
		static ScenarioStorage* scenarioStorage();
		static ScenarioChangeStorage* scenarioChangeStorage();
		static ScenarioDataStorage* scenarioDataStorage();
		static SettingsStorage* settingsStorage();
		static DatabaseHistoryStorage* databaseHistoryStorage();

	private:
		static PlaceStorage* s_placeStorage;
		static LocationStorage* s_locationStorage;
		static LocationPhotoStorage* s_locationPhotoStorage;
		static ScenarioDayStorage* s_scenarioDayStorage;
		static TimeStorage* s_timeStorage;
		static CharacterStorage* s_characterStorage;
		static CharacterStateStorage* s_characterStateStorage;
		static CharacterPhotoStorage* s_characterPhotoStorage;
		static ScenarioStorage* s_scenarioStorage;
		static ScenarioChangeStorage* s_scenarioChangeStorage;
		static ScenarioDataStorage* s_scenarioDataStorage;
		static SettingsStorage* s_settingsStorage;
		static DatabaseHistoryStorage* s_databaseHistoryStorage;
	};
}

#endif // STORAGEFACADE_H
