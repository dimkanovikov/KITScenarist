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
	// ... текст блока
	QString currentBlockText = currentBlock.text().trimmed();
	// ... текст до курсора
	QString cursorBackwardText = currentBlockText.left(cursor.positionInBlock());
	// ... текст после курсора
	QString cursorForwardText = currentBlockText.mid(cursor.positionInBlock());


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
				editor()->changeScenarioBlockType(changeForEnter(ScenarioBlockStyle::Dialogue));
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
					editor()->addScenarioBlock(jumpForEnter(ScenarioBlockStyle::Dialogue));
				} else {
					//! Внутри блока

					//
					// Разрываем диалог блоком персонажа, вставляя его имя
					//
					{
						//
						// Найти име персонажа, кому принадлежит реплика
						//
						QString characterName;
						{
							QTextCursor cursor = editor()->textCursor();
							QTextBlock cursorBlock = cursor.block();
							while ((ScenarioBlockStyle::forBlock(cursorBlock) != ScenarioBlockStyle::Character
									|| ScenarioBlockStyle::forBlock(cursorBlock) == ScenarioBlockStyle::Dialogue
									|| ScenarioBlockStyle::forBlock(cursorBlock) == ScenarioBlockStyle::Parenthetical)
								   && !cursor.atStart()) {
								cursor.movePosition(QTextCursor::PreviousBlock);
								cursorBlock = cursor.block();
							}

							if (ScenarioBlockStyle::forBlock(cursorBlock) == ScenarioBlockStyle::Character) {
								characterName = cursorBlock.text().simplified();
							}
						}

						//
						// Вставляем блок "герой" и добавляем имя
						//
						editor()->addScenarioBlock(ScenarioBlockStyle::Character);
						editor()->insertPlainText(characterName);

						//
						// Оставшийся текст форматируем, как "диалог"
						//
						editor()->addScenarioBlock(ScenarioBlockStyle::Dialogue);
					}
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
				editor()->changeScenarioBlockType(changeForTab(ScenarioBlockStyle::Dialogue));
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
					editor()->addScenarioBlock(jumpForTab(ScenarioBlockStyle::Dialogue));
				} else {
					//! Внутри блока

					//
					// Разрываем диалог ремаркой
					//

					//
					// ... оставляем пустой блок реплики
					//
					editor()->addScenarioBlock(ScenarioBlockStyle::Dialogue);
					editor()->addScenarioBlock(ScenarioBlockStyle::Dialogue);

					//
					// ... возвращаем курсор к пустому блоку
					//
					cursor = editor()->textCursor();
					cursor.movePosition(QTextCursor::PreviousBlock);
					editor()->setTextCursorReimpl(cursor);

					//
					// ... делаем блок под курсором ремаркой
					//
					editor()->changeScenarioBlockType(ScenarioBlockStyle::Parenthetical);
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
			editor()->changeScenarioBlockType(ScenarioBlockStyle::Parenthetical);
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
				editor()->addScenarioBlock(ScenarioBlockStyle::Dialogue);
			}
			//
			// ... если после скобки нет текста, не добавляем новый параграф
			//
			if (!cursorForwardText.isEmpty()) {
				editor()->addScenarioBlock(ScenarioBlockStyle::Dialogue);

				//
				// ... возвращаем курсор к пустому блоку
				//
				cursor = editor()->textCursor();
				cursor.movePosition(QTextCursor::PreviousBlock);
				editor()->setTextCursorReimpl(cursor);
			}

			//
			// ... делаем блок под курсором ремаркой
			//
			editor()->changeScenarioBlockType(ScenarioBlockStyle::Parenthetical);
		}
	} else {
		//! В противном случае, обрабатываем в базовом классе

		StandardKeyHandler::handleOther(_event);
	}
}
