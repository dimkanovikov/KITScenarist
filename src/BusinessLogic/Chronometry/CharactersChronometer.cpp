#include "CharactersChronometer.h"

CharactersChronometer::CharactersChronometer()
{
}

QString CharactersChronometer::name() const
{
	return "characters-chronometer";
}

float CharactersChronometer::calculateFrom(ScenarioTextBlockStyle::Type _type, const QString& _text) const
{
	return 0;
}
