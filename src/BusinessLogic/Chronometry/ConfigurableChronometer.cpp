#include "ConfigurableChronometer.h"

#include <Storage/StorageFacade.h>
#include <Storage/SettingsStorage.h>

using namespace StorageLayer;


ConfigurableChronometer::ConfigurableChronometer()
{
}

QString ConfigurableChronometer::name() const
{
	return "configurable-chronometer";
}

float ConfigurableChronometer::calculateFrom(ScenarioTextBlockStyle::Type _type, const QString& _text) const
{
	if (_type != ScenarioTextBlockStyle::TimeAndPlace
		&& _type != ScenarioTextBlockStyle::Action
		&& _type != ScenarioTextBlockStyle::Dialog) {
		return 0;
	}

	//
	// Длительность зависит от блока
	//
	float secondsForParagraph = 0, defaultSecondsForParagraph = 0;
	float secondsForEvery50 = 0, defaultSecondsForEvery50 = 0;
	QString secondsForParagraphKey;
	QString secondsForEvery50Key;

	if (_type == ScenarioTextBlockStyle::Action) {
		secondsForParagraphKey = "chronometry/configurable/seconds-for-paragraph/action";
		secondsForEvery50Key = "chronometry/configurable/seconds-for-every-50/action";
		defaultSecondsForParagraph = 1;
		defaultSecondsForEvery50 = 1.5;
	} else if (_type == ScenarioTextBlockStyle::Dialog) {
		secondsForParagraphKey = "chronometry/configurable/seconds-for-paragraph/dialog";
		secondsForEvery50Key = "chronometry/configurable/seconds-for-every-50/dialog";
		defaultSecondsForParagraph = 2;
		defaultSecondsForEvery50 = 2.4;
	} else {
		secondsForParagraphKey = "chronometry/configurable/seconds-for-paragraph/time-and-place";
		secondsForEvery50Key = "chronometry/configurable/seconds-for-every-50/time-and-place";
		defaultSecondsForParagraph = 2;
		defaultSecondsForEvery50 = 0;
	}

	//
	// Получим значения длительности
	// Если они не заданы, применим значения по умолчанию
	//
	if (StorageFacade::settingsStorage()->value(secondsForParagraphKey).isNull()) {
		secondsForParagraph = defaultSecondsForParagraph;
		StorageFacade::settingsStorage()->setValue(secondsForParagraphKey, QString::number(secondsForParagraph));
	} else {
		secondsForParagraph = StorageFacade::settingsStorage()->value(secondsForParagraphKey).toFloat();
	}

	if (StorageFacade::settingsStorage()->value(secondsForEvery50Key).isNull()) {
		secondsForEvery50 = defaultSecondsForEvery50;
		StorageFacade::settingsStorage()->setValue(secondsForEvery50Key, QString::number(secondsForEvery50));
	} else {
		secondsForEvery50 = StorageFacade::settingsStorage()->value(secondsForEvery50Key).toFloat();
	}

	const int EVERY_50 = 50;
	const float SECONDS_FOR_CHARACTER = secondsForEvery50 / EVERY_50;

	float textChron = secondsForParagraph + _text.length() * SECONDS_FOR_CHARACTER;
	return textChron;
}
