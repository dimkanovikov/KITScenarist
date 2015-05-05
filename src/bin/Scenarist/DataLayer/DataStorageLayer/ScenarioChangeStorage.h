#ifndef SCENARIOCHANGESTORAGE_H
#define SCENARIOCHANGESTORAGE_H


#include "StorageFacade.h"

#include <QSet>

namespace Domain {
	class ScenarioChange;
	class ScenarioChangesTable;
}

using namespace Domain;


namespace DataStorageLayer
{
	class ScenarioChangeStorage
	{
	public:
		/**
		 * @brief Все изменения сценария
		 */
		ScenarioChangesTable* all();

		/**
		 * @brief Изменения сценария с заданной даты
		 */
		QList<ScenarioChange*> allNew(const QString& _fromDatetime);

		/**
		 * @brief Добавить изменение
		 */
		/** @{ */
		ScenarioChange* append(const QString& _id, const QString& _datetime, const QString& _user,
			const QString& _undoPatch, const QString& _redoPatch, bool _isDraft = false);
		ScenarioChange* append(const QString& _user, const QString& _undoPatch,
			const QString& _redoPatch, bool _isDraft = false);
		/** @} */

		/**
		 * @brief Есть ли такое изменение
		 */
		bool contains(const QString& _uuid);

		/**
		 * @brief Сохранить несохранённые сценарии
		 */
		void store();

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

	private:
		ScenarioChangesTable* m_all;

		/**
		 * @brief Список изменений
		 */
		QSet<QString> m_uuids;

	private:
		ScenarioChangeStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // SCENARIOCHANGESTORAGE_H
