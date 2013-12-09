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
		blockType(_blockType),
		isFirstUppercase(true),
		isCanModify(true)
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

	/**
	 * @brief Первый символ заглавный
	 */
	bool isFirstUppercase;

	/**
	 * @brief Разрешено изменять текст блока
	 */
	bool isCanModify;

	/**
	 * @brief Префикс
	 */
	QString prefix;

	/**
	 * @brief Постфикс
	 */
	QString postfix;
};

#endif // SCENARIOTEXTBLOCKSTYLEPRIVATE_H
