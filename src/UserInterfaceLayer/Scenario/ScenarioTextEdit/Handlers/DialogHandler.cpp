#include "DialogHandler.h"

#include "../ScenarioTextEdit.h"

#include <QKeyEvent>
#include <QTextBlock>

using namespace KeyProcessingLayer;
using namespace BusinessLogic;
using UserInterface::ScenarioTextEdit;


DialogHandler::DialogHandler(ScenarioTextEdit* _editor) :
	StandardKeyHandler(_editor)
{
}

void DialogHandler::handleEnter(QKeyEvent*)
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
				// Меняем стиль блока на описание действия
				//
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Action);
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
					// Перейдём к блоку персонажа
					//
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Character);
				} else {
					//! Внутри блока

					//
					// Перейдём к блоку персонажа
					//
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Character);
				}
			}
		}
	}
}

void DialogHandler::handleTab(QKeyEvent*)
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
				// Меняем стиль на ремарку
				//
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Parenthetical);
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
					// Вставляем блок ремарки
					//
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Parenthetical);
				} else {
					//! Внутри блока

					//
					// Разрываем диалог ремаркой
					//

					//
					// ... оставляем пустой блок реплики
					//
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Dialog);
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Dialog);

					//
					// ... возвращаем курсор к пустому блоку
					//
					editor()->moveCursor(QTextCursor::PreviousBlock);

					//
					// ... делаем блок под курсором ремаркой
					//
					editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Parenthetical);
				}
			}
		}
	}
}

void DialogHandler::handleOther(QKeyEvent* _event)
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
	if (cursorBackwardText.endsWith("(")
		&& _event != 0
		&& _event->text() == "(") {
		//! Если нажата открывающая скобка

		//
		// Удалим лишнюю скобку
		//
		editor()->textCursor().deletePreviousChar();

		if (cursorForwardText.isEmpty()
			&& cursorBackwardText == "(") {
			//! Если текст пуст

			//
			// Cменить стиль на ремарку
			//
			editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Parenthetical);
		} else {
			//! Если текст не пуст

			//
			// Разрываем диалог ремаркой
			//

			//
			// ... оставляем пустой блок реплики
			//
			// если скобка нажата в начале строки, то делаем лишь один перевод строки
			//
			if (cursorBackwardText != "(") {
				editor()->addScenarioBlock(ScenarioTextBlockStyle::Dialog);
			}
			//
			// ... если после скобки нет текста, не добавляем новый параграф
			//
			if (!cursorForwardText.isEmpty()) {
				editor()->addScenarioBlock(ScenarioTextBlockStyle::Dialog);

				//
				// ... возвращаем курсор к пустому блоку
				//
				editor()->moveCursor(QTextCursor::PreviousBlock);
			}

			//
			// ... делаем блок под курсором ремаркой
			//
			editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Parenthetical);
		}
	} else {
		//! В противном случае, обрабатываем в базовом классе

		StandardKeyHandler::handleOther(_event);
	}
}
