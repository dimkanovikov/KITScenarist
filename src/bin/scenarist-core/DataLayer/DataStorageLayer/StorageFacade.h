#ifndef STORAGEFACADE_H
#define STORAGEFACADE_H

#include <QString>


namespace DataStorageLayer
{
    class PlaceStorage;
    class ScenarioDayStorage;
    class TimeStorage;
    class CharacterStateStorage;
    class ScenarioStorage;
    class ScenarioChangeStorage;
    class ScenarioDataStorage;
    class ResearchStorage;
    class SettingsStorage;
    class DatabaseHistoryStorage;

    class StorageFacade
    {
    public:
        /**
         * @brief Получить имя пользователя
         */
        static QString username();

        /**
         * @brief Очистить все хранилища
         */
        static void clearStorages();

        /**
         * @brief Обновить данные хранилищ
         * @note Используется при получении новых данных с облачного сервиса
         */
        static void refreshStorages();

    public:
        static PlaceStorage* placeStorage();
        static ScenarioDayStorage* scenarioDayStorage();
        static TimeStorage* timeStorage();
        static CharacterStateStorage* characterStateStorage();
        static ScenarioStorage* scenarioStorage();
        static ScenarioChangeStorage* scenarioChangeStorage();
        static ScenarioDataStorage* scenarioDataStorage();
        static ResearchStorage* researchStorage();
        static SettingsStorage* settingsStorage();
        static DatabaseHistoryStorage* databaseHistoryStorage();

    private:
        static PlaceStorage* s_placeStorage;
        static ScenarioDayStorage* s_scenarioDayStorage;
        static TimeStorage* s_timeStorage;
        static CharacterStateStorage* s_characterStateStorage;
        static ScenarioStorage* s_scenarioStorage;
        static ScenarioChangeStorage* s_scenarioChangeStorage;
        static ScenarioDataStorage* s_scenarioDataStorage;
        static ResearchStorage* s_researchStorage;
        static SettingsStorage* s_settingsStorage;
        static DatabaseHistoryStorage* s_databaseHistoryStorage;
    };
}

#endif // STORAGEFACADE_H
