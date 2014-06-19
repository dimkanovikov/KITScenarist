#include "SceneCharactersHandler.h"

#include "../ScenarioTextEdit.h"

#include <Domain/Character.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>

#include <QKeyEvent>
#include <QStringListModel>
#include <QTextBlock>

using namespace KeyProcessingLayer;
using namespace DataStorageLayer;
using namespace BusinessLogic;
using UserInterface::ScenarioTextEdit;

SceneCharactersHandler::SceneCharactersHandler(ScenarioTextEdit* _editor) :
	StandardKeyHandler(_editor),
	m_filteredCharactersModel(new QStringListModel(_editor))
{
}

void SceneCharactersHandler::handleEnter(QKeyEvent*)
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
				// Сохраним персонажей
				//
				storeCharacters();

				if (cursorBackwardText.isEmpty()) {
					//! В начале блока

					//
					// Ни чего не делаем
					//
				} else if (cursorForwardText.isEmpty()) {
					//! В конце блока

					//
					// Вставляем блок и применяем ему стиль время и место
					//
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Action);
				} else {
					//! Внутри блока

					//
					// Вставляем блок и применяем ему стиль время и место
					//
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Action);
				}
			}
		}
	}
}

void SceneCharactersHandler::handleTab(QKeyEvent* _event)
{
	handleEnter(_event);
}

void SceneCharactersHandler::handleOther(QKeyEvent*)
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
	// ... текст до курсора
	QString cursorBackwardText = currentBlock.text().left(cursor.positionInBlock());
	// ... текст до запятой
	QString cursorBackwardTextToComma = cursorBackwardText;
	if (!cursorBackwardTextToComma.split(", ").isEmpty()) {
		cursorBackwardTextToComma = cursorBackwardTextToComma.split(", ").last();
	}

	//
	// Получим модель подсказок для текущей секции и выведем пользователю
	//
	QAbstractItemModel* characterModel = StorageFacade::characterStorage()->all();

	//
	// Убрать из модели уже использованные элементы
	//
	// ... сформируем список уже введённых персонажей
	//
	QStringList enteredCharacters = currentBlockText.toUpper().split(", ");
	enteredCharacters.removeOne(cursorBackwardTextToComma);
	//
	// ... скорректируем модель
	//
	QStringList filteredCharacters;
	for (int row = 0; row < characterModel->rowCount(); ++row) {
		const QString characterName =
				characterModel->data(characterModel->index(row, 0)).toString().toUpper();
		if (!enteredCharacters.contains(characterName)) {
			filteredCharacters.append(characterName);
		}
	}
	m_filteredCharactersModel->setStringList(filteredCharacters);

	//
	// Дополним текст
	//
	editor()->complete(m_filteredCharactersModel, cursorBackwardTextToComma);
}

void SceneCharactersHandler::storeCharacters() const
{
	if (editor()->storeDataWhenEditing()) {
		//
		// Получим необходимые значения
		//
		// ... курсор в текущем положении
		QTextCursor cursor = editor()->textCursor();
		// ... блок текста в котором находится курсор
		QTextBlock currentBlock = cursor.block();
		// ... текст блока
		QString currentBlockText = currentBlock.text();
		// ... персонажы
		QStringList enteredCharacters = currentBlockText.split(", ");

		foreach (const QString& character, enteredCharacters) {
			//
			// Сохраняем персонажа
			//
			StorageFacade::characterStorage()->storeCharacter(character);
		}
	}
}
