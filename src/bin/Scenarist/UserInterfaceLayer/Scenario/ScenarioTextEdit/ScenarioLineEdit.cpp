#include "ScenarioLineEdit.h"

#include <QApplication>
#include <QKeyEvent>
#include <QMimeData>
#include <QTextFrame>
#include <QTextFrameFormat>

using UserInterface::ScenarioLineEdit;

namespace {
	const int DOCUMENT_PADDING = 4;
}


ScenarioLineEdit::ScenarioLineEdit(QWidget* _parent) :
	ScenarioTextEdit(_parent),
	m_inClearCharFormat(false)
{
	//
	// Настраиваем документ
	//
	QTextFrameFormat format = document()->rootFrame()->frameFormat();
	format.setPadding(DOCUMENT_PADDING);
	document()->rootFrame()->setFrameFormat(format);

	//
	// Настраиваем собственный размер
	//
	QFontMetricsF metrics(document()->defaultFont());
	int height = metrics.boundingRect("Q").height() + (DOCUMENT_PADDING * 2);
	setFixedHeight(height);

	//
	// Прячем полосы прокрутки
	//
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	//
	// Запрещаем перенос слов
	//
	setWordWrapMode(QTextOption::NoWrap);
	connect(document(), SIGNAL(contentsChanged()), this, SLOT(removeLineBreaks()));

	//
	// Запрещаем сохранение данных во время редактирования
	//
	setStoreDataWhenEditing(false);

	setAddSpaceToBottom(false);
}

void ScenarioLineEdit::keyPressEvent(QKeyEvent* _event)
{
	Qt::Key eventKey = (Qt::Key)_event->key();
	if (eventKey == Qt::Key_Enter
		|| eventKey == Qt::Key_Return
		|| eventKey == Qt::Key_Escape) {
		_event->ignore();
		//
		// Ни чего не делаем, за нас это сделает родительский виджет
		//
	} else {
		ScenarioTextEdit::keyPressEvent(_event);
	}
}

void ScenarioLineEdit::insertFromMimeData(const QMimeData *_source)
{
	//
	// Вставляем только в текстовом виде
	//
	if (_source->hasText()) {
		QString textToInsert = _source->text().simplified();
		insertPlainText(textToInsert);
	}
}

void ScenarioLineEdit::removeLineBreaks()
{
	QTextCursor cursor(document());
	while (!cursor.atEnd()) {
		cursor.movePosition(QTextCursor::EndOfBlock);
		if (!cursor.atEnd()) {
			cursor.deleteChar();
		}
	}
}
