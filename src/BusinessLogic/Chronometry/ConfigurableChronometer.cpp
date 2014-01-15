#include "ConfigurableChronometer.h"


ConfigurableChronometer::ConfigurableChronometer()
{
}

QString ConfigurableChronometer::name() const
{
	return "configurable-chronometer";
}

float ConfigurableChronometer::calculateFrom(ScenarioTextBlockStyle::Type _type, const QString& _text) const
{
	return 0;
}
