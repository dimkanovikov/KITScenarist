#include "PagesChronometer.h"

PagesChronometer::PagesChronometer()
{
}

int PagesChronometer::calculateFrom(ScenarioTextBlockStyle::Type _type, const QString& _text) const
{
	return _text.length();
}
