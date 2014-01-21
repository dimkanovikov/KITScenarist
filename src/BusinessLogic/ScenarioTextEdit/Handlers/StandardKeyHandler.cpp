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
	// ... тип, который будет установлен после удаления блоку
	ScenarioTextBlockStyle::Type nextType = topStyle.blockType();
	// ... если блок является заголовком - расширим выделение
	while (topStyle.isHeader()) {
		topCursor.movePosition(QTextCursor::Left);
		topStyle.setType(editor()->scenarioBlockType(topCursor.block()));

		if (topCursor.atStart()) {
			nextType = ScenarioTextBlockStyle::TimeAndPlace;
			break;
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
	while (bottomStyle.isHeader()
		   && !bottomCursor.atEnd()) {
		bottomCursor.movePosition(QTextCursor::Right);
		bottomStyle.setType(editor()->scenarioBlockType(bottomCursor.block()));
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
			if (bottomStyle.blockType() == ScenarioTextBlockStyle::FolderHeader) {
				++groupFootersToDeleteCount;
			} else if (bottomStyle.blockType() == ScenarioTextBlockStyle::FolderFooter) {
				++groupHeadersToDeleteCount;
			}
		}
	}
	//
	// Иначе расширим выделение
	//
	else {
		//
		// ... сверху
		//
		bool wasInsideCycle = false;
		while ((topStyle.blockType() == ScenarioTextBlockStyle::FolderFooter
				|| topStyle.blockType() == ScenarioTextBlockStyle::FolderHeader)
			   && !topCursor.atStart()) {
			topCursor.movePosition(QTextCursor::Left);
			topStyle.setType(editor()->scenarioBlockType(topCursor.block()));
			wasInsideCycle = true;
		}
		// ... если не дошли до начала документа, нужно вернуться на один символ назад
		/*if (!topCursor.atStart() && wasInsideCycle) {
			topCursor.movePosition(QTextCursor::Right);
		} else*/ if (topCursor.atStart()) {
			nextType = ScenarioTextBlockStyle::TimeAndPlace;
		}
		//
		// ... и снизу
		//
		wasInsideCycle = false;
		while ((bottomStyle.blockType() == ScenarioTextBlockStyle::FolderFooter
				|| bottomStyle.blockType() == ScenarioTextBlockStyle::FolderHeader)
			   && !bottomCursor.atEnd()) {
			bottomCursor.movePosition(QTextCursor::Right);
			bottomStyle.setType(editor()->scenarioBlockType(bottomCursor.block()));
			wasInsideCycle = true;
		}
		// ... если не дошли до конца документа, нужно вернуться на один символ назад
		if (!bottomCursor.atEnd()
			&& wasInsideCycle
			&& !bottomCursor.block().text().isEmpty()) {
			bottomCursor.movePosition(QTextCursor::Left);
		}

		//
		// ... а затем обновим выделение
		//
		cursor.setPosition(topCursor.position());
		cursor.setPosition(bottomCursor.position(), QTextCursor::KeepAnchor);
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
	editor()->applyScenarioTypeToBlockText(nextType);
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
	searchGroupsCursor.movePosition(QTextCursor::NextBlock);

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
			if (currentType == ScenarioTextBlockStyle::FolderHeader) {
				// ... нужно удалить закрывающий блок
				++_groupFootersToRemoveCount;
			}

			//
			// Найден блок закрывающий группу
			//
			else if (currentType == ScenarioTextBlockStyle::FolderFooter) {
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
			if (searchGroupsCursor.atEnd()
				|| searchGroupsCursor.blockNumber() == searchGroupsCursor.document()->blockCount()-1) {
				break;
			}

			//
			// Перейдём к следующему блоку
			//
			searchGroupsCursor.movePosition(QTextCursor::NextBlock);
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
					 && cursor.blockNumber() != (cursor.document()->blockCount()-1));
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
					 && cursor.blockNumber() != 0);
		}
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
			qtKey = Qt::Key_S;
		} else if (keyCharacter == "u"
				   || keyCharacter == QString::fromUtf8("г")) {
			qtKey = Qt::Key_D;
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
