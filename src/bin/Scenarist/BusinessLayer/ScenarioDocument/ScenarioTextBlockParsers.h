#ifndef SCENARIOTEXTBLOCKPARSERS_H
#define SCENARIOTEXTBLOCKPARSERS_H

class QString;
class QStringList;


namespace BusinessLogic
{
	/**
	 * @brief Парсер текста блока персонаж
	 */
	class CharacterParser
	{
	public:
		/**
		 * @brief Секции блока персонаж
		 *
		 * @note [ИМЯ ПЕРСОНАЖА] ([СОСТОЯНИЕ])
		 */
		enum Section {
			SectionUndefined, //!< Неопределённое
			SectionName,	  //!< ИМЯ
			SectionState	  //!< СОСТОЯНИЕ
		};

	public:
		/**
		 * @brief Получить секцию блока
		 */
		static CharacterParser::Section section(const QString& _text);

		/**
		 * @brief Получить имя персонажа
		 */
		static QString name(const QString& _text);

		/**
		 * @brief Получить состояние персонажа
		 */
		static QString state(const QString& _text);
	};


	/**
	 * @brief Парсер текста блока время и место
	 */
	class SceneHeadingParser
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
			SectionLocation,	 //!< ЛОКАЦИЯ
			SectionScenarioDay,	 //!< СЦЕНАРНЫЙ ДЕНЬ
			SectionTime			 //!< ВРЕМЯ
		};

	public:
		/**
		 * @brief Получить секцию блока
		 */
		static SceneHeadingParser::Section section(const QString& _text);

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

	/**
	 * @brief Парсер текста блока участники сцены
	 */
	class SceneCharactersParser
	{
	public:
		/**
		 * @brief Определить список участников
		 */
		static QStringList characters(const QString& _text);
	};
}

#endif // SCENARIOTEXTBLOCKPARSERS_H
