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

void ScenarioTextEdit::setScenarioBlockType(ScenarioTextBlockStyle::Type _blockType)
{
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
}

ScenarioTextBlockStyle::Type ScenarioTextEdit::scenarioBlockType(const QTextBlock& _block)
{
	return (ScenarioTextBlockStyle::Type)_block.blockFormat().intProperty(QTextFormat::UserProperty);
}



//*************************************************

void ScenarioTextEdit::test()
{
	QTextBlock block = textCursor().block();
	qDebug() << block.userState() << block.blockFormat().intProperty(QTextFormat::UserProperty);

	QTextBlock::iterator it;
	for (it = block.begin(); !(it.atEnd()); ++it) {
		QTextFragment currentFragment = it.fragment();
		if (currentFragment.isValid()) {
			qDebug() << currentFragment.text();
		}
	}
}
