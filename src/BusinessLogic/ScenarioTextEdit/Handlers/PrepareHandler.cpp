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
	m_needSendEventToBaseClass = true;
}
