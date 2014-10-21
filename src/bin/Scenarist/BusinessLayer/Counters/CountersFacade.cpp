#include "CountersFacade.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QApplication>
#include <QTextDocument>

using BusinessLogic::CountersFacade;


QString CountersFacade::calculateCounters(QTextDocument* _document)
{
	//
	// Проверить какие счётчики необходимо рассчитать
	//
	bool calculatePages =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"counters/pages/used",
				DataStorageLayer::SettingsStorage::ApplicationSettings).toInt();
	bool calculateWords =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"counters/words/used",
				DataStorageLayer::SettingsStorage::ApplicationSettings).toInt();

	//
	// Рассчитать счётчики
	//
	QString result;
	if (calculatePages) {
		result.append(pagesCounter(_document));
	}

	if (calculateWords) {

		if (!result.isEmpty()) {
			result.append(" ");
		}
		result.append(wordsCounter(_document));
	}

	return result;
}

QString CountersFacade::pagesCounter(QTextDocument* _document)
{
	return QString(" %1: <b>%2</b>")
			.arg(QApplication::translate("BusinessLogic::CountersFacade", "Pages"))
			.arg(_document->pageCount());
}

QString CountersFacade::wordsCounter(QTextDocument* _document)
{
	const int wordsCount =
			_document->toPlainText().simplified().split(" ", QString::SkipEmptyParts).count();
	return QString(" %1: <b>%2</b>")
			.arg(QApplication::translate("BusinessLogic::CountersFacade", "Words"))
			.arg(wordsCount);
}
