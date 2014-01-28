#ifndef CHARACTERSCHRONOMETER_H
#define CHARACTERSCHRONOMETER_H

#include "AbstractChronometer.h"


class CharactersChronometer : public AbstractChronometer
{
public:
	CharactersChronometer();

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

#endif // CHARACTERSCHRONOMETER_H
