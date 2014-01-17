#include "KeyPressHandlerFacade.h"

#include "PrepareHandler.h"
#include "PreHandler.h"
#include "TimeAndPlaceHandler.h"
#include "ActionHandler.h"
#include "CharacterHandler.h"
#include "ParentheticalHandler.h"
#include "DialogHandler.h"
#include "TransitionHandler.h"
#include "NoteHandler.h"
#include "TitleHeaderHandler.h"
#include "TitleHandler.h"
#include "SimpleTextHandler.h"
#include "SceneGroupHeaderHandler.h"
#include "SceneGroupFooterHandler.h"
#include "FolderHeaderHandler.h"
#include "FolderFooterHandler.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>

#include <QTextBlock>
#include <QKeyEvent>

using namespace KeyProcessingLayer;


void KeyPressHandlerFacade::prepare(QKeyEvent* _event)
{
	m_prepareHandler->handle(_event);
}

void KeyPressHandlerFacade::prehandle(QKeyEvent* _event)
{
	m_preHandler->handle(_event);
}

void KeyPressHandlerFacade::handle(QKeyEvent* _event)
{
	QTextBlock currentBlock = m_editor->textCursor().block();
	ScenarioTextBlockStyle::Type currentType = m_editor->scenarioBlockType(currentBlock);
	AbstractKeyHandler* currentHandler = handlerFor(currentType);

	//
	// Если удалось получить обработчик выполним предназначенные ему действия
	//
	if (currentHandler != 0) {
		currentHandler->handle(_event);
	}
}

bool KeyPressHandlerFacade::needSendEventToBaseClass() const
{
	return m_prepareHandler->needSendEventToBaseClass();
}

bool KeyPressHandlerFacade::needEnshureCursorVisible() const
{
	return m_prepareHandler->needEnsureCursorVisible();
}

bool KeyPressHandlerFacade::structureChanged() const
{
	return m_prepareHandler->structureChanged();
}

// ******** private ********

KeyPressHandlerFacade::KeyPressHandlerFacade(ScenarioTextEdit* _editor) :
	m_editor(_editor)
{
	m_prepareHandler = new PrepareHandler(_editor);
	m_preHandler = new PreHandler(_editor);
	m_sceneHeaderHandler = new TimeAndPlaceHandler(_editor);
	m_actionHandler = new ActionHandler(_editor);
	m_characterHandler = new CharacterHandler(_editor);
	m_parentheticalHandler = new ParentheticalHandler(_editor);
	m_dialogHandler = new DialogHandler(_editor);
	m_transitionHandler = new TransitionHandler(_editor);
	m_noteHandler = new NoteHandler(_editor);
	m_titleheaderHandler = new TitleHeaderHandler(_editor);
	m_titleHandler = new TitleHandler(_editor);
	m_simpleTextHandler = new SimpleTextHandler(_editor);
	m_sceneGroupHeaderHandler = new SceneGroupHeaderHandler(_editor);
	m_sceneGroupFooterHandler = new SceneGroupFooterHandler(_editor);
	m_folderHeaderHandler = new FolderHeaderHandler(_editor);
	m_folderFooterHandler = new FolderFooterHandler(_editor);
}

AbstractKeyHandler* KeyPressHandlerFacade::handlerFor(ScenarioTextBlockStyle::Type _type)
{
	AbstractKeyHandler* handler = 0;

	switch (_type) {
		case ScenarioTextBlockStyle::TimeAndPlace: {
			handler = m_sceneHeaderHandler;
			break;
		}

		case ScenarioTextBlockStyle::Action: {
			handler = m_actionHandler;
			break;
		}

		case ScenarioTextBlockStyle::Character: {
			handler = m_characterHandler;
			break;
		}
		case ScenarioTextBlockStyle::Parenthetical: {
			handler = m_parentheticalHandler;
			break;
		}

		case ScenarioTextBlockStyle::Dialog: {
			handler = m_dialogHandler;
			break;
		}

		case ScenarioTextBlockStyle::Transition: {
			handler = m_transitionHandler;
			break;
		}

		case ScenarioTextBlockStyle::Note: {
			handler = m_noteHandler;
			break;
		}

		case ScenarioTextBlockStyle::TitleHeader: {
			handler = m_titleheaderHandler;
			break;
		}

		case ScenarioTextBlockStyle::Title: {
			handler = m_titleHandler;
			break;
		}

		case ScenarioTextBlockStyle::SimpleText: {
			handler = m_simpleTextHandler;
			break;
		}

		case ScenarioTextBlockStyle::SceneGroupHeader: {
			handler = m_sceneGroupHeaderHandler;
			break;
		}

		case ScenarioTextBlockStyle::SceneGroupFooter: {
			handler = m_sceneGroupFooterHandler;
			break;
		}

		case ScenarioTextBlockStyle::FolderHeader: {
			handler = m_folderHeaderHandler;
			break;
		}

		case ScenarioTextBlockStyle::FolderFooter: {
			handler = m_folderFooterHandler;
			break;
		}

		default: {
			handler = 0;
			break;
		}
	}

	return handler;
}

// ********
// Реализация одиночки

KeyPressHandlerFacade* KeyPressHandlerFacade::instance(ScenarioTextEdit* _editor)
{
	//
	// Если необходимо:
	// ... инициилизируем обработчик
	// ... устанавливаем обработчику текстовый редактор
	//
	if (s_instance == 0) {
		s_instance = new KeyPressHandlerFacade(_editor);
	}

	if (s_instance->m_editor != _editor) {
		Q_ASSERT_X(0, Q_FUNC_INFO, "Can't reinitialize KeyPressHandlerFacade");
	}

	return s_instance;
}

KeyPressHandlerFacade* KeyPressHandlerFacade::s_instance = 0;

// ********
