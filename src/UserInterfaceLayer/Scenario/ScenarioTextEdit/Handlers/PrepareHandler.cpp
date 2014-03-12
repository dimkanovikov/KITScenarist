#include "PrepareHandler.h"

#include "../ScenarioTextEdit.h"
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h>

#include <QTextBlock>
#include <QKeyEvent>

using namespace KeyProcessingLayer;
using namespace BusinessLogic;
using UserInterface::ScenarioTextEdit;


PrepareHandler::PrepareHandler(ScenarioTextEdit* _editor) :
	AbstractKeyHandler(_editor),
	m_needSendEventToBaseClass(true),
	m_needEnsureCursorVisible(true)
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

void PrepareHandler::prepareForHandle()
{
	m_needSendEventToBaseClass = true;
	m_needEnsureCursorVisible = true;
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
		&& (pressedKey == Qt::Key_Left		// смещение
			|| pressedKey == Qt::Key_Right	// --//--
			|| pressedKey == Qt::Key_S)		// сохранить
		) {
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

	//
	// Получим стиль последнего блока в выделении
	//
	QTextCursor bottomCursor(editor()->document());
	bottomCursor.setPosition(qMax(cursor.selectionStart(), cursor.selectionEnd()));
}

void PrepareHandler::handleBackspace(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;

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

	//
	// Получим стиль последнего блока в выделении
	//
	QTextCursor bottomCursor(editor()->document());
	bottomCursor.setPosition(qMax(cursor.selectionStart(), cursor.selectionEnd()));
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
	ScenarioTextBlockStyle topStyle(ScenarioTextBlockStyle::forBlock(topCursor.block()));

	//
	// Получим стиль последнего блока в выделении
	//
	QTextCursor bottomCursor(editor()->document());
	bottomCursor.setPosition(qMax(cursor.selectionStart(), cursor.selectionEnd()));
	ScenarioTextBlockStyle bottomStyle(ScenarioTextBlockStyle::forBlock(bottomCursor.block()));

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
}
