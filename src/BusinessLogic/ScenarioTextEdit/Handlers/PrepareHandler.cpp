#include "PrepareHandler.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>
#include <BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStyle.h>

#include <QTextCharFormat>

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

void PrepareHandler::handleShortcut(QKeyEvent*)
{
	m_needSendEventToBaseClass = false;
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

void PrepareHandler::handleOther(QKeyEvent*)
{
	//
	// Не все стили можно редактировать
	//
	QTextCharFormat currentCharFormat = editor()->textCursor().blockCharFormat();
	bool isCanModify = currentCharFormat.boolProperty(ScenarioTextBlockStyle::PropertyIsCanModify);

	m_needSendEventToBaseClass = isCanModify;
}
