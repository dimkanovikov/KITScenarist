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
	bool calculateSimbols =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"counters/simbols/used",
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

	if (calculateSimbols) {
		if (!result.isEmpty()) {
			result.append(" ");
		}
		result.append(simbolsCounter(_document));
	}

	return result;
}

QString CountersFacade::pagesCounter(QTextDocument* _document)
{
	QString result;
	if (_document->pageSize().height() > 0) {
		result = QString(" %1: <b>%2</b>")
				 .arg(QApplication::translate("BusinessLogic::CountersFacade", "Pages"))
				 .arg(_document->pageCount());
	}
	return result;
}

QString CountersFacade::wordsCounter(QTextDocument* _document)
{
	const int wordsCount =
			_document->toPlainText().simplified().split(" ", QString::SkipEmptyParts).count();
	return QString(" %1: <b>%2</b>")
			.arg(QApplication::translate("BusinessLogic::CountersFacade", "Words"))
			.arg(wordsCount);
}

QString CountersFacade::simbolsCounter(QTextDocument* _document)
{
	QString documentText = _document->toPlainText().remove("\n").simplified();
	const int simbolsWithSpacesCount = documentText.size();
	documentText = documentText.remove(" ");
	const int simbolsWithoutSpacesCount = documentText.size();
	return QString(" %1: <b>%2 | %3</b>")
			.arg(QApplication::translate("BusinessLogic::CountersFacade", "Simbols"))
			.arg(simbolsWithoutSpacesCount).arg(simbolsWithSpacesCount);
}
