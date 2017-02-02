#ifndef RESEARCHSTORAGE_H
#define RESEARCHSTORAGE_H

#include "StorageFacade.h"

#include <QMap>

namespace Domain {
	class Research;
	class ResearchTable;
}

using namespace Domain;


namespace DataStorageLayer
{
	class ResearchStorage
	{
	public:
        // ****
        // API для работы с разработкой в целом

		/**
		 * @brief Получить все разработки
		 */
		ResearchTable* all();

		/**
		 * @brief Получить разработку по названию
		 */
		Research* research(const QString& _name);

		/**
		 * @brief Сохранить разработку
		 */
		Research* storeResearch(Research* _parent, int _researchType, int _sortOrder, const QString& _researchName);

		/**
		 * @brief Обновить разработку
		 */
		void updateResearch(Research* _research);

		/**
		 * @brief Удалить разработку
		 */
		void removeResearch(Research* _research);

		/**
		 * @brief Проверить наличие заданной разработки
		 */
		bool hasResearch(Research* _research);

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

		/**
		 * @brief Обновить хранилище
		 */
		void refresh();

        // ****
        // API для работы с персонажами

        /**
         * @brief Все персонажи сценария
         */
        ResearchTable* characters();

        /**
         * @brief Получить персонажа по названию
         */
        Research* character(const QString& _name);

        /**
         * @brief Сохранить персонажа
         */
        Research* storeCharacter(const QString& _name);

        /**
         * @brief Обновить персонажа
         */
        void updateCharacter(Research* _character);

        /**
         * @brief Удалить персонажа
         */
        void removeCharacter(const QString& _name);
        void removeCharacters(const QStringList& _names);

        /**
         * @brief Проверить наличие заданного персонажа
         */
        bool hasCharacter(const QString& _name);

	private:
        /**
         * @brief Список со всеми элементами разработки
         */
        ResearchTable* m_all = nullptr;

        /**
         * @brief Список персонажей
         */
        ResearchTable* m_characters = nullptr;

        /**
         * @brief Список локаций
         */
        ResearchTable* m_locations = nullptr;

	private:
		ResearchStorage();

		// Для доступа к конструктору
        friend class StorageFacade;
    };
}

#endif // RESEARCHSTORAGE_H
