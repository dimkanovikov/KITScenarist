#ifndef PAGESCHRONOMETER_H
#define PAGESCHRONOMETER_H

#include "AbstractChronometer.h"


class PagesChronometer : public AbstractChronometer
{
public:
	PagesChronometer();

	/**
	 * @brief Наименование хронометра
	 */
	QString name() const;

	/**
	 * @brief Подсчитать длительность заданного текста определённого типа
	 */
	float calculateFrom(
			BusinessLogic::ScenarioTextBlockStyle::Type _type, const QString &_text) const;

private:
	int linesInText(const QString& _text, int _lineLength) const;
};

#endif // PAGESCHRONOMETER_H
