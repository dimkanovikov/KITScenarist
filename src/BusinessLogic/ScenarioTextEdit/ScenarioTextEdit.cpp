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
#include <QMimeData>


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
	setScenarioBlockType(ScenarioTextBlockStyle::TimeAndPlace);

	//
	// При перемещении курсора может меняться стиль блока
	//
	connect(this, SIGNAL(cursorPositionChanged()), this, SIGNAL(currentStyleChanged()));
}

void ScenarioTextEdit::setScenarioBlockType(ScenarioTextBlockStyle::Type _blockType)
{
	//
	// Является ли текущий вид заголовком
	//
	bool currentTypeIsHeader =
			scenarioBlockType() == ScenarioTextBlockStyle::TitleHeader;

	//
	// Если текущий вид не заголовок
	//
	if (!currentTypeIsHeader) {

		//
		// Обработаем предшествующий установленный стиль
		//
		{
			QTextCursor cursor = textCursor();
			ScenarioTextBlockStyle oldBlockStyle(scenarioBlockType());

			//
			// Удалить заголовок если происходит смена стиля в текущем блоке
			// в противном случае его не нужно удалять
			//
			if (oldBlockStyle.hasHeader()) {
				QTextCursor headerCursor = cursor;
				headerCursor.movePosition(QTextCursor::StartOfBlock);
				headerCursor.movePosition(QTextCursor::Left);
				if (scenarioBlockType(headerCursor.block())
					== oldBlockStyle.headerType()) {
					headerCursor.select(QTextCursor::BlockUnderCursor);
					headerCursor.deleteChar();

					//
					// Если находимся в самом начале документа, то необходимо удалить ещё один символ
					//
					if (headerCursor.position() == 0) {
						headerCursor.deleteChar();
					}
				}
			}

			//
			// Убрать декорации
			//
			if (oldBlockStyle.hasDecoration()) {
				QString blockText = cursor.block().text();
				//
				// ... префикс
				//
				if (blockText.startsWith(oldBlockStyle.prefix())) {
					cursor.movePosition(QTextCursor::StartOfBlock);
					for (int repeats = 0; repeats < oldBlockStyle.prefix().length(); ++repeats) {
						cursor.deleteChar();
					}
				}

				//
				// ... постфикс
				//
				if (blockText.endsWith(oldBlockStyle.postfix())) {
					cursor.movePosition(QTextCursor::EndOfBlock);
					for (int repeats = 0; repeats < oldBlockStyle.postfix().length(); ++repeats) {
						cursor.deletePreviousChar();
					}
				}
			}
		}

		//
		// Применим новый стиль к блоку
		//
		{
			QTextCursor cursor = textCursor();
			ScenarioTextBlockStyle newBlockStyle(_blockType);

			//
			// Обновим стили
			//
			cursor.setBlockCharFormat(newBlockStyle.charFormat());
			cursor.setBlockFormat(newBlockStyle.blockFormat());

			//
			// Применим стиль текста ко всему блоку, выделив его,
			// т.к. в блоке могут находиться фрагменты в другом стиле
			//
			cursor.select(QTextCursor::BlockUnderCursor);
			cursor.setCharFormat(newBlockStyle.charFormat());
			cursor.clearSelection();

			//
			// Вставим префикс и постфикс стиля, если необходимо
			//
			if (newBlockStyle.hasDecoration()) {
				int cursorPosition = cursor.position();
				QString blockText = cursor.block().text();
				if (!newBlockStyle.postfix().isEmpty()
					&& !blockText.startsWith(newBlockStyle.prefix())) {
					cursor.movePosition(QTextCursor::StartOfBlock);
					cursor.insertText(newBlockStyle.prefix());
					cursorPosition += newBlockStyle.prefix().length();
				}
				if (!newBlockStyle.prefix().isEmpty()
					&& !blockText.endsWith(newBlockStyle.postfix())) {
					cursor.movePosition(QTextCursor::EndOfBlock);
					cursor.insertText(newBlockStyle.postfix());
				}
				cursor.setPosition(cursorPosition);
				setTextCursor(cursor);
			}

			//
			// Вставим заголовок, если необходимо
			//
			if (newBlockStyle.hasHeader()) {
				ScenarioTextBlockStyle headerStyle(newBlockStyle.headerType());

				cursor.movePosition(QTextCursor::StartOfBlock);
				cursor.insertBlock();
				cursor.movePosition(QTextCursor::Left);

				cursor.setBlockCharFormat(headerStyle.charFormat());
				cursor.setBlockFormat(headerStyle.blockFormat());

				cursor.insertText(newBlockStyle.header());
			}
		}

		//
		// Уведомим о том, что стиль сменился
		//
		emit currentStyleChanged();
	}
}

ScenarioTextBlockStyle::Type ScenarioTextEdit::scenarioBlockType(const QTextBlock& _block)
{
	return (ScenarioTextBlockStyle::Type)_block.blockFormat().intProperty(ScenarioTextBlockStyle::PropertyType);
}

ScenarioTextBlockStyle::Type ScenarioTextEdit::scenarioBlockType()
{
	return scenarioBlockType(textCursor().block());
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

void ScenarioTextEdit::insertFromMimeData(const QMimeData* _source)
{
	if (_source->hasText()) {
		QString textToInsert = _source->text();
		textToInsert = textToInsert.simplified();
		textCursor().insertText(textToInsert);
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
	qDebug()
			<< block.blockFormat().intProperty(ScenarioTextBlockStyle::PropertyType)
			<< textCursor().charFormat().boolProperty(ScenarioTextBlockStyle::PropertyIsCanModify);

	QTextBlock::iterator it;
	for (it = block.begin(); !(it.atEnd()); ++it) {
		QTextFragment currentFragment = it.fragment();
		if (currentFragment.isValid()) {
			qDebug() << currentFragment.text();
		}
	}
}
