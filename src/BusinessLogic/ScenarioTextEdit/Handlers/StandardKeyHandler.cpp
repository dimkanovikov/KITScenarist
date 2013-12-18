#include "StandardKeyHandler.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>

#include <QKeyEvent>
#include <QTextBlock>

using namespace KeyProcessingLayer;

StandardKeyHandler::StandardKeyHandler(ScenarioTextEdit* _editor) :
	AbstractKeyHandler(_editor)
{
}
#include <QDebug>
void StandardKeyHandler::handleShortcut(QKeyEvent* _event)
{
	//
	// Получим необходимые переменные
	//
	// ... зажатые управляющие клавиши
	Qt::KeyboardModifiers pressedModifiers = _event->modifiers();

	if (pressedModifiers.testFlag(Qt::ControlModifier)) {
		//
		// Проверяем по коду клавиатуры, этот способ независим от выбранного пользователем языка
		//
		switch (_event->nativeScanCode()) {
			// z
			case 52: {
				if (pressedModifiers.testFlag(Qt::ShiftModifier)) {
					editor()->redo();
				} else {
					editor()->undo();
				}
				break;
			}

			// x
			case 53: {
				editor()->cut();
				break;
			}

			// c
			case 54: {
				editor()->copy();
				break;
			}

			// v
			case 55: {
				editor()->paste();
				break;
			}

			default: {
				break;
			}
		}
	}
}

void StandardKeyHandler::handleDelete(QKeyEvent*)
{
	//
	// Удаление
	//
	removeCharacters(false);

	//
	// Покажем подсказку, если это возможно
	//
	handleOther();
}

void StandardKeyHandler::handleBackspace(QKeyEvent*)
{
	//
	// Удаление
	//
	removeCharacters(true);

	//
	// Покажем подсказку, если это возможно
	//
	handleOther();
}

void StandardKeyHandler::handleEscape(QKeyEvent*)
{
	editor()->closeCompleter();
}

void StandardKeyHandler::handleUp(QKeyEvent* _event)
{
	//
	// Если подстановщик скрыт - имитируем действие обычного редактора
	//
	if (!editor()->isCompleterVisible()) {
		bool isShiftPressed = _event->modifiers().testFlag(Qt::ShiftModifier);
		editor()->moveCursor(
					QTextCursor::Up,
					isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
	}
}

void StandardKeyHandler::handleDown(QKeyEvent* _event)
{
	//
	// Если подстановщик скрыт - имитируем действие обычного редактора
	//
	if (!editor()->isCompleterVisible()) {
		bool isShiftPressed = _event->modifiers().testFlag(Qt::ShiftModifier);
		editor()->moveCursor(
					QTextCursor::Down,
					isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
	}
}

void StandardKeyHandler::handleOther(QKeyEvent*)
{

}

void StandardKeyHandler::removeCharacters(bool _backward)
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();

	//
	// Получим стиль первого блока в выделении
	//
	int topCursorPosition = cursor.position() - (_backward ? 1 : 0);
	if (cursor.hasSelection()) {
		topCursorPosition = qMin(cursor.selectionStart(), cursor.selectionEnd());
	}

	//
	// ... начало документа
	//
	if (topCursorPosition < 0) {
		return;
	}

	QTextCursor topCursor(editor()->document());
	topCursor.setPosition(topCursorPosition);
	ScenarioTextBlockStyle topStyle(editor()->scenarioBlockType(topCursor.block()));
	// ... если блок является заголовком - расширим выделение
	while (topStyle.isHeader()) {
		topCursor.movePosition(QTextCursor::Left);
		topStyle.setType(editor()->scenarioBlockType(topCursor.block()));

		// ... если дошли до начала документа
		if (topCursor.position() == 0) {
			topStyle.setType(ScenarioTextBlockStyle::TimeAndPlace);
		}
	}

	//
	// Получим стиль последнего блока в выделении для удаления
	//
	int bottomCursorPosition = cursor.position() + (_backward ? 0 : 1);
	if (cursor.hasSelection()) {
		bottomCursorPosition = qMax(cursor.selectionStart(), cursor.selectionEnd());
	}

	//
	// ... конец документа
	//
	if (bottomCursorPosition >= editor()->document()->characterCount()) {
		return;
	}

	QTextCursor bottomCursor(editor()->document());
	bottomCursor.setPosition(bottomCursorPosition);
	ScenarioTextBlockStyle bottomStyle(editor()->scenarioBlockType(bottomCursor.block()));
	// ... если блок является заголовком - расширим выделение
	while (bottomStyle.isHeader()) {
		bottomCursor.movePosition(QTextCursor::Right);
		bottomStyle.setType(editor()->scenarioBlockType(bottomCursor.block()));

		// ... после заголовка всегда есть текст, поэтому тут нет ограничений
	}

	//
	// Установим курсору выделение
	//
	cursor.setPosition(topCursor.position());
	cursor.setPosition(bottomCursor.position(), QTextCursor::KeepAnchor);

	//
	// Если в выделении есть блоки открывающие или закрывающие группы,
	// нужно так же удалить их пары
	//
	int groupHeadersToDeleteCount = 0;
	int groupFootersToDeleteCount = 0;

	//
	// Посчитаем количество открывающих и закрывающих блоков для удаления
	//
	findGroupBlocks(topCursor.position(),
					bottomCursor.position(),
					groupHeadersToDeleteCount,
					groupFootersToDeleteCount);

	//
	// Если блоков для удаления не найдено
	//
	if (groupHeadersToDeleteCount == 0
		&& groupFootersToDeleteCount == 0) {
		//
		// Если меняется стиль заголовка группы, то нужно удалить один конец и наоборот
		//
		if (topStyle.blockType() != bottomStyle.blockType()) {
			if (bottomStyle.blockType() == ScenarioTextBlockStyle::SceneGroupHeader) {
				++groupFootersToDeleteCount;
			} else if (bottomStyle.blockType() == ScenarioTextBlockStyle::SceneGroupFooter) {
				++groupHeadersToDeleteCount;
			}
		}
	}

	//
	// Очистим выделение
	//
	if (_backward) {
		cursor.deletePreviousChar();
	} else {
		cursor.deleteChar();
	}

	//
	// Удалим пары групп из стёртого выделения, если таковые есть
	//
	removeGroupsPairs(groupHeadersToDeleteCount, groupFootersToDeleteCount);

	//
	// Применим стиль ко всему тексту (главным образом к новому) в блоке
	//
	editor()->applyScenarioTypeToBlockText(topStyle.blockType());
}

void StandardKeyHandler::findGroupBlocks(
		int _startPosition,
		int _endPosition,
		int& _groupHeadersToRemoveCount,
		int& _groupFootersToRemoveCount)
{
	//
	// Если в выделении есть блоки открывающие или закрывающие группы,
	// нужно так же удалить их пары
	//
	QTextCursor searchGroupsCursor(editor()->document());
	searchGroupsCursor.setPosition(_startPosition);
	// поиск начинается с блока
	int searchStartFromBlock = searchGroupsCursor.blockNumber();
	// нужно сместиться в конец следующего
	// т.к. нас интересуют блоки полностью входящие в выделение
	searchGroupsCursor.movePosition(QTextCursor::NextBlock);
	searchGroupsCursor.movePosition(QTextCursor::EndOfBlock);

	//
	// Посчитаем количество открывающих и закрывающих блоков для удаления
	//
	_groupHeadersToRemoveCount = 0;
	_groupFootersToRemoveCount = 0;

	//
	// Если
	//
	if (searchStartFromBlock != searchGroupsCursor.blockNumber()) {

		while (searchGroupsCursor.position() <= _endPosition) {
			ScenarioTextBlockStyle::Type currentType =
					editor()->scenarioBlockType(searchGroupsCursor.block());

			//
			// Найден блок открывающий группу
			//
			if (currentType == ScenarioTextBlockStyle::SceneGroupHeader) {
				// ... нужно удалить закрывающий блок
				++_groupFootersToRemoveCount;
			}

			//
			// Найден блок закрывающий группу
			//
			else if (currentType == ScenarioTextBlockStyle::SceneGroupFooter) {
				// ... если все группы закрыты, нужно удалить предыдущую открытую
				if (_groupFootersToRemoveCount == 0) {
					++_groupHeadersToRemoveCount;
				}
				// ... в противном случае закрываем открытую группу
				else {
					--_groupFootersToRemoveCount;
				}
			}

			//
			// Если достигнут конец документа, то прервать выполнение
			//
			if (searchGroupsCursor.atEnd()) {
				break;
			}

			//
			// Перейдём в конец следующего блока
			//
			searchGroupsCursor.movePosition(QTextCursor::NextBlock);
			searchGroupsCursor.movePosition(QTextCursor::EndOfBlock);
		}
	}
}

void StandardKeyHandler::removeGroupsPairs(int _groupHeadersCount, int _groupFootersCount)
{
	//
	// Если есть что удалять
	//
	if (_groupHeadersCount > 0
		|| _groupFootersCount > 0) {

		//
		// Удалим пары из последующего текста
		//
		if (_groupFootersCount > 0) {
			QTextCursor cursor = editor()->textCursor();
			cursor.movePosition(QTextCursor::NextBlock);

			// ... открытые группы на пути поиска необходимого для удаления блока
			int openedGroups = 0;
			int groupsToDeleteCount = _groupFootersCount;
			do {
				ScenarioTextBlockStyle::Type currentType =
						editor()->scenarioBlockType(cursor.block());

				if (currentType == ScenarioTextBlockStyle::SceneGroupFooter) {
					if (openedGroups == 0) {
						cursor.select(QTextCursor::BlockUnderCursor);
						cursor.deleteChar();
						--groupsToDeleteCount;
					} else {
						--openedGroups;
					}
				} else if (currentType == ScenarioTextBlockStyle::SceneGroupHeader) {
					// ... встретилась новая группа, которую не нужно удалять
					++openedGroups;
				}

				cursor.movePosition(QTextCursor::NextBlock);
			} while (groupsToDeleteCount > 0);
		}

		//
		// Удалим пары из предшествующего текста
		//
		if (_groupHeadersCount > 0) {
			QTextCursor cursor = editor()->textCursor();
			cursor.movePosition(QTextCursor::PreviousBlock);

			// ... открытые группы на пути поиска необходимого для удаления блока
			int openedGroups = 0;
			int groupsToDeleteCount = _groupHeadersCount;
			do {
				ScenarioTextBlockStyle::Type currentType =
						editor()->scenarioBlockType(cursor.block());

				if (currentType == ScenarioTextBlockStyle::SceneGroupHeader) {
					if (openedGroups == 0) {
						cursor.select(QTextCursor::BlockUnderCursor);
						cursor.deleteChar();
						--groupsToDeleteCount;
					} else {
						--openedGroups;
					}
				} else if (currentType == ScenarioTextBlockStyle::SceneGroupFooter) {
					// ... встретилась новая группа, которую не нужно удалять
					++openedGroups;
				}

				cursor.movePosition(QTextCursor::PreviousBlock);
			} while (groupsToDeleteCount > 0);
		}
	}
}
