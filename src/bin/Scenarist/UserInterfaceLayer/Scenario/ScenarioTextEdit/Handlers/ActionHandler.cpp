#include "ActionHandler.h"

#include "../ScenarioTextEdit.h"

#include <Domain/Place.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/PlaceStorage.h>

#include <QKeyEvent>
#include <QTextBlock>

using namespace KeyProcessingLayer;
using namespace BusinessLogic;
using UserInterface::ScenarioTextEdit;


ActionHandler::ActionHandler(ScenarioTextEdit* _editor) :
	StandardKeyHandler(_editor)
{

}

void ActionHandler::handleEnter(QKeyEvent*)
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
				// Меняем стиль на место и время
				//
				editor()->changeScenarioBlockType(ScenarioBlockStyle::TimeAndPlace);
			} else {
				//! Текст не пуст

				//
				// Вставляем блок и применяем ему стиль описания действия
				//
//				editor()->addScenarioBlock(ScenarioBlockStyle::Action);
				editor()->addScenarioBlock(jumpForEnter(ScenarioBlockStyle::Action));
			}
		}
	}
}

void ActionHandler::handleTab(QKeyEvent*)
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
				// Если строка пуста, то сменить стиль на имя героя
				//
				editor()->changeScenarioBlockType(ScenarioBlockStyle::Character);
			} else {
				//! Текст не пуст

				if (cursorBackwardText.isEmpty()) {
					//! В начале блока

					//
					// Меняем на блок персонажа
					//
					editor()->changeScenarioBlockType(ScenarioBlockStyle::Character);
				} else if (cursorForwardText.isEmpty()) {
					//! В конце блока

					//
					// Вставляем блок персонажа
					//
//					editor()->addScenarioBlock(ScenarioBlockStyle::Character);
					editor()->addScenarioBlock(jumpForTab(ScenarioBlockStyle::Action));
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

void ActionHandler::handleOther(QKeyEvent* _event)
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


	//
	// Обработка
	//
	if (cursorBackwardText.endsWith(".")
		&& _event != 0
		&& _event->text() == ".") {
		//! Если нажата точка

		//
		// Если было введено какое-либо значение из списка мест (ИНТ./НАТ. и т.п.)
		// то необходимо преобразовать блок во время и место
		//
		QString maybePlace = cursorBackwardText.remove(".").toUpper();
		if (DataStorageLayer::StorageFacade::placeStorage()->hasPlace(maybePlace)) {
			editor()->changeScenarioBlockType(ScenarioBlockStyle::TimeAndPlace);
		}
	} else {
		//! В противном случае, обрабатываем в базовом классе

		StandardKeyHandler::handleOther(_event);
	}
}
