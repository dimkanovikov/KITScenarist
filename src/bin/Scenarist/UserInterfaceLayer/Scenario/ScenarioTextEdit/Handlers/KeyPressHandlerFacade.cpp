#include "KeyPressHandlerFacade.h"

#include "PrepareHandler.h"
#include "PreHandler.h"
#include "TimeAndPlaceHandler.h"
#include "SceneCharactersHandler.h"
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

#include "../ScenarioTextEdit.h"

#include <QTextBlock>
#include <QKeyEvent>

using namespace KeyProcessingLayer;
using namespace BusinessLogic;
using UserInterface::ScenarioTextEdit;


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
	ScenarioBlockStyle::Type currentType = ScenarioBlockStyle::forBlock(currentBlock);
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

bool KeyPressHandlerFacade::needEnsureCursorVisible() const
{
	return m_prepareHandler->needEnsureCursorVisible();
}

// ******** private ********

KeyPressHandlerFacade::KeyPressHandlerFacade(ScenarioTextEdit* _editor) :
	m_editor(_editor)
{
	m_prepareHandler = new PrepareHandler(_editor);
	m_preHandler = new PreHandler(_editor);
	m_sceneHeaderHandler = new TimeAndPlaceHandler(_editor);
	m_sceneCharactersHandler = new SceneCharactersHandler(_editor);
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

AbstractKeyHandler* KeyPressHandlerFacade::handlerFor(ScenarioBlockStyle::Type _type)
{
	AbstractKeyHandler* handler = 0;

	switch (_type) {
		case ScenarioBlockStyle::TimeAndPlace: {
			handler = m_sceneHeaderHandler;
			break;
		}

		case ScenarioBlockStyle::SceneCharacters: {
			handler = m_sceneCharactersHandler;
			break;
		}

		case ScenarioBlockStyle::Action: {
			handler = m_actionHandler;
			break;
		}

		case ScenarioBlockStyle::Character: {
			handler = m_characterHandler;
			break;
		}
		case ScenarioBlockStyle::Parenthetical: {
			handler = m_parentheticalHandler;
			break;
		}

		case ScenarioBlockStyle::Dialog: {
			handler = m_dialogHandler;
			break;
		}

		case ScenarioBlockStyle::Transition: {
			handler = m_transitionHandler;
			break;
		}

		case ScenarioBlockStyle::Note: {
			handler = m_noteHandler;
			break;
		}

		case ScenarioBlockStyle::TitleHeader: {
			handler = m_titleheaderHandler;
			break;
		}

		case ScenarioBlockStyle::Title: {
			handler = m_titleHandler;
			break;
		}

		case ScenarioBlockStyle::NoprintableText: {
			handler = m_simpleTextHandler;
			break;
		}

		case ScenarioBlockStyle::SceneGroupHeader: {
			handler = m_sceneGroupHeaderHandler;
			break;
		}

		case ScenarioBlockStyle::SceneGroupFooter: {
			handler = m_sceneGroupFooterHandler;
			break;
		}

		case ScenarioBlockStyle::FolderHeader: {
			handler = m_folderHeaderHandler;
			break;
		}

		case ScenarioBlockStyle::FolderFooter: {
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
	if (s_instance.value(_editor, 0) == 0) {
		s_instance.insert(_editor, new KeyPressHandlerFacade(_editor));
	}

	return s_instance.value(_editor);
}

QMap<UserInterface::ScenarioTextEdit*, KeyPressHandlerFacade*> KeyPressHandlerFacade::s_instance;

// ********
