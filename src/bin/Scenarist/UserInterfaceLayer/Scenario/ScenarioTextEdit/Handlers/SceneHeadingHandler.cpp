#include "SceneHeadingHandler.h"

#include "../ScenarioTextEdit.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>

#include <Domain/Place.h>
#include <Domain/Location.h>
#include <Domain/ScenarioDay.h>
#include <Domain/Time.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/PlaceStorage.h>
#include <DataLayer/DataStorageLayer/LocationStorage.h>
#include <DataLayer/DataStorageLayer/ScenarioDayStorage.h>
#include <DataLayer/DataStorageLayer/TimeStorage.h>

#include <QKeyEvent>
#include <QTextBlock>

using namespace Domain;
using namespace DataStorageLayer;
using namespace KeyProcessingLayer;
using namespace BusinessLogic;
using UserInterface::ScenarioTextEdit;


SceneHeadingHandler::SceneHeadingHandler(ScenarioTextEdit* _editor) :
	StandardKeyHandler(_editor)
{
}

void SceneHeadingHandler::handleEnter(QKeyEvent*)
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
	// ... текущая секция
	SceneHeadingParser::Section currentSection =
			SceneHeadingParser::section(cursorBackwardText);


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
			case SceneHeadingParser::SectionPlace: {
				cursor.insertText(". ");
				break;
			}

			default: {
				break;
			}
		}

		//
		// Покажем подсказку, если это возможно
		//
		handleOther();

		//
		// Если нужно автоматически перепрыгиваем к следующему блоку
		//
		if (autoJumpToNextBlock()
			&& currentSection == SceneHeadingParser::SectionTime) {
			//
			// Сохраним параметры сцены
			//
			storeSceneParameters();
			//
			// Переходим к следующему блоку
			//
			cursor.movePosition(QTextCursor::EndOfBlock);
			editor()->setTextCursor(cursor);
			editor()->addScenarioBlock(jumpForEnter(ScenarioBlockStyle::SceneHeading));
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
				// Меняем в соответствии с настройками
				//
				editor()->changeScenarioBlockType(changeForEnter(ScenarioBlockStyle::SceneHeading));
			} else {
				//! Текст не пуст

				//
				// Сохраним параметры сцены
				//
				storeSceneParameters();

				if (cursorBackwardText.isEmpty()) {
					//! В начале блока

					//
					// Вставка блока заголовка перед собой
					//
					editor()->addScenarioBlock(ScenarioBlockStyle::SceneHeading);
				} else if (cursorForwardText.isEmpty()) {
					//! В конце блока

					//
					// Вставка блока описания действия
					//
					editor()->addScenarioBlock(jumpForEnter(ScenarioBlockStyle::SceneHeading));
				} else {
					//! Внутри блока

					//
					// Вставка блока описания действия
					//
					editor()->addScenarioBlock(ScenarioBlockStyle::Action);
				}
			}
		}
	}
}

void SceneHeadingHandler::handleTab(QKeyEvent*)
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
		// Работаем аналогично нажатию ENTER
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
				// Если строка пуста, то сменить стиль на описание действия
				//
				editor()->changeScenarioBlockType(changeForTab(ScenarioBlockStyle::SceneHeading));
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
					// Если нет времени, то добавление " - " и отображение подсказки
					//
					if (SceneHeadingParser::timeName(cursorBackwardText).isEmpty()) {
						//
						// Добавим необходимый текст в зависимости от того, что ввёл пользователь
						//
						if (cursorBackwardText.endsWith(" -")) {
							cursor.insertText(" ");
						} else if (cursorBackwardText.endsWith(" ")) {
							cursor.insertText("- ");
						} else {
							cursor.insertText(" - ");
						}

						//
						// Отображение подсказки
						//
						handleOther();
					}

					//
					// В противном случае перейдём к блоку участников сцены
					//
					else {
						//
						// Сохраним параметры сцены
						//
						storeSceneParameters();

						//
						// А затем вставим блок
						//
						editor()->addScenarioBlock(jumpForTab(ScenarioBlockStyle::SceneHeading));
					}
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

void SceneHeadingHandler::handleOther(QKeyEvent*)
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
	SceneHeadingParser::Section currentSection = SceneHeadingParser::section(cursorBackwardText);


	//
	// Получим модель подсказок для текущей секции и выведем пользователю
	//
	QAbstractItemModel* sectionModel = 0;
	//
	// ... в соответствии со введённым в секции текстом
	//
	QString sectionText;

	switch (currentSection) {
		case SceneHeadingParser::SectionPlace: {
			sectionModel = StorageFacade::placeStorage()->all();
			sectionText = SceneHeadingParser::placeName(currentBlockText);
			break;
		}

		case SceneHeadingParser::SectionLocation: {
			sectionModel = StorageFacade::locationStorage()->all();
			sectionText = SceneHeadingParser::locationName(currentBlockText);
			break;
		}

		case SceneHeadingParser::SectionScenarioDay: {
			sectionModel = StorageFacade::scenarioDayStorage()->all();
			sectionText = SceneHeadingParser::scenarioDayName(currentBlockText);
			break;
		}

		case SceneHeadingParser::SectionTime: {
			sectionModel = StorageFacade::timeStorage()->all();
			sectionText = SceneHeadingParser::timeName(currentBlockText);
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

void SceneHeadingHandler::storeSceneParameters() const
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

		//
		// Сохраняем время
		//
		QString placeName = SceneHeadingParser::placeName(currentBlockText);
		StorageFacade::placeStorage()->storePlace(placeName);

		//
		// Сохраняем локацию
		//
		QString locationName = SceneHeadingParser::locationName(currentBlockText);
		StorageFacade::locationStorage()->storeLocation(locationName);

		//
		// Сохраняем место
		//
		QString timeName = SceneHeadingParser::timeName(currentBlockText);
		StorageFacade::timeStorage()->storeTime(timeName);

		//
		// Сохраняем сценарный день
		//
		QString scenarioDayName = SceneHeadingParser::scenarioDayName(currentBlockText);
		StorageFacade::scenarioDayStorage()->storeScenarioDay(scenarioDayName);
	}
}
