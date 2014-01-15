#include "PagesChronometer.h"

PagesChronometer::PagesChronometer()
{
}

float PagesChronometer::calculateFrom(ScenarioTextBlockStyle::Type _type, const QString& _text) const
{
	//
	// Длительность строки на странице равна 1.2, из расчёта что одна страница - 60 секунд
	//
	const float LINE_CHRON = 1.2;

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
