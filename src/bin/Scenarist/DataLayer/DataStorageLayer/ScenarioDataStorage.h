#ifndef SCENARIODATASTORAGE_H
#define SCENARIODATASTORAGE_H

#include "StorageFacade.h"

class QString;

namespace Domain {
	class ScenarioData;
	class ScenarioDataTable;
}

using namespace Domain;


namespace DataStorageLayer
{
	/**
	 * @brief Хранилище данных сценария
	 */
	class ScenarioDataStorage
	{
	public:
		/**
		 * @brief Очистить хранилище
		 */
		void clear();

		/**
		 * @brief Название сценария
		 */
		/** @{ */
		QString name() const;
		void setName(const QString& _name);
		/** @} */

		/**
		 * @brief Дополнительная информация
		 */
		/** @{ */
		QString additionalInfo() const;
		void setAdditionalInfo(const QString& _additionalInfo);
		/** @} */

		/**
		 * @brief Жанр
		 */
		/** @{ */
		QString genre() const;
		void setGenre(const QString& _genre);
		/** @} */

		/**
		 * @brief Автор
		 */
		/** @{ */
		QString author() const;
		void setAuthor(const QString _author);
		/** @} */

		/**
		 * @brief Контактная информация
		 */
		/** @{ */
		QString contacts() const;
		void setContacts(const QString& _contacts);
		/** @} */

		/**
		 * @brief Год
		 */
		/** @{ */
		QString year() const;
		void setYear(const QString& _year);
		/** @} */

		/**
		 * @brief Синопсис сценария
		 */
		/** @{ */
		QString synopsis() const;
		void setSynopsis(const QString& _synopsis);
		/** @} */

	private:
		/**
		 * @brief Все данные
		 */
		ScenarioDataTable* all() const;

		/**
		 * @brief Получить данные по заданному ключу
		 * @note Если таких данных ещё нет - создать
		 */
		ScenarioData* data(const QString& _name) const;

		/**
		 * @brief Сохранить данные
		 */
		void saveData(const QString& _name, const QString& _newValue);

	private:
		/**
		 * @brief Все данные
		 */
		mutable ScenarioDataTable* m_all;

	private:
		ScenarioDataStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // SCENARIODATASTORAGE_H
