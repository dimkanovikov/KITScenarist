#include "SceneHeaderParser.h"

#include <QString>
#include <QStringList>


SceneHeaderSection SceneHeaderParser::section(const QString& _text)
{
	SceneHeaderSection section = SceneHeaderSectionUndefined;

	if (_text.split(", ").count() == 2) {
		section = SceneHeaderSectionScenarioDay;
	} else if (_text.split(" - ").count() == 2) {
		section = SceneHeaderSectionTime;
	} else {
		const int splitDotCount = _text.split(". ").count();
		if (splitDotCount == 1) {
			section = SceneHeaderSectionPlace;
		} else if (splitDotCount == 2) {
			section = SceneHeaderSectionLocation;
		} else if (splitDotCount == 3) {
			section = SceneHeaderSectionSubLocation;
		}
	}

	return section;
}

QString SceneHeaderParser::placeName(const QString& _text)
{
	QString placeName;

	if (_text.split(". ").count() >= 1) {
		placeName = _text.split(". ").value(0);
	}

	return placeName;
}

QString SceneHeaderParser::locationName(const QString& _text)
{
	QString locationName;

	if (_text.split(". ").count() >= 2) {
		locationName = _text.split(". ").value(1);
	}

	return locationName;
}

QString SceneHeaderParser::sublocationName(const QString& _text)
{
	QString sublocationName;

	if (_text.split(". ").count() >= 3) {
		sublocationName = _text.split(". ").value(2);
	}

	return sublocationName;
}

QString SceneHeaderParser::scenarioDayName(const QString& _text)
{
	QString scenarioDayName;

	if (_text.split(", ").count() == 2) {
		scenarioDayName = _text.split(", ").last();
	}

	return scenarioDayName;
}

QString SceneHeaderParser::timeName(const QString& _text)
{
	QString timeName;

	if (_text.split(" - ").count() == 2) {
		timeName = _text.split(" - ").last().split(",").first();
	}

	return timeName;
}
