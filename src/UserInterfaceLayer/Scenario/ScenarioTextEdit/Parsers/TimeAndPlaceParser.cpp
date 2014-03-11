#include "TimeAndPlaceParser.h"

#include <QString>
#include <QStringList>


TimeAndPlaceSection TimeAndPlaceParser::section(const QString& _text)
{
	TimeAndPlaceSection section = SceneHeaderSectionUndefined;

	if (_text.split(", ").count() == 2) {
		section = SceneHeaderSectionScenarioDay;
	} else if (_text.split(" - ").count() == 2) {
		section = SceneHeaderSectionTime;
	} else {
		const int splitDotCount = _text.split(". ").count();
		if (splitDotCount == 1) {
			section = SceneHeaderSectionPlace;
		} else {
			section = SceneHeaderSectionLocation;
		}
	}

	return section;
}

QString TimeAndPlaceParser::placeName(const QString& _text)
{
	QString placeName;

	if (_text.split(". ").count() > 0) {
		placeName = _text.split(". ").value(0);
	}

	return placeName;
}

QString TimeAndPlaceParser::locationName(const QString& _text)
{
	QString locationName;

	if (_text.split(". ").count() > 1) {
		locationName = _text.mid(_text.indexOf(". ") + 2);
		locationName = locationName.split(" - ").first();
	}

	return locationName;
}

QString TimeAndPlaceParser::scenarioDayName(const QString& _text)
{
	QString scenarioDayName;

	if (_text.split(", ").count() == 2) {
		scenarioDayName = _text.split(", ").last();
	}

	return scenarioDayName;
}

QString TimeAndPlaceParser::timeName(const QString& _text)
{
	QString timeName;

	if (_text.split(" - ").count() == 2) {
		timeName = _text.split(" - ").last().split(",").first();
	}

	return timeName;
}
