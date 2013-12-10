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
	// Если имеется выделение, то удалим все выделенные символы
	//
	if (cursor.hasSelection()
		&& !_event->text().isEmpty()) {
		handleDelete();
	}
}
