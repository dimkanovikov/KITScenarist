#include "CountersFacade.h"

#include "Counter.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QApplication>
#include <QTextCursor>
#include <QTextDocument>

using BusinessLogic::CountersFacade;
using BusinessLogic::Counter;


Counter CountersFacade::calculate(QTextDocument* _document, int _fromCursorPosition, int _toCursorPosition)
{
	//
	// Проверить какие счётчики необходимо рассчитать
	//
	bool calculateWords =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"counters/words/used",
				DataStorageLayer::SettingsStorage::ApplicationSettings).toInt();
	bool calculateCharacters =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"counters/simbols/used",
				DataStorageLayer::SettingsStorage::ApplicationSettings).toInt();

	Counter counter;
	//
	// Рассчитываем, если необходимо
	//
	if (calculateWords || calculateCharacters) {
		//
		// Определим текст, который необходимо обсчитать
		//
		QTextCursor cursor(_document);
		cursor.setPosition(_fromCursorPosition);
		cursor.setPosition(_toCursorPosition, QTextCursor::KeepAnchor);
		QString text = cursor.selectedText();

		if (calculateWords) {
			counter.setWords(wordsCount(text.simplified()));
		}

		if (calculateCharacters) {
			//
			// Когда текст получаем из QTextCursor'а, то строки в нём разделяются не \n,
			// а UTF-символом разрыва параграфов
			//
			static const QChar paragraphSeparator(8233);
			text = text.remove(paragraphSeparator);
			counter.setCharactersWithSpaces(charactersWithSpacesCount(text));
			text = text.simplified();
			counter.setCharactersWithoutSpaces(charactersWithoutSpacesCount(text));
		}
	}

	return counter;
}

QString CountersFacade::countersInfo(int pageCount, const BusinessLogic::Counter& _counter)
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
	bool calculateCharacters =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"counters/simbols/used",
				DataStorageLayer::SettingsStorage::ApplicationSettings).toInt();
	//
	// Рассчитать счётчики
	//
	QString result;
	if (calculatePages) {
		result.append(pageInfo(pageCount));
	}

	if (calculateWords) {
		if (!result.isEmpty()) {
			result.append(" ");
		}
		result.append(wordsInfo(_counter.words()));
	}

	if (calculateCharacters) {
		if (!result.isEmpty()) {
			result.append(" ");
		}
		result.append(charactersInfo(_counter.charactersWithSpaces(), _counter.charactersWithoutSpaces()));
	}

	return result;
}

int CountersFacade::wordsCount(const QString& _text)
{
	return _text.split(" ", QString::SkipEmptyParts).count();
}

int CountersFacade::charactersWithSpacesCount(const QString& _text)
{
	return _text.size();
}

int CountersFacade::charactersWithoutSpacesCount(const QString& _text)
{
	return QString(_text).replace(" ", "").size();
}

QString CountersFacade::pageInfo(int _count)
{
	return QString(" %1: <b>%2</b>")
			.arg(QApplication::translate("BusinessLogic::CountersFacade", "Pages"))
			.arg(_count);
}

QString CountersFacade::wordsInfo(int _count)
{
	return QString(" %1: <b>%2</b>")
			.arg(QApplication::translate("BusinessLogic::CountersFacade", "Words"))
			.arg(_count);
}

QString CountersFacade::charactersInfo(int _countWithSpaces, int _countWithoutSpaces)
{
	return QString(" %1: <b>%2 | %3</b>")
			.arg(QApplication::translate("BusinessLogic::CountersFacade", "Simbols"))
			.arg(_countWithoutSpaces).arg(_countWithSpaces);
}
