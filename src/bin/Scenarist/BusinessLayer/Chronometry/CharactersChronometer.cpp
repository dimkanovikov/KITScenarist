#include "CharactersChronometer.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

using namespace DataStorageLayer;
using namespace BusinessLogic;


CharactersChronometer::CharactersChronometer()
{
}

QString CharactersChronometer::name() const
{
	return "characters-chronometer";
}

float CharactersChronometer::calculateFrom(BusinessLogic::ScenarioTextBlockStyle::Type _type, const QString& _text) const
{
	//
	// Не включаем в хронометраж непечатный текст, заголовок и окончание папки
	//
	if (_type == ScenarioTextBlockStyle::NoprintableText
		|| _type == ScenarioTextBlockStyle::FolderHeader
		|| _type == ScenarioTextBlockStyle::FolderFooter) {
		return 0;
	}

	static const QString CHARACTERS_KEY = "chronometry/characters/characters";
	static const QString SECONDS_KEY = "chronometry/characters/seconds";

	//
	// Рассчитаем длительность одного символа
	//
	int characters =
			StorageFacade::settingsStorage()->value(
				CHARACTERS_KEY,
				SettingsStorage::ApplicationSettings)
			.toInt();
	int seconds =
			StorageFacade::settingsStorage()->value(
				SECONDS_KEY,
				SettingsStorage::ApplicationSettings)
			.toInt();

	const float CHARACTER_CHRON = (float)seconds / (float)characters;

	//
	// Рассчитаем длительность текста
	//
	QString textForChron = _text;
	float textChron = textForChron.length() * CHARACTER_CHRON;

	return textChron;
}
