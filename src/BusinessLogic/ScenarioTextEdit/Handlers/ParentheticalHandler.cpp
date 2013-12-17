#include "ParentheticalHandler.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>
#include <BusinessLogic/ScenarioTextEdit/ScenarioTextBlock/ScenarioTextBlockStyle.h>

#include <QKeyEvent>
#include <QTextBlock>

using namespace KeyProcessingLayer;


ParentheticalHandler::ParentheticalHandler(ScenarioTextEdit* _editor) :
	StandardKeyHandler(_editor)
{
	ScenarioTextBlockStyle style(ScenarioTextBlockStyle::Parenthetical);
	m_stylePrefix = style.prefix();
	m_stylePostfix = style.postfix();
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
				|| (cursorBackwardText + cursorForwardText == m_stylePrefix + m_stylePostfix)) {
				//! Текст пуст

				//
				// Ни чего не делаем
				//
			} else {
				//! Текст не пуст

				if (cursorBackwardText.isEmpty()
					|| cursorBackwardText == m_stylePrefix) {
					//! В начале блока

					//
					// Ни чего не делаем
					//
				} else if (cursorForwardText.isEmpty()
						   || cursorForwardText == m_stylePostfix) {
					//! В конце блока

					//
					// Перейдём к блоку реплики
					//
					cursor.movePosition(QTextCursor::EndOfBlock);
					editor()->setTextCursor(cursor);
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Dialog);
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
				|| (cursorBackwardText + cursorForwardText == m_stylePrefix + m_stylePostfix)) {
				//! Текст пуст

				//
				// Меняем стиль на реплику
				//
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Dialog);
			} else {
				//! Текст не пуст

				if (cursorBackwardText.isEmpty()
					|| cursorBackwardText == m_stylePrefix) {
					//! В начале блока

					//
					// Ни чего не делаем
					//
				} else if (cursorForwardText.isEmpty()
						   || cursorForwardText == m_stylePostfix) {
					//! В конце блока

					//
					// Вставляем блок реплики
					//
					cursor.movePosition(QTextCursor::EndOfBlock);
					editor()->setTextCursor(cursor);
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Dialog);
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
