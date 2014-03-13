#include "StandardKeyHandler.h"

#include "../ScenarioTextEdit.h"

#include <QKeyEvent>
#include <QTextBlock>

using namespace KeyProcessingLayer;
using namespace BusinessLogic;
using UserInterface::ScenarioTextEdit;


StandardKeyHandler::StandardKeyHandler(ScenarioTextEdit* _editor) :
	AbstractKeyHandler(_editor)
{
}

void StandardKeyHandler::handleShortcut(QKeyEvent* _event)
{
	//
	// Получим необходимые переменные
	//
	// ... зажатые управляющие клавиши
	Qt::KeyboardModifiers pressedModifiers = _event->modifiers();

	if (pressedModifiers.testFlag(Qt::ControlModifier)) {
		//
		// Получим код нажатой кнопки
		//
		Qt::Key realKey = (Qt::Key)keyCharacterToQtKey(_event->text());
		if (realKey == Qt::Key_unknown) {
			realKey = (Qt::Key)_event->key();
		}

		//
		// Проверяем по коду клавиатуры, этот способ независим от выбранного пользователем языка
		//
		switch (realKey) {
			/**
			 * @note Действия редактирования текста (дублирующие контектстное меню)
			 */
			/** @{ */
			case Qt::Key_A: {
				editor()->selectAll();
				break;
			}

			case Qt::Key_Z: {
				if (pressedModifiers.testFlag(Qt::ShiftModifier)) {
					editor()->redo();
				} else {
					editor()->undo();
				}

				emit editor()->currentStyleChanged();

				break;
			}

			case Qt::Key_X: {
				editor()->copy();
				handleDelete();
				break;
			}

			case Qt::Key_C: {
				editor()->copy();
				break;
			}

			case Qt::Key_V: {
				editor()->paste();
				break;
			}
			/** @} */

			/**
			 * @note Действия смены стиля
			 *
				Время и место: Ctrl+Enter
				Описание действия: Ctrl+J (на русской раскладке это Ctrl+О)
				Персонаж: Ctrl+U (на русской раскладке это Ctrl+Г (герой))
				Диалог: Ctrl+L (на русской раскладке это Ctrl+Д)
				Ремарка: Ctrl+H (на русской раскладке это Ctrl+Р)
				Титр: Ctrl+N (на русской раскладке это Ctrl+Т)
				Примечание: Ctrl+P (на русской раскладке это Ctrl+З (заметка))
				Переход: Ctrl+G (на русской раскладке это Ctrl+П)
				Непечатный текст: Ctrl+Y (на русской Ctrl+Н)
				Группа сцен: Ctrl+D (на русской раскладке это Ctrl+В (вставка))
				Папка: Ctrl+Пробел
			 *
			 */
			/** @{ */
			case Qt::Key_Enter:
			case Qt::Key_Return: {
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::TimeAndPlace);
				break;
			}

			case Qt::Key_J: {
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Action);
				break;
			}

			case Qt::Key_U: {
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Character);
				break;
			}

			case Qt::Key_L: {
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Dialog);
				break;
			}

			case Qt::Key_H: {
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Parenthetical);
				break;
			}

			case Qt::Key_N: {
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Title);
				break;
			}

			case Qt::Key_P: {
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Note);
				break;
			}

			case Qt::Key_G: {
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::Transition);
				break;
			}

			case Qt::Key_Y: {
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::NoprintableText);
				break;
			}

			case Qt::Key_D: {
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::SceneGroupHeader);
				break;
			}

			case Qt::Key_Space: {
				editor()->changeScenarioBlockType(ScenarioTextBlockStyle::FolderHeader);
				break;
			}
			/** @} */

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
	ScenarioTextBlockStyle topStyle(ScenarioTextBlockStyle::Undefined);
	{
		QTextCursor topCursor(editor()->document());
		topCursor.setPosition(topCursorPosition);
		topStyle.setType(ScenarioTextBlockStyle::forBlock(topCursor.block()));

		//
		// Если блок является заголовком - расширим выделение, сдвигая курсор влево
		//
		while (topStyle.isHeader()
			   && !topCursor.atStart()) {
			topCursor.movePosition(QTextCursor::Left);
			topStyle.setType(ScenarioTextBlockStyle::forBlock(topCursor.block()));
		}

		topCursorPosition = topCursor.position();
	}
	//
	// ... и конца
	//
	ScenarioTextBlockStyle bottomStyle(ScenarioTextBlockStyle::Undefined);
	{
		QTextCursor bottomCursor(editor()->document());
		bottomCursor.setPosition(bottomCursorPosition);
		bottomStyle.setType(ScenarioTextBlockStyle::forBlock(bottomCursor.block()));

		//
		// Если блок является заголовком - расширим выделение, сдвигая курсор вправо
		//
		while (bottomStyle.isHeader()
			   && !bottomCursor.atEnd()) {
			bottomCursor.movePosition(QTextCursor::Right);
			bottomStyle.setType(ScenarioTextBlockStyle::forBlock(bottomCursor.block()));
		}

		bottomCursorPosition = bottomCursor.position();
	}

	//
	// Расширить выделение, если в выделении разные блоки и в одном из них группирующий элемент
	//
	{
		QTextCursor topCursor(editor()->document());
		topCursor.setPosition(topCursorPosition);

		QTextCursor bottomCursor(editor()->document());
		bottomCursor.setPosition(bottomCursorPosition);

		//
		// Если разные блоки
		//
		if (topCursor.block() != bottomCursor.block()) {
			//
			// Если необходимо расширим верхний
			//
			if (topStyle.isEmbeddable()
				&& !bottomCursor.block().text().isEmpty()) {
				//
				// Если блок является группирующим - расширим выделение, сдвигая курсор влево
				//
				while (topStyle.isEmbeddable()
					   && !topCursor.atStart()) {
					topCursor.movePosition(QTextCursor::Left);
					topStyle.setType(ScenarioTextBlockStyle::forBlock(topCursor.block()));
				}

				topCursorPosition = topCursor.position();

				if (!cursor.atStart()) {
					++topCursorPosition;
				}
			}

			//
			// Если необходимо расширим нижний
			//
			if (bottomStyle.isEmbeddable()
				&& !topCursor.block().text().isEmpty()) {
				//
				// Если блок является группирующим - расширим выделение, сдвигая курсор вправо
				//
				while (bottomStyle.isEmbeddable()
					   && !bottomCursor.atEnd()) {
					bottomCursor.movePosition(QTextCursor::Right);
					bottomStyle.setType(ScenarioTextBlockStyle::forBlock(bottomCursor.block()));
				}

				bottomCursorPosition = bottomCursor.position();

				if (!bottomCursor.atEnd()) {
					--bottomCursorPosition;
				}
			}
		}
	}

	//
	// Определим стиль результирующего блока
	//
	ScenarioTextBlockStyle::Type targetType = ScenarioTextBlockStyle::Undefined;
	{
		//
		// Если начало документа, то время и место
		//
		if (topCursorPosition == 0) {
			if (bottomCursorPosition != 1) {
				targetType = ScenarioTextBlockStyle::TimeAndPlace;
			}
		}
		//
		// Если не начало документа
		//
		else {
			QTextCursor checkCursor(editor()->document());
			checkCursor.setPosition(topCursorPosition);

			//
			// Если блок начала выделения пуст, то стиль конечного блока
			//
			if (checkCursor.block().text().simplified().isEmpty()) {
				targetType = bottomStyle.blockType();
			}
			//
			// В противном случае - стиль начального блока
			//
			else {
				targetType = topStyle.blockType();
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
		// Определим тип блока
		//
		ScenarioTextBlockStyle::Type currentType =
				ScenarioTextBlockStyle::forBlock(searchGroupsCursor.block());

		//
		// Если найден блок открывающий группу, то нужно удалить закрывающий блок
		//
		if (currentType == ScenarioTextBlockStyle::SceneGroupHeader) {
			++groupCountsToDelete[SCENE_GROUP_FOOTER];
		} else if (currentType == ScenarioTextBlockStyle::FolderHeader) {
			++groupCountsToDelete[FOLDER_FOOTER];
		}

		//
		// Если найден блок закрывающий группу
		// ... если все группы закрыты, нужно удалить предыдущую открытую
		// ... в противном случае закрываем открытую группу
		//
		else if (currentType == ScenarioTextBlockStyle::SceneGroupFooter) {
			if (groupCountsToDelete.value(SCENE_GROUP_FOOTER) == 0) {
				++groupCountsToDelete[SCENE_GROUP_HEADER];
			}
			else {
				--groupCountsToDelete[SCENE_GROUP_FOOTER];
			}
		} else if (currentType == ScenarioTextBlockStyle::FolderFooter) {
			if (groupCountsToDelete.value(FOLDER_FOOTER) == 0) {
				++groupCountsToDelete[FOLDER_HEADER];
			}
			else {
				--groupCountsToDelete[FOLDER_FOOTER];
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
			ScenarioTextBlockStyle::Type currentType =
					ScenarioTextBlockStyle::forBlock(cursor.block());

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
			ScenarioTextBlockStyle::Type currentType =
					ScenarioTextBlockStyle::forBlock(cursor.block());

			if (currentType == ScenarioTextBlockStyle::FolderFooter) {
				if (openedGroups == 0) {
					cursor.select(QTextCursor::BlockUnderCursor);
					cursor.deleteChar();
					--groupsToDeleteCount;
				} else {
					--openedGroups;
				}
			} else if (currentType == ScenarioTextBlockStyle::FolderHeader) {
				// ... встретилась новая группа, которую не нужно удалять
				++openedGroups;
			}

			cursor.movePosition(QTextCursor::NextBlock);
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
			ScenarioTextBlockStyle::Type currentType =
					ScenarioTextBlockStyle::forBlock(cursor.block());

			if (currentType == ScenarioTextBlockStyle::SceneGroupHeader) {
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
			} else if (currentType == ScenarioTextBlockStyle::SceneGroupFooter) {
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
			ScenarioTextBlockStyle::Type currentType =
					ScenarioTextBlockStyle::forBlock(cursor.block());

			if (currentType == ScenarioTextBlockStyle::FolderHeader) {
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
			} else if (currentType == ScenarioTextBlockStyle::FolderFooter) {
				// ... встретилась новая группа, которую не нужно удалять
				++openedGroups;
			}

			cursor.movePosition(QTextCursor::PreviousBlock);
		} while (groupsToDeleteCount > 0
				 && !cursor.atStart());
	}
}

int StandardKeyHandler::keyCharacterToQtKey(const QString& _keyCharacter) const
{
	int qtKey = Qt::Key_unknown;

	//
	// Если это не одиночный символ, то не возможно определить код клавиши
	//
	if (_keyCharacter.length() == 1) {
		QString keyCharacter = _keyCharacter.toLower();
		if (keyCharacter == "a"
			|| keyCharacter == QString::fromUtf8("ф")) {
			qtKey = Qt::Key_A;
		} else if (keyCharacter == "z"
				   || keyCharacter == QString::fromUtf8("я")) {
			qtKey = Qt::Key_Z;
		} else if (keyCharacter == "x"
				   || keyCharacter == QString::fromUtf8("ч")) {
			qtKey = Qt::Key_X;
		} else if (keyCharacter == "c"
				   || keyCharacter == QString::fromUtf8("с")) {
			qtKey = Qt::Key_C;
		} else if (keyCharacter == "v"
				   || keyCharacter == QString::fromUtf8("м")) {
			qtKey = Qt::Key_V;
		} else if (keyCharacter == "j"
				   || keyCharacter == QString::fromUtf8("о")) {
			qtKey = Qt::Key_J;
		} else if (keyCharacter == "u"
				   || keyCharacter == QString::fromUtf8("г")) {
			qtKey = Qt::Key_U;
		} else if (keyCharacter == "l"
				   || keyCharacter == QString::fromUtf8("д")) {
			qtKey = Qt::Key_L;
		} else if (keyCharacter == "h"
				   || keyCharacter == QString::fromUtf8("р")) {
			qtKey = Qt::Key_H;
		} else if (keyCharacter == "n"
				   || keyCharacter == QString::fromUtf8("т")) {
			qtKey = Qt::Key_N;
		} else if (keyCharacter == "p"
				   || keyCharacter == QString::fromUtf8("з")) {
			qtKey = Qt::Key_P;
		} else if (keyCharacter == "g"
				   || keyCharacter == QString::fromUtf8("п")) {
			qtKey = Qt::Key_G;
		} else if (keyCharacter == "d"
				   || keyCharacter == QString::fromUtf8("в")) {
			qtKey = Qt::Key_D;
		} else if (keyCharacter == "y"
				   || keyCharacter == QString::fromUtf8("н")) {
			qtKey = Qt::Key_Y;
		}
	}

	return qtKey;
}
