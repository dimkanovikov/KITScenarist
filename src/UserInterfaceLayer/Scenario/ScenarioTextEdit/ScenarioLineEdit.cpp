#include "ScenarioLineEdit.h"

#include <QApplication>
#include <QKeyEvent>

using UserInterface::ScenarioLineEdit;

namespace {
	const int DOCUMENT_MARGIN = 4;
}


ScenarioLineEdit::ScenarioLineEdit(QWidget* _parent) :
	ScenarioTextEdit(_parent)
{
	//
	// Настраиваем документ
	//
	document()->setDocumentMargin(DOCUMENT_MARGIN);

	//
	// Настраиваем собственный размер
	//
	QFontMetrics metrics(document()->defaultFont());
	int height = metrics.boundingRect("Q").height() + (DOCUMENT_MARGIN * 2);
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

	//
	// Делаем себя похожим на обычный редактор
	//
	setFont(QApplication::font());
	document()->setDefaultFont(QApplication::font());
	clearCharFormat();
}

void ScenarioLineEdit::clearCharFormat()
{
	QTextCursor cursor = textCursor();

	//
	// Установим стиль отображения текста, как в обычном редакторе
	//
	QTextCharFormat format = cursor.charFormat();
	format.setForeground(QApplication::palette().foreground());

	//
	// Обновим стили
	//
	cursor.setBlockCharFormat(format);

	//
	// Применим стиль текста ко всему блоку, выделив его,
	// т.к. в блоке могут находиться фрагменты в другом стиле
	//
	cursor.select(QTextCursor::BlockUnderCursor);
	cursor.setCharFormat(format);
	cursor.clearSelection();
	setTextCursor(cursor);
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
