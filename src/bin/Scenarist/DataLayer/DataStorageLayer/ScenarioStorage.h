#ifndef SCENARIOSTORAGE_H
#define SCENARIOSTORAGE_H

#include "StorageFacade.h"

#include <QString>

class QDateTime;

namespace Domain {
	class Scenario;
	class ScenariosTable;
}

using namespace Domain;


namespace DataStorageLayer
{
	class ScenarioStorage
	{
	public:
		/**
		 * @brief Получить все версии сценария
		 */
		ScenariosTable* all();

		/**
		 * @brief Получить текущую версию сценария
		 */
		Scenario* current(bool _isDraft = false);

		/**
		 * @brief Получить последнюю синхронизированную версию сценария
		 */
		Scenario* lastSynced(bool _isDraft = false);

		/**
		 * @brief Получить сценарий с заданным uuid'ом
		 */
		Scenario* findByUuid(const QString& _uuid, bool _isDraft = false);

		/**
		 * @brief Сохранить текст сценария
		 * @param Текст сценария в формате xml
		 * @note Используется при сохранении сценария по запросу пользователя из приложения
		 *		 Создаёт новые версии по истечении времени сессии работы с версией
		 */
		Scenario* storeScenario(const QString& _name, const QString& _synopsis,
			const QString& _text, bool _isDraft = false, const QString& _comment = QString::null);

		/**
		 * @brief Сохранить актуализированный текст текущей версии сценария
		 * @note Используется при синхронизации последней версии сценария, после её объединения
		 *		 с наработками соавторов
		 *		 Создаёт новые версии по истечении времени сессии работы с версией
		 */
		Scenario* storeActualScenario(const QString& _name, const QString& _additionalInfo,
			const QString& _genre, const QString& _author, const QString& _contacts,
			const QString& _year, const QString& _synopsis, const QString& _text,
			const bool _isDraft, const QDateTime& _versionStartDatetime,
			const QDateTime& _versionEndDatetime, const QString& _versionComment,
			const QString& _uuid);

		/**
		 * @brief Простое сохранение/обновление сценария с заданными параметрами
		 */
		Scenario* storeOldScenario(const QString& _name, const QString& _additionalInfo,
			const QString& _genre, const QString& _author, const QString& _contacts,
			const QString& _year, const QString& _synopsis, const QString& _text,
			const bool _isDraft, const QDateTime& _versionStartDatetime,
			const QDateTime& _versionEndDatetime, const QString& _versionComment,
			const QString& _uuid, const bool _isSynced);

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

	private:
		ScenariosTable* m_all;

	private:
		ScenarioStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // SCENARIOSTORAGE_H
