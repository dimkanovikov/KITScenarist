#include "StandardKeyHandler.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>

#include <QKeyEvent>
#include <QTextBlock>

using namespace KeyProcessingLayer;

StandardKeyHandler::StandardKeyHandler(ScenarioTextEdit* _editor) :
	AbstractKeyHandler(_editor)
{
}

void StandardKeyHandler::handleShortcut(QKeyEvent*)
{
}

void StandardKeyHandler::handleDelete()
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();

	//
	// Стираем символы и применяем соответствующий стиль
	//
	int destBlockPosition = cursor.position();
	if (cursor.hasSelection()) {
		destBlockPosition = qMin(cursor.selectionStart(), cursor.selectionEnd());
	}
	QTextCursor destBlockCursor(editor()->document());
	destBlockCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, destBlockPosition);
	ScenarioTextBlockStyle::Type destType = editor()->scenarioBlockType(destBlockCursor.block());

	cursor.deleteChar();

	editor()->setScenarioBlockType(destType);

	//
	// Покажем подсказку, если это возможно
	//
	handleOther();
}

void StandardKeyHandler::handleBackspace()
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();

	//
	// Стираем символы и применяем соответствующий стиль
	//
	int destBlockPosition = cursor.position() - 1; // отнимаем 1 т.к. удаляём предыдущий текст
	if (cursor.hasSelection()) {
		destBlockPosition = qMin(cursor.selectionStart(), cursor.selectionEnd());
	}
	QTextCursor destBlockCursor(editor()->document());
	destBlockCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, destBlockPosition);
	ScenarioTextBlockStyle::Type destType = editor()->scenarioBlockType(destBlockCursor.block());

	cursor.deletePreviousChar();

	editor()->setScenarioBlockType(destType);

	//
	// Покажем подсказку, если это возможно
	//
	handleOther();
}

void StandardKeyHandler::handleEscape()
{
	editor()->closeCompleter();
}

void StandardKeyHandler::handleUp(QKeyEvent* _event)
{
	//
	// Если подстановщик скрыт - имитируем действие обычного редактора
	//
	if (!editor()->isCompleterVisible()) {
		bool isShiftPressed = _event->modifiers().testFlag(Qt::ShiftModifier);
		editor()->moveCursor(
					QTextCursor::Up,
					isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
	}
}

void StandardKeyHandler::handleDown(QKeyEvent* _event)
{
	//
	// Если подстановщик скрыт - имитируем действие обычного редактора
	//
	if (!editor()->isCompleterVisible()) {
		bool isShiftPressed = _event->modifiers().testFlag(Qt::ShiftModifier);
		editor()->moveCursor(
					QTextCursor::Down,
					isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
	}
}

void StandardKeyHandler::handleOther()
{

}
