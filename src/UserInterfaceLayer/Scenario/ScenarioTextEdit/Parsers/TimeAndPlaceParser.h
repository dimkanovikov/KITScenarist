#ifndef SCENEHEADERPARSER_H
#define SCENEHEADERPARSER_H

class QString;


/**
 * @brief Секции блока заголовка сцены
 *
 * @note [МЕСТО]. [ЛОКАЦИЯ]. [ВНУТРЕННЯЯ ЛОКАЦИЯ] - [ВРЕМЯ], [СЦЕНАРНЫЙ ДЕНЬ]
 */
enum TimeAndPlaceSection {
	SceneHeaderSectionUndefined,	 //!< Неопределённое
	SceneHeaderSectionPlace,		 //!< МЕСТО
	SceneHeaderSectionLocation,		 //!< ЛОКАЦИЯ
	SceneHeaderSectionScenarioDay,	 //!< СЦЕНАРНЫЙ ДЕНЬ
	SceneHeaderSectionTime			 //!< ВРЕМЯ
};


/**
 * @brief Парсер текста блока время и место
 */
class TimeAndPlaceParser
{
public:
	/**
	 * @brief Получить секцию блока
	 */
	static TimeAndPlaceSection section(const QString& _text);

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

#endif // SCENEHEADERPARSER_H
