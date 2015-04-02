#include "StandardKeyHandler.h"

#include "../ScenarioTextEdit.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QKeyEvent>
#include <QTextBlock>

using namespace KeyProcessingLayer;
using namespace BusinessLogic;
using UserInterface::ScenarioTextEdit;

namespace {
	/**
	 * @brief Получить тип перехода/смены в зависимости от заданных параметров
	 */
	static ScenarioBlockStyle::Type actionFor(bool _tab, bool _jump, ScenarioBlockStyle::Type _blockType) {
		const QString settingsKey =
				QString("scenario-editor/styles-%1/from-%2-by-%3")
				.arg(_jump ? "jumping" : "changing")
				.arg(ScenarioBlockStyle::typeName(_blockType))
				.arg(_tab ? "tab" : "enter");

		int result =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					settingsKey, DataStorageLayer::SettingsStorage::ApplicationSettings
					).toInt();

		return (ScenarioBlockStyle::Type)result;
	}

	/**
	 * @brief Вспомогательные константы для использования с функцией actionFor
	 */
	/** @{ */
	const bool TAB = true;
	const bool ENTER = false;
	const bool JUMP = true;
	const bool CHANGE = false;
	/** @} */
}


StandardKeyHandler::StandardKeyHandler(ScenarioTextEdit* _editor) :
	AbstractKeyHandler(_editor)
{
}

ScenarioBlockStyle::Type StandardKeyHandler::jumpForTab(ScenarioBlockStyle::Type _blockType)
{
	return ::actionFor(TAB, JUMP, _blockType);
}

ScenarioBlockStyle::Type StandardKeyHandler::jumpForEnter(ScenarioBlockStyle::Type _blockType)
{
	return ::actionFor(ENTER, JUMP, _blockType);
}

ScenarioBlockStyle::Type StandardKeyHandler::changeForTab(ScenarioBlockStyle::Type _blockType)
{
	return ::actionFor(TAB, CHANGE, _blockType);
}

ScenarioBlockStyle::Type StandardKeyHandler::changeForEnter(ScenarioBlockStyle::Type _blockType)
{
	return ::actionFor(ENTER, CHANGE, _blockType);
}

void StandardKeyHandler::handleDelete(QKeyEvent* _event)
{
	//
	// Удаление
	//
	removeCharacters(false);

	//
	// Покажем подсказку, если это возможно
	//
	handleOther(_event);
}

void StandardKeyHandler::handleBackspace(QKeyEvent* _event)
{
	//
	// Удаление
	//
	removeCharacters(true);

	//
	// Покажем подсказку, если это возможно
	//
	handleOther(_event);
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

		QTextCursor cursor = editor()->textCursor();
		cursor.beginEditBlock();

		//
		// Исходная позиция курсора
		//
		int initCursorPosition = cursor.position();

		//
		// Рассчитаем количество символов от края
		//
		int marginFromLineStart = 0;
		{
			int currentLineYCoordinate = editor()->cursorRect(cursor).y();
			while (!cursor.atStart()
				   && editor()->cursorRect(cursor).y() == currentLineYCoordinate) {
				cursor.movePosition(QTextCursor::Left,
									isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
			}
			marginFromLineStart =
					initCursorPosition
					- cursor.position()
					- (cursor.atStart() ? 0 : 1);
		}

		//
		// В данный момент курсор либо в начале документа, либо поднялся к концу предыдущей строки
		//
		if (!cursor.atStart()) {
			//
			// Сместим курсор в предыдущей строке на то кол-во символов, на которое он был смещён прежде
			//
			{
				int currentLineEndPosition = cursor.position();
				int currentLineYCoordinate = editor()->cursorRect(cursor).y();
				while (!cursor.atStart()
					   && editor()->cursorRect(cursor).y() == currentLineYCoordinate) {
					cursor.movePosition(
								QTextCursor::Left,
								isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
				}

				//
				// Возвратим курсор на одну позицию назад, т.к. в предыдущем цикле мы перешли на новую строку
				//
				if (!cursor.atStart()) {
					cursor.movePosition(
								QTextCursor::Right,
								isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
				}

				int currentLineStartPosition = cursor.position();
				if (currentLineStartPosition + marginFromLineStart < currentLineEndPosition) {
					cursor.movePosition(
								QTextCursor::Right,
								isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor,
								marginFromLineStart);
				} else {
					cursor.setPosition(
								currentLineEndPosition,
								isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
				}
			}
		}

		cursor.endEditBlock();
		editor()->setTextCursor(cursor);
	}
}

void StandardKeyHandler::handleDown(QKeyEvent* _event)
{
	//
	// Если подстановщик скрыт - имитируем действие обычного редактора
	//
	if (!editor()->isCompleterVisible()) {
		bool isShiftPressed = _event->modifiers().testFlag(Qt::ShiftModifier);

		QTextCursor cursor = editor()->textCursor();
		cursor.beginEditBlock();

		//
		// Исходная позиция курсора
		//
		int initCursorPosition = cursor.position();

		//
		// Рассчитаем количество символов от края
		//
		int marginFromLineStart = 0;
		{
			int currentLineYCoordinate = editor()->cursorRect(cursor).y();
			while (!cursor.atStart()
				   && editor()->cursorRect(cursor).y() == currentLineYCoordinate) {
				cursor.movePosition(QTextCursor::Left,
									isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
			}
			marginFromLineStart =
					initCursorPosition
					- cursor.position()
					- (cursor.atStart() ? 0 : 1);
		}

		//
		// Вернём курсор в исходное положение
		//
		cursor.setPosition(initCursorPosition,
						   isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);

		//
		// Сместим курсор к следующей строке или к концу документа
		//
		{
			int currentLineYCoordinate = editor()->cursorRect(cursor).y();
			while (!cursor.atEnd()
				   && editor()->cursorRect(cursor).y() == currentLineYCoordinate) {
				cursor.movePosition(QTextCursor::Right,
									isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
			}
		}

		//
		// В данный момент курсор либо в конце документа, либо перешёл к началу следующей строки
		//
		if (!cursor.atEnd()) {
			//
			// Сместим курсор в следующей строке на то кол-во символов, на которое он был смещён прежде
			//
			{
				int currentLineStartPosition = cursor.position();
				int currentLineYCoordinate = editor()->cursorRect(cursor).y();
				while (!cursor.atEnd()
					   && editor()->cursorRect(cursor).y() == currentLineYCoordinate) {
					cursor.movePosition(
								QTextCursor::Right,
								isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
				}

				//
				// Возвратим курсор на одну позицию назад, т.к. в предыдущем цикле мы перешли на новую строку
				//
				if (!cursor.atEnd()) {
					cursor.movePosition(
								QTextCursor::Left,
								isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
				}

				int currentLineEndPosition = cursor.position();
				if (currentLineStartPosition + marginFromLineStart < currentLineEndPosition) {
					cursor.movePosition(
								QTextCursor::Left,
								isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor,
								currentLineEndPosition - currentLineStartPosition - marginFromLineStart);
				} else {
					cursor.setPosition(
								currentLineEndPosition,
								isShiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
				}
			}
		}

		cursor.endEditBlock();
		editor()->setTextCursor(cursor);
	}
}

void StandardKeyHandler::handlePageUp(QKeyEvent* _event)
{
	QTextCursor cursor = editor()->textCursor();
	cursor.beginEditBlock();

	for (int line = 0; line < 20; ++line) {
		handleUp(_event);
	}

	cursor.endEditBlock();
}

void StandardKeyHandler::handlePageDown(QKeyEvent* _event)
{
	QTextCursor cursor = editor()->textCursor();
	cursor.beginEditBlock();

	for (int line = 0; line < 20; ++line) {
		handleDown(_event);
	}

	cursor.endEditBlock();
}

void StandardKeyHandler::handleOther(QKeyEvent*)
{

}


// **** private ****


void StandardKeyHandler::removeCharacters(bool _backward)
{
	QTextCursor cursor = editor()->textCursor();

	if (!cursor.hasSelection()) {
		//
		// Если в начале документа нажат backspace
		//
		if (cursor.atStart()
			&& _backward == true) {
			return;
		}

		//
		// Если в конце документа нажат delete
		//
		if (cursor.atEnd()
			&& _backward == false) {
			return;
		}
	}

	//
	// Начинаем операцию удаления
	//
	cursor.beginEditBlock();

	//
	// Определим границы выделения
	//
	// ... верхнюю
	//
	int topCursorPosition = 0;
	{
		if (cursor.hasSelection()) {
			topCursorPosition = qMin(cursor.selectionStart(), cursor.selectionEnd());
		} else {
			topCursorPosition = cursor.position() - (_backward ? 1 : 0);
		}
	}
	//
	// ... и нижнюю
	//
	int bottomCursorPosition = 0;
	{
		if (cursor.hasSelection()) {
			bottomCursorPosition = qMax(cursor.selectionStart(), cursor.selectionEnd());
		} else {
			bottomCursorPosition = cursor.position() + (_backward ? 0 : 1);
		}
	}

	//
	// Получим стили блоков начала и конца выделения
	//
	// ... начала
	//
	ScenarioBlockStyle topStyle = ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::Undefined);
	QTextBlock topBlock;
	{
		QTextCursor topCursor(editor()->document());
		topCursor.setPosition(topCursorPosition);
		topStyle = ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::forBlock(topCursor.block()));
		topBlock = topCursor.block();
	}
	//
	// ... и конца
	//
	ScenarioBlockStyle bottomStyle = ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::Undefined);
	QTextBlock bottomBlock;
	{
		QTextCursor bottomCursor(editor()->document());
		bottomCursor.setPosition(bottomCursorPosition);
		bottomStyle = ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::forBlock(bottomCursor.block()));
		bottomBlock = bottomCursor.block();


	}

	//
	// Если удаление затрагивает несколько блоков или удаление происходит в заголовочном блоке,
	// возможно понадобится расширить область выделения
	//
	if (topBlock != bottomBlock
		|| topStyle.isHeader()) {
		//
		// Если верхний блок является заголовком - расширим выделение до следующего блока, сдвигая курсор влево
		//
		if (topStyle.isHeader())
		{
			QTextCursor topCursor(editor()->document());
			topCursor.setPosition(topCursorPosition);

			while (topBlock == topCursor.block()
				   && !topCursor.atStart()) {
				topCursor.movePosition(QTextCursor::Left);
				topStyle = ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::forBlock(topCursor.block()));
			}

			topCursorPosition = topCursor.position();
		}

		//
		// Если нижний блок является заголовком - расширим выделение, сдвигая курсор вправо
		//
		if (bottomStyle.isHeader())
		{
			QTextCursor bottomCursor(editor()->document());
			bottomCursor.setPosition(bottomCursorPosition);

			while (bottomBlock == bottomCursor.block()
				   && !bottomCursor.atEnd()) {
				bottomCursor.movePosition(QTextCursor::Right);
				bottomStyle = ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::forBlock(bottomCursor.block()));
			}

			bottomCursorPosition = bottomCursor.position();
		}
	}

	//
	// Определим стиль результирующего блока
	//
	ScenarioBlockStyle::Type targetType = ScenarioBlockStyle::TimeAndPlace;
	{
		if (topBlock == bottomBlock) {
			targetType = topStyle.type();
		} else {
			if (!topBlock.text().isEmpty()) {
				targetType = topStyle.type();
			} else if (!bottomBlock.text().isEmpty()) {
				targetType = bottomStyle.type();
			}
		}
	}

	//
	// Собственно удаление
	//
	{
		//
		// Подсчитать количество группирующих элементов входящих в выделение
		//
		QList<int> groupsToDeleteCounts = findGroupCountsToDelete(topCursorPosition, bottomCursorPosition);

		//
		// Удалить текст
		//
		cursor.setPosition(topCursorPosition);
		cursor.setPosition(bottomCursorPosition, QTextCursor::KeepAnchor);
		cursor.removeSelectedText();

		//
		// Удалить вторые половинки группирующих элементов
		//
		removeGroupsPairs(cursor.position(), groupsToDeleteCounts);
	}

	//
	// Применим финальный стиль
	//
	editor()->applyScenarioTypeToBlockText(targetType);

	//
	// Завершим операцию удаления
	//
	cursor.endEditBlock();
}

namespace {
	const int SCENE_GROUP_HEADER = 0;
	const int SCENE_GROUP_FOOTER = 1;
	const int FOLDER_HEADER = 2;
	const int FOLDER_FOOTER = 3;
}

QList<int> StandardKeyHandler::findGroupCountsToDelete(int _topCursorPosition, int _bottomCursorPosition)
{
	QList<int> groupCountsToDelete;
	groupCountsToDelete << 0 << 0 << 0 << 0;

	//
	// Начнём поиск с заданной позиции
	//
	QTextCursor searchGroupsCursor(editor()->document());
	searchGroupsCursor.setPosition(_topCursorPosition);

	//
	// Если стартовая позиция не в начале блока, перейдём к следующему блоку,
	// т.к. нас интересуют полные вхождения блоков
	//
	if (!searchGroupsCursor.atBlockStart()) {
		searchGroupsCursor.movePosition(QTextCursor::NextBlock);
	}

	//
	// Позиция конца блока
	//
	int endSearchBlockPosition = searchGroupsCursor.position() + searchGroupsCursor.block().length() - 1;

	while (endSearchBlockPosition <= _bottomCursorPosition
		   && !searchGroupsCursor.atEnd()) {
		//
		// Для удаления группы может быть захвачен символ как сверху, так и снизу
		//
		if ((searchGroupsCursor.position() - 1 >= _topCursorPosition)
			|| (endSearchBlockPosition + 1 <= _bottomCursorPosition)) {
			//
			// Определим тип блока
			//
			ScenarioBlockStyle::Type currentType =
					ScenarioBlockStyle::forBlock(searchGroupsCursor.block());

			//
			// Если найден блок открывающий группу, то нужно удалить закрывающий блок
			//
			if (currentType == ScenarioBlockStyle::SceneGroupHeader) {
				++groupCountsToDelete[SCENE_GROUP_FOOTER];
			} else if (currentType == ScenarioBlockStyle::FolderHeader) {
				++groupCountsToDelete[FOLDER_FOOTER];
			}

			//
			// Если найден блок закрывающий группу
			// ... если все группы закрыты, нужно удалить предыдущую открытую
			// ... в противном случае закрываем открытую группу
			//
			else if (currentType == ScenarioBlockStyle::SceneGroupFooter) {
				if (groupCountsToDelete.value(SCENE_GROUP_FOOTER) == 0) {
					++groupCountsToDelete[SCENE_GROUP_HEADER];
				}
				else {
					--groupCountsToDelete[SCENE_GROUP_FOOTER];
				}
			} else if (currentType == ScenarioBlockStyle::FolderFooter) {
				if (groupCountsToDelete.value(FOLDER_FOOTER) == 0) {
					++groupCountsToDelete[FOLDER_HEADER];
				}
				else {
					--groupCountsToDelete[FOLDER_FOOTER];
				}
			}
		}

		//
		// Перейдём к следующему блоку или концу блока
		//
		if (!searchGroupsCursor.movePosition(QTextCursor::NextBlock)) {
			searchGroupsCursor.movePosition(QTextCursor::EndOfBlock);
		}
		endSearchBlockPosition = searchGroupsCursor.position() + searchGroupsCursor.block().length() - 1;
	}

	return groupCountsToDelete;
}

void StandardKeyHandler::removeGroupsPairs(int _cursorPosition, const QList<int>& _groupCountsToDelete)
{
	//
	// Удалим пары из последующего текста
	//
	// ... группы сцен
	//
	if (_groupCountsToDelete.value(SCENE_GROUP_FOOTER) > 0) {
		QTextCursor cursor(editor()->document());
		cursor.setPosition(_cursorPosition);
		cursor.movePosition(QTextCursor::NextBlock);

		// ... открытые группы на пути поиска необходимого для удаления блока
		int openedGroups = 0;
		int groupsToDeleteCount = _groupCountsToDelete.value(SCENE_GROUP_FOOTER);
		do {
			ScenarioBlockStyle::Type currentType =
					ScenarioBlockStyle::forBlock(cursor.block());

			if (currentType == ScenarioBlockStyle::SceneGroupFooter) {
				if (openedGroups == 0) {
					cursor.select(QTextCursor::BlockUnderCursor);
					cursor.deleteChar();
					--groupsToDeleteCount;
				} else {
					--openedGroups;
				}
			} else if (currentType == ScenarioBlockStyle::SceneGroupHeader) {
				// ... встретилась новая группа, которую не нужно удалять
				++openedGroups;
			}

			cursor.movePosition(QTextCursor::NextBlock);
			cursor.movePosition(QTextCursor::EndOfBlock);
		} while (groupsToDeleteCount > 0
				 && !cursor.atEnd());
	}
	//
	// ... папки
	//
	if (_groupCountsToDelete.value(FOLDER_FOOTER) > 0) {
		QTextCursor cursor(editor()->document());
		cursor.setPosition(_cursorPosition);
		cursor.movePosition(QTextCursor::NextBlock);

		// ... открытые группы на пути поиска необходимого для удаления блока
		int openedGroups = 0;
		int groupsToDeleteCount = _groupCountsToDelete.value(FOLDER_FOOTER);
		do {
			ScenarioBlockStyle::Type currentType =
					ScenarioBlockStyle::forBlock(cursor.block());

			if (currentType == ScenarioBlockStyle::FolderFooter) {
				if (openedGroups == 0) {
					cursor.select(QTextCursor::BlockUnderCursor);
					cursor.deleteChar();
					--groupsToDeleteCount;
				} else {
					--openedGroups;
				}
			} else if (currentType == ScenarioBlockStyle::FolderHeader) {
				// ... встретилась новая группа, которую не нужно удалять
				++openedGroups;
			}

			cursor.movePosition(QTextCursor::NextBlock);
			cursor.movePosition(QTextCursor::EndOfBlock);
		} while (groupsToDeleteCount > 0
				 && !cursor.atEnd());
	}

	//
	// Удалим пары из предшествующего текста
	//
	// ... группы сцен
	//
	if (_groupCountsToDelete.value(SCENE_GROUP_HEADER) > 0) {
		QTextCursor cursor = editor()->textCursor();
		cursor.setPosition(_cursorPosition);
		cursor.movePosition(QTextCursor::PreviousBlock);

		// ... открытые группы на пути поиска необходимого для удаления блока
		int openedGroups = 0;
		int groupsToDeleteCount = _groupCountsToDelete.value(SCENE_GROUP_HEADER);
		do {
			ScenarioBlockStyle::Type currentType =
					ScenarioBlockStyle::forBlock(cursor.block());

			if (currentType == ScenarioBlockStyle::SceneGroupHeader) {
				if (openedGroups == 0) {
					cursor.select(QTextCursor::BlockUnderCursor);
					cursor.deleteChar();

					//
					// Если это был самый первый блок
					//
					if (cursor.atStart()) {
						cursor.deleteChar();
					}

					--groupsToDeleteCount;
				} else {
					--openedGroups;
				}
			} else if (currentType == ScenarioBlockStyle::SceneGroupFooter) {
				// ... встретилась новая группа, которую не нужно удалять
				++openedGroups;
			}

			cursor.movePosition(QTextCursor::PreviousBlock);
		} while (groupsToDeleteCount > 0
				 && !cursor.atStart());
	}
	//
	// ... папки
	//
	if (_groupCountsToDelete.value(FOLDER_HEADER) > 0) {
		QTextCursor cursor = editor()->textCursor();
		cursor.setPosition(_cursorPosition);
		cursor.movePosition(QTextCursor::PreviousBlock);

		// ... открытые группы на пути поиска необходимого для удаления блока
		int openedGroups = 0;
		int groupsToDeleteCount = _groupCountsToDelete.value(FOLDER_HEADER);
		do {
			ScenarioBlockStyle::Type currentType =
					ScenarioBlockStyle::forBlock(cursor.block());

			if (currentType == ScenarioBlockStyle::FolderHeader) {
				if (openedGroups == 0) {
					cursor.select(QTextCursor::BlockUnderCursor);
					cursor.deleteChar();

					//
					// Если это был самый первый блок
					//
					if (cursor.atStart()) {
						cursor.deleteChar();
					}

					--groupsToDeleteCount;
				} else {
					--openedGroups;
				}
			} else if (currentType == ScenarioBlockStyle::FolderFooter) {
				// ... встретилась новая группа, которую не нужно удалять
				++openedGroups;
			}

			cursor.movePosition(QTextCursor::PreviousBlock);
		} while (groupsToDeleteCount > 0
				 && !cursor.atStart());
	}
}
