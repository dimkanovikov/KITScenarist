#ifndef CONFIGURABLECHRONOMETER_H
#define CONFIGURABLECHRONOMETER_H

#include "AbstractChronometer.h"


class ConfigurableChronometer : public AbstractChronometer
{
public:
	ConfigurableChronometer();

	/**
	 * @brief Наименование хронометра
	 */
	QString name() const;

	/**
	 * @brief Подсчитать длительность заданного текста определённого типа
	 */
	float calculateFrom(
			BusinessLogic::ScenarioTextBlockStyle::Type _type, const QString &_text) const;
};

#endif // CONFIGURABLECHRONOMETER_H
