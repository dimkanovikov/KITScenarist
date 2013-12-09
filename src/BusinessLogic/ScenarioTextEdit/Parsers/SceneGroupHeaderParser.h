#ifndef SCENEGROUPHEADERPARSER_H
#define SCENEGROUPHEADERPARSER_H

class QString;


/**
 * @brief Секции блока заголовка группы сцен
 *
 * @note [ОПИСАНИЕ]: [НАЗВАНИЕ]
 */
enum SceneGroupHeaderSection {
	SceneGroupHeaderSectionUndefined,	 //!< Неопределённое
	SceneGroupHeaderSectionDescription,	 //!< ОПИСАНИЕ
	SceneGroupHeaderSectionName			 //!< НАЗВАНИЕ
};


/**
 * @brief Парсер блока заголовок группы сцен
 */
class SceneGroupHeaderParser
{
public:
	/**
	 * @brief Получить секцию блока
	 */
	static SceneGroupHeaderSection section(const QString& _text);
};

#endif // SCENEGROUPHEADERPARSER_H
