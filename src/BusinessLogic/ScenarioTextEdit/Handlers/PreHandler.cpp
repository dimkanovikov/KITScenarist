#include "PreHandler.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>

#include <QKeyEvent>
#include <QTextBlock>

using namespace KeyProcessingLayer;


PreHandler::PreHandler(ScenarioTextEdit* _editor) :
	StandardKeyHandler(_editor)
{
}

void PreHandler::handleDelete(QKeyEvent* _event)
{
	if (_event == 0) {
		StandardKeyHandler::handleDelete();
	}
}

void PreHandler::handleOther(QKeyEvent* _event)
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

	//
	// Не все стили можно редактировать
	//
	if (topStyle.isCanModify()
		&& bottomStyle.isCanModify()) {
		//
		// Если имеется выделение, то удалим все выделенные символы
		//
		if (cursor.hasSelection()
			&& !_event->text().isEmpty()) {
			handleDelete();
		}
	}
}
