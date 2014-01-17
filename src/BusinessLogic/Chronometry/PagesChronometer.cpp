#include "PagesChronometer.h"

#include <Storage/StorageFacade.h>
#include <Storage/SettingsStorage.h>

using namespace StorageLayer;


PagesChronometer::PagesChronometer()
{
}

QString PagesChronometer::name() const
{
	return "pages-chronometer";
}

float PagesChronometer::calculateFrom(ScenarioTextBlockStyle::Type _type, const QString& _text) const
{
	//
	// Получим значение длительности одной страницы текста
	//
	static const QString SECONDS_KEY = "chronometry/pages/seconds";
	int seconds = StorageFacade::settingsStorage()->value(SECONDS_KEY).toInt();

	//
	// Если не заданы, применим значения по умолчанию
	//
	if (seconds == 0) {
		seconds = 60;
		StorageFacade::settingsStorage()->setValue(SECONDS_KEY, QString::number(seconds));
	}


	//
	// Высчитываем длительность строки на странице, из расчёта что на одной странице - 50 строк
	//
	const float LINES_IN_PAGE = 50;
	const float LINE_CHRON = seconds / LINES_IN_PAGE;

	//
	// Длина строки в зависимости от типа
	//
	int lineLength = 0;

	//
	// Количество дополнительных строк
	// По-умолчанию равно единице, чтобы учесть отступ перед блоком
	//
	int additionalLines = 1;

	switch (_type) {
		case ScenarioTextBlockStyle::Character: {
			lineLength = 31;
			break;
		}

		case ScenarioTextBlockStyle::Dialog: {
			lineLength = 28;
			break;
		}

		case ScenarioTextBlockStyle::Parenthetical:
		case ScenarioTextBlockStyle::Title: {
			lineLength = 18;
			break;
		}

		case ScenarioTextBlockStyle::FolderHeader:
		case ScenarioTextBlockStyle::FolderFooter: {
			lineLength = 0;
			additionalLines = 0;
			break;
		}

		default: {
			lineLength = 58;
			break;
		}
	}

	//
	// Подсчитаем хронометраж
	//
	float textChron = 0;
	if (lineLength > 0) {
		textChron = (float)(linesInText(_text, lineLength) + additionalLines) * LINE_CHRON;
	}
	return textChron;
}

int PagesChronometer::linesInText(const QString& _text, int _lineLength) const
{
	//
	// Переносы не должны разрывать текст
	//

	return _text.length() / _lineLength + ((_text.length() % _lineLength > 0) ? 1 : 0);
}
