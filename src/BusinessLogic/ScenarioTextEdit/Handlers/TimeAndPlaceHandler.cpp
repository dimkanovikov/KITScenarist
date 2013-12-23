#include "TimeAndPlaceHandler.h"

#include "../Parsers/TimeAndPlaceParser.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>

#include <Domain/Place.h>
#include <Domain/Location.h>
#include <Domain/ScenarioDay.h>
#include <Domain/Time.h>

#include <Storage/StorageFacade.h>
#include <Storage/PlaceStorage.h>
#include <Storage/LocationStorage.h>
#include <Storage/ScenarioDayStorage.h>
#include <Storage/TimeStorage.h>

#include <QKeyEvent>
#include <QTextBlock>

using namespace Domain;
using namespace StorageLayer;
using namespace KeyProcessingLayer;


TimeAndPlaceHandler::TimeAndPlaceHandler(ScenarioTextEdit* _editor) :
	StandardKeyHandler(_editor)
{
}

void TimeAndPlaceHandler::handleEnter(QKeyEvent*)
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
	TimeAndPlaceSection currentSection = TimeAndPlaceParser::section(cursorBackwardText);


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
		// Дописать необходимые символы
		//
		switch (currentSection) {
			case SceneHeaderSectionPlace: {
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
			} else {
				//! Текст не пуст

				if (cursorBackwardText.isEmpty()) {
					//! В начале блока

					//
					// Сохраним параметры сцены
					//
					storeSceneParameters();

					//
					// Вставка блока заголовка перед собой
					//
					editor()->addScenarioBlock(ScenarioTextBlockStyle::TimeAndPlace);
				} else if (cursorForwardText.isEmpty()) {
					//! В конце блока

					//
					// Сохраним параметры сцены
					//
					storeSceneParameters();

					//
					// Вставка блока описания действия
					//
					editor()->addScenarioBlock(ScenarioTextBlockStyle::Action);
				} else {
					//! Внутри блока

					//
					// Ни чего не делаем
					//
				}
			}
		}
	}

	//
	// FIXME: Сохранение элементов в базу данных (локация и т.п.) происходит на этом этапе,
	//		  в тот момент, когда пользователь переходит к описанию действия
	//
}

void TimeAndPlaceHandler::handleTab(QKeyEvent*)
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
					// Если нет времени, то добавление " - " и отображение подсказки
					//
					if (TimeAndPlaceParser::timeName(cursorBackwardText).isEmpty()) {
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
						// Покажем подсказку
						//
						handleOther();
					}

					//
					// В противном случае перейдём к блоку описания действия
					//
					else {
						handleEnter();
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

void TimeAndPlaceHandler::handleOther(QKeyEvent*)
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
	TimeAndPlaceSection currentSection = TimeAndPlaceParser::section(cursorBackwardText);


	//
	// Получим модель подсказок для текущей секции и выведем пользователю
	//
	QAbstractItemModel* sectionModel = 0;
	//
	// ... в соответствии со введённым в секции текстом
	//
	QString sectionText;

	switch (currentSection) {
		case SceneHeaderSectionPlace: {
			sectionModel = StorageFacade::placeStorage()->all();
			sectionText = TimeAndPlaceParser::placeName(currentBlockText);
			break;
		}

		case SceneHeaderSectionLocation: {
			sectionModel = StorageFacade::locationStorage()->allMainLocations();
			sectionText = TimeAndPlaceParser::locationName(currentBlockText);
			break;
		}

		case SceneHeaderSectionSubLocation: {
			QString locationName = TimeAndPlaceParser::locationName(cursorBackwardText);
			sectionModel = StorageFacade::locationStorage()->sublocations(locationName);
			sectionText = TimeAndPlaceParser::sublocationName(currentBlockText);
			break;
		}

		case SceneHeaderSectionScenarioDay: {
			sectionModel = StorageFacade::scenarioDayStorage()->all();
			sectionText = TimeAndPlaceParser::scenarioDayName(currentBlockText);
			break;
		}

		case SceneHeaderSectionTime: {
			sectionModel = StorageFacade::timeStorage()->all();
			sectionText = TimeAndPlaceParser::timeName(currentBlockText);
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

void TimeAndPlaceHandler::storeSceneParameters() const
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
	// ... локация
	QString locationName = TimeAndPlaceParser::locationName(currentBlockText);
	// ... подлокация
	QString sublocationName = TimeAndPlaceParser::sublocationName(currentBlockText);
	// ... сценарный день
	QString scenarioDayName = TimeAndPlaceParser::scenarioDayName(currentBlockText);

	//
	// Сохраняем локацию и подлокацию
	//
	StorageFacade::locationStorage()->storeLocationWithSublocation(locationName, sublocationName);

	//
	// Сохраняем сценарный день
	//
	StorageFacade::scenarioDayStorage()->storeScenarioDay(scenarioDayName);
}
