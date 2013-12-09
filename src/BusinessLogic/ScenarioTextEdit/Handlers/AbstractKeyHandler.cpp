#include "AbstractKeyHandler.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>
#include <QKeyEvent>

using namespace KeyProcessingLayer;


AbstractKeyHandler::AbstractKeyHandler(ScenarioTextEdit* _editor) :
	m_editor(_editor)
{
	Q_ASSERT(_editor);
}

AbstractKeyHandler::~AbstractKeyHandler()
{

}

void AbstractKeyHandler::handle(QKeyEvent* _event)
{
	//
	// Получим необходимые переменные
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();
	// ... нажатая кнопка
	Qt::Key pressedKey = (Qt::Key)_event->key();
	// ... зажатые управляющие клавиши
	Qt::KeyboardModifiers pressedModifiers = _event->modifiers();

	//
	// Начнём блок операций
	//
	cursor.beginEditBlock();

	//
	// Нажата клавиша "Control" в сочетании с какой-либо другой
	//
	if (pressedModifiers.testFlag(Qt::ControlModifier)
		&& pressedKey != Qt::Key_Control) {
		handleShortcut(_event);
	}

	//
	// Нажата клавиша "Enter"
	//
	else if (pressedKey == Qt::Key_Enter
			 || pressedKey == Qt::Key_Return) {
		handleEnter();
	}

	//
	// Нажата клавиша "Tab"
	//
	else if (pressedKey == Qt::Key_Tab) {
		handleTab();
	}

	//
	// Нажата кнопка "Delete"
	//
	else if (pressedKey == Qt::Key_Delete) {
		handleDelete();
	}

	//
	// Нажата кнопка "Backspace"
	//
	else if (pressedKey == Qt::Key_Backspace) {
		handleBackspace();
	}

	//
	// Нажата кнопка "Escape"
	//
	else if (pressedKey == Qt::Key_Escape) {
		handleEscape();
	}

	//
	// Нажата кнопка стрелочки "Вниз"
	//
	else if (pressedKey == Qt::Key_Down) {
		handleDown(_event);
	}

	//
	// Нажата кнопка стрелочки "Вверх"
	//
	else if (pressedKey == Qt::Key_Up) {
		handleUp(_event);
	}

	//
	// Нажата любая другая клавиша
	//
	else {
		handleOther();
	}

	//
	// Завершим блок операций
	//
	cursor.endEditBlock();
}

ScenarioTextEdit* AbstractKeyHandler::editor() const
{
	return m_editor;
}
