#include "PrepareHandler.h"

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

void PrepareHandler::handleEnter()
{
	m_needSendEventToBaseClass = false;
}

void PrepareHandler::handleTab()
{
	m_needSendEventToBaseClass = false;
}

void PrepareHandler::handleDelete()
{
	m_needSendEventToBaseClass = false;
}

void PrepareHandler::handleBackspace()
{
	m_needSendEventToBaseClass = false;
}

void PrepareHandler::handleEscape()
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

void PrepareHandler::handleOther()
{
	m_needSendEventToBaseClass = true;
}
