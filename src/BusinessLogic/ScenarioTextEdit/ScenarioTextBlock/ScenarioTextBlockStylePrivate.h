#ifndef SCENARIOTEXTBLOCKSTYLEPRIVATE_H
#define SCENARIOTEXTBLOCKSTYLEPRIVATE_H

#include "ScenarioTextBlockStyle.h"


/**
 * @brief Данные класса стиля блока текста сценария
 */
class ScenarioTextBlockStylePrivate
{
public:
	ScenarioTextBlockStylePrivate(ScenarioTextBlockStyle::Type _blockType) :
		blockType(_blockType)
	{}

	/**
	 * @brief Вид блока
	 */
	ScenarioTextBlockStyle::Type blockType;

	/**
	 * @brief Настройки стиля отображения блока
	 */
	QTextBlockFormat blockFormat;

	/**
	 * @brief Настройки шрифта блока
	 */
	QTextCharFormat charFormat;
};

#endif // SCENARIOTEXTBLOCKSTYLEPRIVATE_H
