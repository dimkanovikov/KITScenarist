#ifndef PAGESCHRONOMETER_H
#define PAGESCHRONOMETER_H

#include "AbstractChronometer.h"


class PagesChronometer : public AbstractChronometer
{
public:
	PagesChronometer();

	int calculateFrom(ScenarioTextBlockStyle::Type _type, const QString &_text) const;
};

#endif // PAGESCHRONOMETER_H
