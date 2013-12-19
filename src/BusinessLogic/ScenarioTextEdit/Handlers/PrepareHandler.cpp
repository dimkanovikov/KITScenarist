#include "PrepareHandler.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>
#include <BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStyle.h>

#include <QTextBlock>
#include <QKeyEvent>

using namespace KeyProcessingLayer;


PrepareHandler::PrepareHandler(ScenarioTextEdit* _editor) :
	AbstractKeyHandler(_editor),
	m_needSendEventToBaseClass(true)
{
}

bool PrepareHandler::needSendEventToBaseClass() const
{
	return m_needSendEventToBaseClass;
}

void PrepareHandler::handleShortcut(QKeyEvent* _event)
{
	//
	// Получим необходимые переменные
	//
	// ... нажатая клавиша
	Qt::Key pressedKey = (Qt::Key)_event->key();
	// ... зажатые управляющие клавиши
	Qt::KeyboardModifiers pressedModifiers = _event->modifiers();

	if (pressedModifiers.testFlag(Qt::ControlModifier)
		&& (pressedKey == Qt::Key_Left || pressedKey == Qt::Key_Right)) {
		m_needSendEventToBaseClass = true;
	} else {
		m_needSendEventToBaseClass = false;
	}
}

void PrepareHandler::handleEnter(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;
}

void PrepareHandler::handleTab(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;
}

void PrepareHandler::handleDelete(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;
}

void PrepareHandler::handleBackspace(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;
}

void PrepareHandler::handleEscape(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;
}

void PrepareHandler::handleUp(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;
}

void PrepareHandler::handleDown(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;
}

void PrepareHandler::handleOther(QKeyEvent* _event)
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();

	//
	// Получим стиль первого блока в выделении
	//
	QTextCursor topCursor(editor()->document());
	topCursor.movePosition(QTextCursor::Right,
						   QTextCursor::MoveAnchor,
						   qMin(cursor.selectionStart(), cursor.selectionEnd()));
	ScenarioTextBlockStyle topStyle(editor()->scenarioBlockType(topCursor.block()));

	//
	// Получим стиль последнего блока в выделении
	//
	QTextCursor bottomCursor(editor()->document());
	bottomCursor.movePosition(QTextCursor::Right,
							  QTextCursor::MoveAnchor,
							  qMax(cursor.selectionStart(), cursor.selectionEnd()));
	ScenarioTextBlockStyle bottomStyle(editor()->scenarioBlockType(bottomCursor.block()));

	if (!_event->text().isEmpty()) {
		//
		// Не все стили можно редактировать
		//
		m_needSendEventToBaseClass = topStyle.isCanModify() && bottomStyle.isCanModify() ;
	} else {
		m_needSendEventToBaseClass = true;
	}
}
