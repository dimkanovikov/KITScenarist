#include "PrepareHandler.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>
#include <BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStyle.h>

#include <QTextBlock>
#include <QKeyEvent>

using namespace KeyProcessingLayer;


PrepareHandler::PrepareHandler(ScenarioTextEdit* _editor) :
	AbstractKeyHandler(_editor),
	m_needSendEventToBaseClass(true),
	m_needEnsureCursorVisible(true),
	m_structureChanged(false)
{
}

bool PrepareHandler::needSendEventToBaseClass() const
{
	return m_needSendEventToBaseClass;
}

bool PrepareHandler::needEnsureCursorVisible() const
{
	return m_needEnsureCursorVisible;
}

bool PrepareHandler::structureChanged() const
{
	return m_structureChanged;
}

void PrepareHandler::prepareForHandle()
{
	m_needSendEventToBaseClass = true;
	m_needEnsureCursorVisible = true;
	m_structureChanged = false;
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

	if (pressedModifiers.testFlag(Qt::ControlModifier)
		&& (_event->nativeScanCode() == 52	   // z
			|| _event->nativeScanCode() == 53  // x
			|| _event->nativeScanCode() == 55) // v
		) {
		m_structureChanged = true;
	}
}

void PrepareHandler::handleEnter(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;

	if (!editor()->isCompleterVisible()){
		m_structureChanged = true;
	}
}

void PrepareHandler::handleTab(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;

	if (!editor()->isCompleterVisible()){
		m_structureChanged = true;
	}
}

void PrepareHandler::handleDelete(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;
	m_structureChanged = true;
}

void PrepareHandler::handleBackspace(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;
	m_structureChanged = true;
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
	topCursor.setPosition(qMin(cursor.selectionStart(), cursor.selectionEnd()));
	ScenarioTextBlockStyle topStyle(editor()->scenarioBlockType(topCursor.block()));

	//
	// Получим стиль последнего блока в выделении
	//
	QTextCursor bottomCursor(editor()->document());
	bottomCursor.setPosition(qMax(cursor.selectionStart(), cursor.selectionEnd()));
	ScenarioTextBlockStyle bottomStyle(editor()->scenarioBlockType(bottomCursor.block()));

	if (!_event->text().isEmpty()) {
		//
		// Не все стили можно редактировать
		//
		m_needSendEventToBaseClass = topStyle.isCanModify() && bottomStyle.isCanModify() ;
	} else {
		m_needSendEventToBaseClass = true;
	}

	//
	// Если нажат шифт, то не нужно прокручивать окно редактора к тому месту, где установлен курсор
	//
	if (_event->key() == Qt::Key_Shift) {
		m_needEnsureCursorVisible = false;
	}

	//
	// Если действие выполняется над несколькими блоками
	//
	if (topCursor.blockNumber() != bottomCursor.blockNumber()) {
		m_structureChanged = true;
	}
}
