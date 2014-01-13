#include "ChronometerFacade.h"

#include "AbstractChronometer.h"
#include "PagesChronometer.h"

#include <QTextCursor>
#include <QTextBlock>
#include <QString>


int ChronometerFacade::calculate(const QTextBlock& _fromBlock, const QTextBlock& _toBlock)
{
	return calculate(
				const_cast<QTextDocument*>(_fromBlock.document()),
				_fromBlock.position(),
				_toBlock.position() + _toBlock.length() - 1);
}

int ChronometerFacade::calculate(QTextDocument* _document, int _fromCursorPosition, int _toCursorPosition)
{
	int chronometry = 0;

	QTextCursor cursor(_document);
	cursor.setPosition(_fromCursorPosition);
	bool isFirstStep = true;
	do {
		//
		// Перейти к следующему, если это не первый шаг цикла
		//
		if (!isFirstStep) {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} else {
			isFirstStep = false;
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
						ScenarioTextBlockStyle::forBlock(cursor.block()),
						cursor.selectedText()
						);
			cursor.clearSelection();
		}
	} while (!cursor.atEnd()
			 && cursor.position() <= _toCursorPosition);

	return chronometry;
}

AbstractChronometer* ChronometerFacade::chronometer()
{
	//
	// Определить какой хронометр нужно использовать
	// Проверить какой используется
	// Если нужно создать необходимый
	//

	if (s_chronometer == 0) {
		s_chronometer = new PagesChronometer;
	}
	return s_chronometer;
}

AbstractChronometer* ChronometerFacade::s_chronometer = 0;
