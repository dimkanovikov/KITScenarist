#ifndef SCENARIOTEXTBLOCKPARSERS_H
#define SCENARIOTEXTBLOCKPARSERS_H

class QString;


namespace BusinessLogic
{
	/**
	 * @brief Парсер текста блока персонаж
	 */
	class CharacterParser
	{
	public:
		/**
		 * @brief Получить имя персонажа
		 */
		static QString name(const QString& _text);
	};


	/**
	 * @brief Парсер текста блока время и место
	 */
	class TimeAndPlaceParser
	{
	public:
		/**
		 * @brief Секции блока заголовка сцены
		 *
		 * @note [МЕСТО]. [ЛОКАЦИЯ] - [ВРЕМЯ], [СЦЕНАРНЫЙ ДЕНЬ]
		 */
		enum Section {
			SectionUndefined,	 //!< Неопределённое
			SectionPlace,		 //!< МЕСТО
			SectionLocation,		 //!< ЛОКАЦИЯ
			SectionScenarioDay,	 //!< СЦЕНАРНЫЙ ДЕНЬ
			SectionTime			 //!< ВРЕМЯ
		};

	public:
		/**
		 * @brief Получить секцию блока
		 */
		static TimeAndPlaceParser::Section section(const QString& _text);

		/**
		 * @brief Получить название места
		 */
		static QString placeName(const QString& _text);

		/**
		 * @brief Получить название локации
		 */
		static QString locationName(const QString& _text);

		/**
		 * @brief Получить название сценарного дня
		 */
		static QString scenarioDayName(const QString& _text);

		/**
		 * @brief Получить название времени
		 */
		static QString timeName(const QString& _text);
	};
}

#endif // SCENARIOTEXTBLOCKPARSERS_H
