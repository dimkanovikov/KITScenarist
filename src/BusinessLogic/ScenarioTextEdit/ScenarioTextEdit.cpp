#include "ScenarioTextEdit.h"

#include "Handlers/KeyPressHandlerFacade.h"

#include <QTextCursor>
#include <QTextBlock>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QAbstractTextDocumentLayout>
#include <QTimer>

#include <QDebug>
#include <QDateTime>
#include <QCompleter>
#include <QStringListModel>
#include <QAbstractItemView>
#include <QScrollBar>


ScenarioTextEdit::ScenarioTextEdit(QWidget* _parent) :
	CompletableTextEdit(_parent)
{
	//
	// Параметры стиля страницы
	//
	const int PAGE_FONT_SIZE = 12;
	const QFont PAGE_FONT("Courier New", PAGE_FONT_SIZE);
	const int PAGE_WIDTH = 60;
	const int PAGE_HEIGHT = 50;
	const QSizeF PAGE_SIZE(PAGE_WIDTH, PAGE_HEIGHT);
	const int PAGE_MARGIN = 10;

	//
	// Настраиваем редактор
	//
	setFont(PAGE_FONT);

	//
	// Создаём и настраиваем документ редактора
	//
	document()->setDefaultFont(PAGE_FONT);
	document()->setDocumentMargin(PAGE_MARGIN);
	document()->setPageSize(PAGE_SIZE);

	//
	// Финт ушами для того, чтобы применить UpperCase
	//
	textCursor().insertBlock();
	textCursor().deletePreviousChar();
	setScenarioBlockType(ScenarioTextBlockStyle::SceneHeader);
}

void ScenarioTextEdit::setScenarioBlockType(ScenarioTextBlockStyle::Type _blockType)
{
	int verticalScrollPosition = verticalScrollBar()->value();

	QTextCursor cursor = textCursor();

	//
	// Применим стиль к блоку
	//
	ScenarioTextBlockStyle blockStyle(_blockType);
	cursor.setBlockCharFormat(blockStyle.charFormat());
	cursor.setBlockFormat(blockStyle.blockFormat());

	//
	// Применим стиль текста ко всему блоку, выделив его,
	// т.к. в блоке могут находиться фрагменты в другом стиле
	//
	cursor.select(QTextCursor::BlockUnderCursor);
	cursor.setCharFormat(blockStyle.charFormat());
	cursor.clearSelection();

	//
	// Вставим префикс и постфикс стиля, если необходимо
	//
	if (blockStyle.hasDecoration()) {
		int cursorPosition = cursor.position();
		QString blockText = cursor.block().text();
		if (!blockStyle.postfix().isEmpty()
			&& !blockText.startsWith(blockStyle.prefix())) {
			cursor.movePosition(QTextCursor::StartOfBlock);
			cursor.insertText(blockStyle.prefix());
			cursorPosition += blockStyle.prefix().length();
		}
		if (!blockStyle.prefix().isEmpty()
			&& !blockText.endsWith(blockStyle.postfix())) {
			cursor.movePosition(QTextCursor::EndOfBlock);
			cursor.insertText(blockStyle.postfix());
		}
		cursor.setPosition(cursorPosition);
		setTextCursor(cursor);
	}

	verticalScrollBar()->setValue(verticalScrollPosition + 100);
}

ScenarioTextBlockStyle::Type ScenarioTextEdit::scenarioBlockType(const QTextBlock& _block)
{
	return (ScenarioTextBlockStyle::Type)_block.blockFormat().intProperty(ScenarioTextBlockStyle::PropertyType);
}

void ScenarioTextEdit::keyPressEvent(QKeyEvent* _event)
{
	//
	// Получим обработчик
	//
	KeyProcessingLayer::KeyPressHandlerFacade* handler =
			KeyProcessingLayer::KeyPressHandlerFacade::instance(this);

	//
	// Получим курсор в текущем положении
	//
	QTextCursor cursor = textCursor();

	//
	// Начнём блок операций
	//
	cursor.beginEditBlock();

	//
	// Подготовка к обработке
	//
	handler->prepare(_event);

	//
	// Предварительная обработка
	//
	handler->prehandle(_event);

	//
	// Отправить событие в базовый класс
	//
	if (handler->needSendEventToBaseClass()) {
		SpellCheckTextEdit::keyPressEvent(_event);
		makeTextUpper(_event);
	}

	//
	// Обработка
	//
	handler->handle(_event);

	//
	// Событие дошло по назначению
	//
	_event->accept();

	//
	// Завершим блок операций
	//
	cursor.endEditBlock();

	//
	// Убедимся, что курсор виден
	//
	ensureCursorVisible();
}

void ScenarioTextEdit::wheelEvent(QWheelEvent* _event)
{
	if (_event->orientation() == Qt::Vertical
		&& (_event->modifiers() & Qt::ControlModifier)) {
		//
		// zoomRange > 0 - Текст увеличивается
		// zoomRange < 0 - Текст уменьшается
		//
		int zoomRange = (_event->delta() / 120) * 2;
		zoomIn(zoomRange);

		_event->accept();
	} else {
		QTextEdit::wheelEvent(_event);
	}
}

void ScenarioTextEdit::makeTextUpper(QKeyEvent* _event)
{
	//
	// Получим значения
	//
	// ... курсора
	QTextCursor cursor = textCursor();
	// ... блок текста в котором находится курсор
	QTextBlock currentBlock = cursor.block();
	// ... текст блока
	QString currentBlockText = currentBlock.text();
	// ... текст до курсора
	QString cursorBackwardText = currentBlockText.left(cursor.positionInBlock());
	// ... стиль шрифта блока
	QTextCharFormat currentCharFormat = currentBlock.charFormat();
	// ... текст события
	QString eventText = _event->text();

	//
	// Если был введён текст
	//
	if (!eventText.isEmpty()) {
		//
		// Определяем необходимость установки верхнего регистра для первого символа
		//
		if (cursorBackwardText == eventText
			|| cursorBackwardText == (currentCharFormat.stringProperty(ScenarioTextBlockStyle::PropertyPrefix)
									  + eventText)) {
			//
			// Сформируем правильное представление строки
			//
			bool isFirstUpperCase = currentCharFormat.boolProperty(ScenarioTextBlockStyle::PropertyIsFirstUppercase);
			QString correctedText = eventText;
			if (isFirstUpperCase) {
				correctedText[0] = correctedText[0].toUpper();
			} else {
				correctedText[0] = correctedText[0].toLower();
			}

			//
			// Стираем предыдущий введённый текст
			//
			for (int repeats = 0; repeats < eventText.length(); ++repeats) {
				cursor.deletePreviousChar();
			}

			//
			// Выводим необходимый
			//
			cursor.insertText(correctedText);
			setTextCursor(cursor);
		}

		//
		// Делаем буквы в начале предложения заглавными
		//
		else {
			//
			// Если перед нами конец предложения и не сокращение
			//
			QString endOfSentancePattern = QString("([.]|[?]|[!]) %1$").arg(eventText);
			if (cursorBackwardText.contains(QRegularExpression(endOfSentancePattern))
				&& !stringEndsWithAbbrev(cursorBackwardText)) {
				//
				// Сделаем первую букву заглавной
				//
				QString correctedText = eventText;
				correctedText[0] = correctedText[0].toUpper();

				//
				// Стираем предыдущий введённый текст
				//
				for (int repeats = 0; repeats < eventText.length(); ++repeats) {
					cursor.deletePreviousChar();
				}

				//
				// Выводим необходимый
				//
				cursor.insertText(correctedText);
				setTextCursor(cursor);
			}
		}
	}

}

bool ScenarioTextEdit::stringEndsWithAbbrev(const QString& _text)
{
	//
	// FIXME проработать словарь сокращений
	//

	return false;
}



//*************************************************

void ScenarioTextEdit::test()
{
	QTextBlock block = textCursor().block();
	qDebug() << block.userState() << block.blockFormat().intProperty(ScenarioTextBlockStyle::PropertyType);

	QTextBlock::iterator it;
	for (it = block.begin(); !(it.atEnd()); ++it) {
		QTextFragment currentFragment = it.fragment();
		if (currentFragment.isValid()) {
			qDebug() << currentFragment.text();
		}
	}
}
