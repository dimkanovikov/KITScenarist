#ifndef ABSTRACTCHRONOMETER_H
#define ABSTRACTCHRONOMETER_H

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStyle.h>

class QString;


/**
 * @brief Базовый класс рассчёта хронометража
 */
class AbstractChronometer
{
public:
	virtual ~AbstractChronometer() {}

	/**
	 * @brief Подсчитать длительность заданного текста определённого типа
	 */
	virtual float calculateFrom(ScenarioTextBlockStyle::Type _type, const QString& _text) const = 0;
};

#endif // ABSTRACTCHRONOMETER_H
