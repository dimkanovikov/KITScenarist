#ifndef SCENARIOXML_H
#define SCENARIOXML_H

#include <QString>

class ScenarioTextEdit;


/**
 * @brief Класс для записи сценария в xml-строку
 */
class ScenarioXmlWriter
{
public:
	/**
	 * @brief Записать сценарий в xml-строку из заданного диапозона текста
	 * @param Редактор сценария
	 * @param Позиция начала текста для записи
	 * @param Позиция конца текста для записи
	 *
	 * @note При начальной и конечной позициях равных нулю, в xml пишется весь сценарий
	 */
	static QString scenarioToXml(const ScenarioTextEdit* _editor, int _startPosition = 0, int _endPosition = 0);
};

/**
 * @brief Класс для чтения сценария из xml-строки
 */
class ScenarioXmlReader
{
public:
	/**
	 * @brief Ввести сценарий из xml в редактор в месте, где установлен курсор
	 */
	static void xmlToScenario(const QString& _xml, ScenarioTextEdit* _editor);
};

#endif // SCENARIOXML_H
