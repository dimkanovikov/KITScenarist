#ifndef STORAGEFACADE_H
#define STORAGEFACADE_H


namespace StorageLayer
{
	class PlaceStorage;
	class LocationStorage;
	class ScenarioDayStorage;
	class TimeStorage;
	class CharacterStorage;

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
		static ScenarioDayStorage* scenarioDayStorage();
		static TimeStorage* timeStorage();
		static CharacterStorage* characterStorage();

	private:
		static PlaceStorage* s_placeStorage;
		static LocationStorage* s_locationStorage;
		static ScenarioDayStorage* s_scenarioDayStorage;
		static TimeStorage* s_timeStorage;
		static CharacterStorage* s_characterStorage;
	};
}

#endif // STORAGEFACADE_H
