#ifndef SCENEHEADERPARSER_H
#define SCENEHEADERPARSER_H

class QString;


/**
 * @brief Секции блока заголовка сцены
 *
 * @note [МЕСТО]. [ЛОКАЦИЯ]. [ВНУТРЕННЯЯ ЛОКАЦИЯ] - [ВРЕМЯ], [СЦЕНАРНЫЙ ДЕНЬ]
 */
enum SceneHeaderSection {
	SceneHeaderSectionUndefined,	 //!< Неопределённое
	SceneHeaderSectionPlace,		 //!< МЕСТО
	SceneHeaderSectionLocation,		 //!< ЛОКАЦИЯ
	SceneHeaderSectionSubLocation,	 //!< ВНУТРЕННЯЯ ЛОКАЦИЯ
	SceneHeaderSectionScenarioDay,	 //!< СЦЕНАРНЫЙ ДЕНЬ
	SceneHeaderSectionTime			 //!< ВРЕМЯ
};


/**
 * @brief Парсер текста блока время и место
 */
class SceneHeaderParser
{
public:
	/**
	 * @brief Получить секцию блока
	 */
	static SceneHeaderSection section(const QString& _text);

	/**
	 * @brief Получить название места
	 */
	static QString placeName(const QString& _text);

	/**
	 * @brief Получить название локации
	 */
	static QString locationName(const QString& _text);

	/**
	 * @brief Получить название подлокации
	 */
	static QString sublocationName(const QString& _text);

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
