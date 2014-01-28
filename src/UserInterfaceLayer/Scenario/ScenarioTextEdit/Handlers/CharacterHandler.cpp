#include "CharacterHandler.h"

#include "../Parsers/CharacterParser.h"

#include "../ScenarioTextEdit.h"

#include <Domain/Character.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>

#include <QKeyEvent>
#include <QTextBlock>

using namespace KeyProcessingLayer;
using namespace DataStorageLayer;
using namespace BusinessLogic;


CharacterHandler::CharacterHandler(ScenarioTextEdit* _editor) :
	StandardKeyHandler(_editor)
{
}

void CharacterHandler::handleEnter(QKeyEvent*)
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
		// Вставить выбранный вариант
		//
		editor()->applyCompletion();
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
				// Cменить стиль на описание действия
				//
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Action);
			} else {
				//! Текст не пуст

				//
				// Сохраним имя персонажа
				//
				storeCharacter();

				if (cursorBackwardText.isEmpty()) {
					//! В начале блока

					//
					// Вставим блок имени героя перед собой
					//
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Character);
				} else if (cursorForwardText.isEmpty()) {
					//! В конце блока

					//
					// Вставить блок реплики героя
					//
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Dialog);
				} else {
					//! Внутри блока

					//
					// Вставить блок реплики героя
					//
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Dialog);
				}
			}
		}
	}
}

void CharacterHandler::handleTab(QKeyEvent*)
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
		// Работаем, как ENTER
		//
		handleEnter();
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
				// Cменить стиль на описание действия
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
					// Вставить блок ремарки
					//
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Parenthetical);
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

void CharacterHandler::handleOther(QKeyEvent*)
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();
	// ... блок текста в котором находится курсор
	QTextBlock currentBlock = cursor.block();
	// ... текст блока
	QString currentBlockText = currentBlock.text();


	//
	// Получим модель подсказок для текущей секции и выведем пользователю
	//
	QAbstractItemModel* characterModel = StorageFacade::characterStorage()->all();

	//
	// Дополним текст
	//
	editor()->complete(characterModel, currentBlockText);
}

void CharacterHandler::storeCharacter() const
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();
	// ... блок текста в котором находится курсор
	QTextBlock currentBlock = cursor.block();
	// ... текст блока
	QString currentBlockText = currentBlock.text();
	// ... имя персонажа
	QString characterName = CharacterParser::name(currentBlockText);

	//
	// Сохраняем персонажа
	//
	StorageFacade::characterStorage()->storeCharacter(characterName);
}
