#include "ScenarioLineEdit.h"

#include <QApplication>
#include <QKeyEvent>
#include <QMimeData>

using UserInterface::ScenarioLineEdit;

namespace {
	const int DOCUMENT_MARGIN = 4;
}


ScenarioLineEdit::ScenarioLineEdit(QWidget* _parent) :
	ScenarioTextEdit(_parent),
	m_inClearCharFormat(false)
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
	connect(document(), SIGNAL(contentsChanged()), this, SLOT(clearCharFormat()));
	clearCharFormat();
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

void ScenarioLineEdit::clearCharFormat()
{
	if (!m_inClearCharFormat) {
		m_inClearCharFormat = true;

		QTextCursor cursor = textCursor();

		//
		// Установим стиль отображения текста, как в обычном редакторе
		//
		QTextCharFormat charFormat = cursor.charFormat();
		charFormat.setForeground(QApplication::palette().foreground());

		//
		// Обновим стили
		//
		cursor.setBlockCharFormat(charFormat);

		//
		// Применим стиль текста ко всему блоку, выделив его,
		// т.к. в блоке могут находиться фрагменты в другом стиле
		//
		cursor.select(QTextCursor::BlockUnderCursor);
		cursor.setCharFormat(charFormat);
		cursor.clearSelection();
		setTextCursor(cursor);

		m_inClearCharFormat = false;
	}
}
