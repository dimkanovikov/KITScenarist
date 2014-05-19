#include "ConfigurableChronometer.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

using namespace DataStorageLayer;
using namespace BusinessLogic;


ConfigurableChronometer::ConfigurableChronometer()
{
}

QString ConfigurableChronometer::name() const
{
	return "configurable-chronometer";
}

float ConfigurableChronometer::calculateFrom(
		BusinessLogic::ScenarioTextBlockStyle::Type _type, const QString& _text) const
{
	if (_type != ScenarioTextBlockStyle::TimeAndPlace
		&& _type != ScenarioTextBlockStyle::Action
		&& _type != ScenarioTextBlockStyle::Dialog) {
		return 0;
	}

	//
	// Длительность зависит от блока
	//
	float secondsForParagraph = 0;
	float secondsForEvery50 = 0;
	QString secondsForParagraphKey;
	QString secondsForEvery50Key;

	if (_type == ScenarioTextBlockStyle::Action) {
		secondsForParagraphKey = "chronometry/configurable/seconds-for-paragraph/action";
		secondsForEvery50Key = "chronometry/configurable/seconds-for-every-50/action";
	} else if (_type == ScenarioTextBlockStyle::Dialog) {
		secondsForParagraphKey = "chronometry/configurable/seconds-for-paragraph/dialog";
		secondsForEvery50Key = "chronometry/configurable/seconds-for-every-50/dialog";
	} else {
		secondsForParagraphKey = "chronometry/configurable/seconds-for-paragraph/time-and-place";
		secondsForEvery50Key = "chronometry/configurable/seconds-for-every-50/time-and-place";
	}

	//
	// Получим значения длительности
	//
	secondsForParagraph =
			StorageFacade::settingsStorage()->value(
				secondsForParagraphKey,
				SettingsStorage::ApplicationSettings)
			.toFloat();

	secondsForEvery50 =
			StorageFacade::settingsStorage()->value(
				secondsForEvery50Key,
				SettingsStorage::ApplicationSettings)
			.toFloat();

	const int EVERY_50 = 50;
	const float SECONDS_FOR_CHARACTER = secondsForEvery50 / EVERY_50;

	float textChron = secondsForParagraph + _text.length() * SECONDS_FOR_CHARACTER;
	return textChron;
}
