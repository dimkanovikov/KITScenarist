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
		 * @brief Добавить изменение
		 */
		/** @{ */
		ScenarioChange* append(const QString& _id, const QString& _datetime, const QString& _user,
			const QString& _undoPatch, const QString& _redoPatch, bool _isDraft = false);
		ScenarioChange* append(const QString& _user, const QString& _undoPatch,
			const QString& _redoPatch, bool _isDraft = false);
		/** @} */

		/**
		 * @brief Сохранить несохранённые изменения сценарии
		 */
		void store();

		/**
		 * @brief Очистить хранилище
		 */
		void clear();


		/**
		 * @brief Есть ли такое изменение
		 */
		bool contains(const QString& _uuid);

		/**
		 * @brief Получить список uuid'ов всех локальных изменений
		 */
		QList<QString> uuids() const;

		/**
		 * @brief Изменения сценария с заданной даты
		 */
		QList<QString> newUuids(const QString& _fromDatetime);

		/**
		 * @brief Получить изменение по uuid'у не загружая в кучу
		 */
		ScenarioChange change(const QString& _uuid);

	private:
		/**
		 * @brief Список загруженных изменений
		 */
		ScenarioChangesTable* m_all;

		/**
		 * @brief Список изменений к сохранению
		 * @note Используется только внутри данного хранилища, исключительно для оптимизации
		 */
		ScenarioChangesTable* allToSave();
		ScenarioChangesTable* m_allToSave;

		/**
		 * @brief Список изменений не сохранённых в БД
		 */
		QSet<QString> m_uuids;

	private:
		ScenarioChangeStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // SCENARIOCHANGESTORAGE_H
