#include "SceneGroupHeaderHandler.h"

#include "../ScenarioTextEdit.h"
#include "../ScenarioTextEditHelpers.h"

#include <QKeyEvent>
#include <QTextBlock>

using namespace KeyProcessingLayer;
using namespace BusinessLogic;
using UserInterface::ScenarioTextEdit;


SceneGroupHeaderHandler::SceneGroupHeaderHandler(ScenarioTextEdit* _editor) :
	StandardKeyHandler(_editor)
{
}

void SceneGroupHeaderHandler::handleEnter(QKeyEvent*)
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

			if (cursorBackwardText.isEmpty()
				&& cursorForwardText.isEmpty()) {
				//! Текст пуст

				//
				// Ни чего не делаем
				//
				editor()->changeScenarioBlockType(changeForEnter(ScenarioBlockStyle::SceneGroupHeader));
			} else {
				//! Текст не пуст

				if (cursorBackwardText.isEmpty()) {
					//! В начале блока

					//
					// Вставить блок время и место перед группой
					//
					cursor.insertBlock();
					cursor.movePosition(QTextCursor::Left);
					cursor.setBlockFormat(QTextBlockFormat());
					editor()->setTextCursor(cursor);
					editor()->changeScenarioBlockType(ScenarioBlockStyle::TimeAndPlace);
					editor()->moveCursor(QTextCursor::Right);
				} else if (cursorForwardText.isEmpty()) {
					//! В конце блока

					//
					// Вставить блок время и место
					//
					editor()->addScenarioBlock(jumpForEnter(ScenarioBlockStyle::SceneGroupHeader));
				} else {
					//! Внутри блока

					//
					// Вставить блок время и место
					//
					editor()->addScenarioBlock(ScenarioBlockStyle::TimeAndPlace);
				}
			}
		}
	}
}

void SceneGroupHeaderHandler::handleTab(QKeyEvent*)
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

			if (cursorBackwardText.isEmpty()
				&& cursorForwardText.isEmpty()) {
				//! Текст пуст

				//
				// Ни чего не делаем
				//
				editor()->changeScenarioBlockType(changeForTab(ScenarioBlockStyle::SceneGroupHeader));
			} else {
				//! Текст не пуст

				if (cursorBackwardText.isEmpty()) {
					//! В начале блока

					//
					// Ни чего не делаем
					//
				} else if (cursorForwardText.isEmpty()) {
					//! В конце блока

					//
					// Как ENTER
					//
					editor()->addScenarioBlock(jumpForTab(ScenarioBlockStyle::SceneGroupHeader));
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

void SceneGroupHeaderHandler::handleOther(QKeyEvent* _event)
{
	//
	// Если не было введено текста, прерываем операцию
	// _event->key() == -1 // событие посланное редактором текста, его необходимо обработать
	//
	if (_event == 0
		|| (_event->key() != -1 && _event->text().isEmpty())) {
		return;
	}

	//
	// Найти закрывающий блок и обновить его текст
	//

	QTextCursor cursor = editor()->textCursor();

	//
	// Если редактируется заголовок группы
	//
	if (editor()->scenarioBlockType() == ScenarioBlockStyle::SceneGroupHeader) {

		cursor.movePosition(QTextCursor::NextBlock);

		// ... открытые группы на пути поиска необходимого для обновления блока
		int openedGroups = 0;
		bool isFooterUpdated = false;
		do {
			ScenarioBlockStyle::Type currentType =
					ScenarioBlockStyle::forBlock(cursor.block());

			if (currentType == ScenarioBlockStyle::SceneGroupFooter) {
				if (openedGroups == 0) {
					cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
					cursor.insertText(Helpers::footerText(editor()->textCursor().block().text()));
					isFooterUpdated = true;
				} else {
					--openedGroups;
				}
			} else if (currentType == ScenarioBlockStyle::SceneGroupHeader) {
				// ... встретилась новая группа
				++openedGroups;
			}

			cursor.movePosition(QTextCursor::EndOfBlock);
			cursor.movePosition(QTextCursor::NextBlock);
		} while (!isFooterUpdated && !cursor.atEnd());
	}
}
