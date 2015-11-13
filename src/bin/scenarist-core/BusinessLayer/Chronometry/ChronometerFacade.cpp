#include "ChronometerFacade.h"

#include "AbstractChronometer.h"
#include "PagesChronometer.h"
#include "CharactersChronometer.h"
#include "ConfigurableChronometer.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QTime>

using namespace DataStorageLayer;
using namespace BusinessLogic;


bool ChronometerFacade::chronometryUsed()
{
	return StorageFacade::settingsStorage()->value(
				"chronometry/used",
				SettingsStorage::ApplicationSettings).toInt();
}

qreal ChronometerFacade::calculate(const QTextBlock& _block)
{
	return calculate(_block, _block);
}

qreal ChronometerFacade::calculate(const QTextBlock& _fromBlock, const QTextBlock& _toBlock)
{
	return calculate(
				const_cast<QTextDocument*>(_fromBlock.document()),
				_fromBlock.position(),
				_toBlock.position() + _toBlock.length() - 1);
}

qreal ChronometerFacade::calculate(QTextDocument* _document, int _fromCursorPosition, int _toCursorPosition)
{
	qreal chronometry = -1;

	if (chronometryUsed()) {
		chronometry = 0;

		if (!_document->isEmpty()) {
			QTextCursor cursor(_document);
			cursor.setPosition(_fromCursorPosition);
			bool isFirstStep = true;
			do {
				//
				// Перейти к следующему, если это не первый шаг цикла
				//
				if (!isFirstStep) {
					cursor.movePosition(QTextCursor::NextBlock);
				} else {
					isFirstStep = false;
				}
				cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
				if (cursor.position() > _toCursorPosition) {
					cursor.setPosition(_toCursorPosition, QTextCursor::KeepAnchor);
				}

				//
				// Получить текст одного блока
				//
				if (cursor.atBlockEnd()
					|| cursor.position() == _toCursorPosition) {
					//
					// Посчитать его хронометраж, добавив к результату
					//
					chronometry +=
							chronometer()->calculateFrom(
								ScenarioBlockStyle::forBlock(cursor.block()),
								cursor.selectedText()
								);
					cursor.clearSelection();
				}
			} while (!cursor.atEnd()
					 && cursor.position() < _toCursorPosition);
		}
	}

	return chronometry;
}

qreal ChronometerFacade::calculate(QTextDocument* _document)
{
	return calculate(_document, 0, _document->characterCount());
}

QString ChronometerFacade::secondsToTime(int _seconds)
{
	QString timeString = "0:00";
	if (_seconds != 0) {
		QTime time(0, 0, 0);
		time = time.addSecs(_seconds);
		timeString =
				QString("%1:%2")
				.arg(time.hour()*60 + time.minute())
				.arg(time.toString("ss"));
	}
	return timeString;
}

QString ChronometerFacade::secondsToTime(qreal _seconds)
{
	return secondsToTime(qRound(_seconds));
}

AbstractChronometer* ChronometerFacade::chronometer()
{
	static QString CHRONOMETRY_TYPE_KEY = "chronometry/current-chronometer-type";
	static QString CHRONOMETRY_PAGES = PagesChronometer().name();
	static QString CHRONOMETRY_CHARACTERS = CharactersChronometer().name();
	static QString CHRONOMETRY_CONFIGURABLE = ConfigurableChronometer().name();

	//
	// Определить какой хронометр нужно использовать
	// Если не задан, настроить на хронометр для страниц
	//
	QString chronometryType =
			StorageFacade::settingsStorage()->value(
				CHRONOMETRY_TYPE_KEY,
				SettingsStorage::ApplicationSettings);
	if (chronometryType.isEmpty()) {
		chronometryType = CHRONOMETRY_PAGES;
		StorageFacade::settingsStorage()->setValue(
					CHRONOMETRY_TYPE_KEY,
					chronometryType,
					SettingsStorage::ApplicationSettings);
	}

	//
	// Проверить какой используется
	// Если нужно создать необходимый
	//
	if (chronometryType == CHRONOMETRY_PAGES) {
		if (s_chronometer == 0
			|| s_chronometer->name() != CHRONOMETRY_PAGES) {
			delete s_chronometer;
			s_chronometer = new PagesChronometer;
		}
	} else if (chronometryType == CHRONOMETRY_CHARACTERS) {
		if (s_chronometer == 0
			|| s_chronometer->name() != CHRONOMETRY_CHARACTERS) {
			delete s_chronometer;
			s_chronometer = new CharactersChronometer;
		}
	} else {
		if (s_chronometer == 0
			|| s_chronometer->name() != CHRONOMETRY_CONFIGURABLE) {
			delete s_chronometer;
			s_chronometer = new ConfigurableChronometer;
		}
	}

	return s_chronometer;
}

AbstractChronometer* ChronometerFacade::s_chronometer = 0;
