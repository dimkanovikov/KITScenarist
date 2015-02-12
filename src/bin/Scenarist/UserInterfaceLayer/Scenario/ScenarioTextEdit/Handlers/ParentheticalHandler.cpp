#include "ParentheticalHandler.h"

#include "../ScenarioTextEdit.h"
#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>

#include <QKeyEvent>
#include <QTextBlock>

using namespace KeyProcessingLayer;
using namespace BusinessLogic;
using UserInterface::ScenarioTextEdit;


ParentheticalHandler::ParentheticalHandler(ScenarioTextEdit* _editor) :
	StandardKeyHandler(_editor)
{
}

void ParentheticalHandler::handleEnter(QKeyEvent*)
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();
	// ... блок текста в котором находится курсор
	QTextBlock currentBlock = cursor.block();
	// ... текст до курсора
	QString cursorBackwardText = currentBlock.text().left(cursor.positionInBlock());
	// ... текст после курсора
	QString cursorForwardText = currentBlock.text().mid(cursor.positionInBlock());
	// ... префикс и постфикс стиля
	ScenarioBlockStyle style = ScenarioStyleFacade::style().blockStyle(ScenarioBlockStyle::Parenthetical);
	QString stylePrefix = style.prefix();
	QString stylePostfix = style.postfix();


	//
	// Обработка
	//
	if (editor()->isCompleterVisible()) {
		//! Если открыт подстановщик

		//
		// Ни чего не делаем
		//
	} else {
		//! Подстановщик закрыт

		if (cursor.hasSelection()) {
			//! Есть выделение

			//
			// Ни чего не делаем
			//
		} else {
			//! Нет выделения

			if ((cursorBackwardText.isEmpty() && cursorForwardText.isEmpty())
				|| (cursorBackwardText + cursorForwardText == stylePrefix + stylePostfix)) {
				//! Текст пуст

				//
				// Ни чего не делаем
				//
				editor()->changeScenarioBlockType(changeForEnter(ScenarioBlockStyle::Parenthetical));
			} else {
				//! Текст не пуст

				if (cursorBackwardText.isEmpty()
					|| cursorBackwardText == stylePrefix) {
					//! В начале блока

					//
					// Ни чего не делаем
					//
				} else if (cursorForwardText.isEmpty()
						   || cursorForwardText == stylePostfix) {
					//! В конце блока

					//
					// Перейдём к блоку реплики
					//
					cursor.movePosition(QTextCursor::EndOfBlock);
					editor()->setTextCursor(cursor);
//					editor()->addScenarioBlock(ScenarioBlockStyle::Dialog);
					editor()->addScenarioBlock(jumpForEnter(ScenarioBlockStyle::Parenthetical));
				} else {
					//! Внутри блока

					//
					// Переместим обрамление в правильное место
					//
					cursor.movePosition(QTextCursor::EndOfBlock);
					if (cursorForwardText.endsWith(stylePostfix)) {
						for (int deleteReplays = stylePostfix.length(); deleteReplays > 0; --deleteReplays) {
							cursor.deletePreviousChar();
						}
					}
					cursor = editor()->textCursor();
					cursor.insertText(stylePostfix);

					//
					// Перейдём к блоку реплики
					//
					editor()->setTextCursor(cursor);
					editor()->addScenarioBlock(ScenarioBlockStyle::Dialog);
				}
			}
		}
	}
}

void ParentheticalHandler::handleTab(QKeyEvent*)
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();
	// ... блок текста в котором находится курсор
	QTextBlock currentBlock = cursor.block();
	// ... текст до курсора
	QString cursorBackwardText = currentBlock.text().left(cursor.positionInBlock());
	// ... текст после курсора
	QString cursorForwardText = currentBlock.text().mid(cursor.positionInBlock());
	// ... префикс и постфикс стиля
	ScenarioBlockStyle style = ScenarioStyleFacade::style().blockStyle(ScenarioBlockStyle::Parenthetical);
	QString stylePrefix = style.prefix();
	QString stylePostfix = style.postfix();


	//
	// Обработка
	//
	if (editor()->isCompleterVisible()) {
		//! Если открыт подстановщик

		//
		// Ни чего не делаем
		//
	} else {
		//! Подстановщик закрыт

		if (cursor.hasSelection()) {
			//! Есть выделение

			//
			// Ни чего не делаем
			//
		} else {
			//! Нет выделения

			if ((cursorBackwardText.isEmpty() && cursorForwardText.isEmpty())
				|| (cursorBackwardText + cursorForwardText == stylePrefix + stylePostfix)) {
				//! Текст пуст

				//
				// Меняем стиль на реплику
				//
//				editor()->changeScenarioBlockType(ScenarioBlockStyle::Dialog);
				editor()->changeScenarioBlockType(changeForTab(ScenarioBlockStyle::Parenthetical));
			} else {
				//! Текст не пуст

				if (cursorBackwardText.isEmpty()
					|| cursorBackwardText == stylePrefix) {
					//! В начале блока

					//
					// Ни чего не делаем
					//
				} else if (cursorForwardText.isEmpty()
						   || cursorForwardText == stylePostfix) {
					//! В конце блока

					//
					// Вставляем блок реплики
					//
					cursor.movePosition(QTextCursor::EndOfBlock);
					editor()->setTextCursor(cursor);
//					editor()->addScenarioBlock(ScenarioBlockStyle::Dialog);
					editor()->addScenarioBlock(jumpForTab(ScenarioBlockStyle::Parenthetical));
				} else {
					//! Внутри блока

					//
					// Ни чего не делаем
					//
				}
			}
		}
	}
}

void ParentheticalHandler::handleOther(QKeyEvent* _event)
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();
	// ... блок текста в котором находится курсор
	QTextBlock currentBlock = cursor.block();
	// ... текст до курсора
	QString cursorBackwardText = currentBlock.text().left(cursor.positionInBlock());
	// ... текст после курсора
	QString cursorForwardText = currentBlock.text().mid(cursor.positionInBlock());

	//
	// Была нажата открывающая скобка
	//
	if (_event != 0
		&& _event->text() == "(") {
		//
		// Если в начале строки и скобки не было
		//
		if (cursorBackwardText == "("
			&& !cursorForwardText.startsWith("(")
			&& !cursorForwardText.contains("(")) {
			//
			// Ни чего не делаем
			//
		}
		//
		// Во всех остальных случаях удаляем введённую скобку
		//
		else {
			cursor.deletePreviousChar();

			//
			// BUG: Если курсор в начале документа, то не прорисовывается текст
			//
		}
	}
	//
	// Была нажата закрывающая скобка
	//
	else if (_event != 0
			 && _event->text() == ")") {
		//
		// Если в конце строки или перед закрывающей скобкой
		//
		if (cursorForwardText.isEmpty()
			|| cursorForwardText == ")") {
			//
			// Если необходимо удаляем лишнюю скобку
			//
			if (cursorBackwardText.endsWith("))")
				|| cursorForwardText == ")") {
				cursor.deletePreviousChar();
				cursor.movePosition(QTextCursor::EndOfBlock);
				editor()->setTextCursor(cursor);
			}

			//
			// Переходим к блоку реплики
			//
			editor()->addScenarioBlock(ScenarioBlockStyle::Dialog);
		}
		//
		// Во всех остальных случаях удаляем введённую скобку
		//
		else {
			cursor.deletePreviousChar();
		}
	}
	//
	// В противном случае, обрабатываем в базовом классе
	//
	else {
		StandardKeyHandler::handleOther(_event);
	}
}
