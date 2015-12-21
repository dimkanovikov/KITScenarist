#include "CharacterHandler.h"

#include "../ScenarioTextEdit.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>

#include <Domain/Character.h>
#include <Domain/CharacterState.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>
#include <DataLayer/DataStorageLayer/CharacterStateStorage.h>

#include <QKeyEvent>
#include <QStringListModel>
#include <QTextBlock>

using namespace KeyProcessingLayer;
using namespace DataStorageLayer;
using namespace BusinessLogic;
using UserInterface::ScenarioTextEdit;


CharacterHandler::CharacterHandler(ScenarioTextEdit* _editor) :
	StandardKeyHandler(_editor),
	m_sceneCharactersModel(new QStringListModel(_editor))
{
}

void CharacterHandler::prehandle()
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

	//
	// Пробуем определить кто сейчас должен говорить
	//
	if (currentBlockText.isEmpty()) {
		QString previousCharacter, character;

		//
		// ... для этого ищем предпоследнего персонажа сцены
		//
		cursor.movePosition(QTextCursor::PreviousBlock);
		while (!cursor.atStart()
			   && ScenarioBlockStyle::forBlock(cursor.block()) != ScenarioBlockStyle::SceneHeading) {
			if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::Character) {
				//
				// Нашли предыдущего персонажа
				//
				if (previousCharacter.isEmpty()) {
					previousCharacter = CharacterParser::name(cursor.block().text());
				}
				//
				// Нашли потенциального говорящего
				//
				else {
					//
					// Выберем его в списке вариантов
					//
					character = CharacterParser::name(cursor.block().text());
					if (character != previousCharacter) {
						break;
					}
				}
			}

			cursor.movePosition(QTextCursor::PreviousBlock);
		}

		//
		// Показываем всплывающую подсказку
		//
		QAbstractItemModel* model = 0;
		if (character.isEmpty()) {
			model = StorageFacade::characterStorage()->all();
		} else {
			m_sceneCharactersModel->setStringList(QStringList() << character.toUpper());
			model = m_sceneCharactersModel;
		}
		editor()->complete(model, QString::null);
	}
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
	// ... текст блока
	QString currentBlockText = currentBlock.text().trimmed();
	// ... текст до курсора
	QString cursorBackwardText = currentBlockText.left(cursor.positionInBlock());
	// ... текст после курсора
	QString cursorForwardText = currentBlockText.mid(cursor.positionInBlock());
	// ... текущая секция
	CharacterParser::Section currentSection = CharacterParser::section(cursorBackwardText);


	//
	// Обработка
	//
	if (editor()->isCompleterVisible()) {
		//! Если открыт подстановщик

		//
		// Вставить выбранный вариант
		//
		editor()->applyCompletion();

		//
		// Обновим курсор, т.к. после автозавершения он смещается
		//
		cursor = editor()->textCursor();

		//
		// Дописать необходимые символы
		//
		switch (currentSection) {
			case CharacterParser::SectionState: {
				cursor.insertText(")");
				break;
			}

			default: {
				break;
			}
		}

		//
		// Если нужно автоматически перепрыгиваем к следующему блоку
		//
		if (autoJumpToNextBlock()
			&& currentSection == CharacterParser::SectionName) {
			cursor.movePosition(QTextCursor::EndOfBlock);
			editor()->setTextCursor(cursor);
			editor()->addScenarioBlock(jumpForEnter(ScenarioBlockStyle::Character));
		}
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
				editor()->changeScenarioBlockType(changeForEnter(ScenarioBlockStyle::Character));
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
					editor()->addScenarioBlock(ScenarioBlockStyle::Character);
				} else if (cursorForwardText.isEmpty()) {
					//! В конце блока

					//
					// Вставить блок реплики героя
					//
					editor()->addScenarioBlock(jumpForEnter(ScenarioBlockStyle::Character));
				} else {
					//! Внутри блока

					//
					// Вставить блок реплики героя
					//
					editor()->addScenarioBlock(ScenarioBlockStyle::Dialogue);
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
				editor()->changeScenarioBlockType(changeForTab(ScenarioBlockStyle::Character));
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
					// Сохраним имя персонажа
					//
					storeCharacter();

					//
					// Вставить блок ремарки
					//
					editor()->addScenarioBlock(jumpForTab(ScenarioBlockStyle::Character));
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
	// ... текст до курсора
	QString cursorBackwardText = currentBlock.text().left(cursor.positionInBlock());
	// ... текущая секция
	CharacterParser::Section currentSection = CharacterParser::section(cursorBackwardText);


	//
	// Получим модель подсказок для текущей секции и выведем пользователю
	//
	QAbstractItemModel* sectionModel = 0;
	//
	// ... в соответствии со введённым в секции текстом
	//
	QString sectionText;

	switch (currentSection) {
		case CharacterParser::SectionName: {
			QStringList sceneCharacters;
			//
			// Когда введён один символ имени пробуем оптимизировать поиск персонажей из текущей сцены
			//
			if (cursorBackwardText.length() < 2) {
				cursor.movePosition(QTextCursor::PreviousBlock);
				while (!cursor.atStart()
					   && ScenarioBlockStyle::forBlock(cursor.block()) != ScenarioBlockStyle::SceneHeading) {
					if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::Character) {
						const QString character = CharacterParser::name(cursor.block().text());
						if (character.startsWith(cursorBackwardText, Qt::CaseInsensitive)
							&& !sceneCharacters.contains(character)) {
							sceneCharacters.append(character.toUpper());
						}
					} else if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneCharacters) {
						const QStringList characters = SceneCharactersParser::characters(cursor.block().text());
						foreach (const QString& character, characters) {
							if (character.startsWith(cursorBackwardText, Qt::CaseInsensitive)
								&& !sceneCharacters.contains(character)) {
								sceneCharacters.append(character.toUpper());
							}
						}
					}
					cursor.movePosition(QTextCursor::PreviousBlock);
				}
			}

			//
			// По возможности используем список персонажей сцены
			//
			if (!sceneCharacters.isEmpty()) {
				m_sceneCharactersModel->setStringList(sceneCharacters);
				sectionModel = m_sceneCharactersModel;
			} else {
				sectionModel = StorageFacade::characterStorage()->all();
			}
			sectionText = CharacterParser::name(currentBlockText);
			break;
		}

		case CharacterParser::SectionState: {
			sectionModel = StorageFacade::characterStateStorage()->all();
			sectionText = CharacterParser::state(currentBlockText);
			break;
		}

		default: {
			break;
		}
	}

	//
	// Дополним текст
	//
	editor()->complete(sectionModel, sectionText);
}

void CharacterHandler::storeCharacter() const
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
		// ... имя персонажа
		QString characterName = CharacterParser::name(currentBlockText);
		// ... состояние персонажа
		QString characterState = CharacterParser::state(currentBlockText);

		//
		// Сохраняем персонажа
		//
		StorageFacade::characterStorage()->storeCharacter(characterName);
		StorageFacade::characterStateStorage()->storeCharacterState(characterState);
	}
}
