#include "ScenarioTextEdit.h"

#include "Handlers/KeyPressHandlerFacade.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>

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

using UserInterface::ScenarioTextEdit;
using namespace BusinessLogic;

namespace BusinessLogic {
	/**
	 * Параметры стиля страницы
	 */
	/** @{ */
	const int PAGE_FONT_SIZE = 12;
	const QFont PAGE_FONT("Courier New", PAGE_FONT_SIZE);
	const int PAGE_MARGIN = 24;
	/** @} */
}


ScenarioTextEdit::ScenarioTextEdit(QWidget* _parent) :
	CompletableTextEdit(_parent),
	m_storeDataWhenEditing(true)
{
	m_document = new ScenarioTextDocument(this, 0);
	initDocument(m_document);
	setDocument(m_document);
	initEditor();

	initView();
	initConnections();
}

void ScenarioTextEdit::init()
{
	initConnections();
}

void ScenarioTextEdit::setScenarioDocument(ScenarioTextDocument* _document)
{
	resetZoom();

	m_document = _document;
	initDocument(m_document);
	setDocument(m_document);

	if (m_document != 0) {
		initEditor();
	}

	resetHighlighter();
}

void ScenarioTextEdit::addScenarioBlock(ScenarioTextBlockStyle::Type _blockType)
{
	QTextCursor cursor = textCursor();
	cursor.beginEditBlock();

	//
	// Вставим блок
	//
	textCursor().insertBlock();

	//
	// Применим стиль к новому блоку
	//
	applyScenarioTypeToBlock(_blockType);

	//
	// Уведомим о том, что стиль сменился
	//
	emit currentStyleChanged();

	cursor.endEditBlock();
}

void ScenarioTextEdit::changeScenarioBlockType(ScenarioTextBlockStyle::Type _blockType)
{
	if (scenarioBlockType() == _blockType) {
		return;
	}

	QTextCursor cursor = textCursor();
	cursor.beginEditBlock();

	//
	// Нельзя сменить стиль заголовка блока и конечных элементов групп и папок
	//
	bool canChangeType =
			(scenarioBlockType() != ScenarioTextBlockStyle::TitleHeader)
			&& (scenarioBlockType() != ScenarioTextBlockStyle::SceneGroupFooter)
			&& (scenarioBlockType() != ScenarioTextBlockStyle::FolderFooter);

	//
	// Если текущий вид можно сменить
	//
	if (canChangeType) {
		//
		// Закроем подсказку
		//
		closeCompleter();

		ScenarioTextBlockStyle oldStyle(scenarioBlockType());
		ScenarioTextBlockStyle newStyle(_blockType);

		//
		// Если необходимо сменить группирующий стиль на аналогичный
		//
		if (oldStyle.isEmbeddableHeader()
			&& newStyle.isEmbeddableHeader()) {
			applyScenarioGroupTypeToGroupBlock(_blockType);
		}
		//
		// Во всех остальных случаях
		//
		else {
			//
			// Обработаем предшествующий установленный стиль
			//
			cleanScenarioTypeFromBlock();

			//
			// Применим новый стиль к блоку
			//
			applyScenarioTypeToBlock(_blockType);
		}

		//
		// Уведомим о том, что стиль сменился
		//
		emit currentStyleChanged();
	}

	cursor.endEditBlock();

	emit styleChanged();
}

void ScenarioTextEdit::applyScenarioTypeToBlockText(ScenarioTextBlockStyle::Type _blockType)
{
	QTextCursor cursor = textCursor();
	cursor.beginEditBlock();

	ScenarioTextBlockStyle newBlockStyle(_blockType, cursor.charFormat().font());

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

	cursor.endEditBlock();

	emit styleChanged();
}

ScenarioTextBlockStyle::Type ScenarioTextEdit::scenarioBlockType() const
{
	return ScenarioTextBlockStyle::forBlock(textCursor().block());
}

void ScenarioTextEdit::setTextCursorReimpl(const QTextCursor& _cursor)
{
	int verticalScrollValue = verticalScrollBar()->value();
	setTextCursor(_cursor);
	verticalScrollBar()->setValue(verticalScrollValue);
}

bool ScenarioTextEdit::storeDataWhenEditing() const
{
	return m_storeDataWhenEditing;
}

void ScenarioTextEdit::setStoreDataWhenEditing(bool _store)
{
	if (m_storeDataWhenEditing != _store) {
		m_storeDataWhenEditing = _store;
	}
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
	// Начнём блок операций
	//
	QTextCursor cursor = textCursor();
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
		updateEnteredText(_event);
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
	if (handler->needEnsureCursorVisible()) {
		ensureCursorVisible();
    }
}

void ScenarioTextEdit::paintEvent(QPaintEvent* _event)
{
	//
	// Если в документе формат первого блока имеет отступ сверху, это приводит
	// к некорректной прорисовке текста, это баг Qt...
	// Поэтому приходится отлавливать этот момент и вручную корректировать
	//
//	QTextCursor cursor(document());
//	if (verticalScrollBar()->value() == 0
//		&& document() != 0
//		&& !document()->isEmpty()
//		&& cursorRect(cursor).top() > 30) {
//		cursor.beginEditBlock();
//		cursor.setBlockFormat(cursor.blockFormat());
//		cursor.movePosition(QTextCursor::End);
//		cursor.setBlockFormat(cursor.blockFormat());
//		cursor.endEditBlock();
//	}

	CompletableTextEdit::paintEvent(_event);
}

void ScenarioTextEdit::dropEvent(QDropEvent* _event)
{
	QTextCursor cursor = textCursor();
	cursor.beginEditBlock();

	if (cursor.hasSelection()) {
		//
		// Если курсор в начале блоке, нужно сместиться ещё на один символ влево,
		// чтобы не оставлять за собой пустых блоков
		//
		int startCursorPosition = qMin(cursor.selectionStart(), cursor.selectionEnd());
		int endCursorPosition = qMax(cursor.selectionStart(), cursor.selectionEnd());
		cursor.setPosition(startCursorPosition);
		if (cursor.atBlockStart()) {
			cursor.movePosition(QTextCursor::Left);
		}
		cursor.setPosition(endCursorPosition, QTextCursor::KeepAnchor);
		setTextCursor(cursor);
	}

	CompletableTextEdit::dropEvent(_event);

	cursor.endEditBlock();
}

bool ScenarioTextEdit::canInsertFromMimeData(const QMimeData* _source) const
{
	bool canInsert = false;
	if (_source->formats().contains(ScenarioDocument::MIME_TYPE)
		|| _source->hasText()) {
		canInsert = true;
	}
	return canInsert;
}

QMimeData* ScenarioTextEdit::createMimeDataFromSelection() const
{
	QMimeData* mimeData = new QMimeData;

	//
	// Поместим в буфер данные о тексте в специальном формате
	//
	{
		QTextCursor cursor = textCursor();
		cursor.setPosition(textCursor().selectionStart());
		cursor.setPosition(textCursor().selectionEnd());

		mimeData->setData(
					ScenarioDocument::MIME_TYPE,
					m_document->mimeFromSelection(textCursor().selectionStart(),
												  textCursor().selectionEnd()).toUtf8());
	}

	mimeData->setData("text/plain", textCursor().selectedText().toUtf8());
	return mimeData;
}

void ScenarioTextEdit::insertFromMimeData(const QMimeData* _source)
{
	QTextCursor cursor = textCursor();
	cursor.beginEditBlock();

	//
	// Если вставляются данные в сценарном формате, то вставляем как положено
	//
	if (_source->formats().contains(ScenarioDocument::MIME_TYPE)) {
		m_document->insertFromMime(cursor.position(), _source->data(ScenarioDocument::MIME_TYPE));
	}
	//
	// Если простой текст, то вставляем его, как непечатаемый текст
	//
	else if (_source->hasText()) {
		QString textToInsert = _source->text();
		foreach (const QString& line, textToInsert.split("\n", QString::SkipEmptyParts)) {
			if (cursor.block().text().isEmpty()) {
				changeScenarioBlockType(ScenarioTextBlockStyle::NoprintableText);
			} else {
				moveCursor(QTextCursor::EndOfBlock);
				addScenarioBlock(ScenarioTextBlockStyle::NoprintableText);
			}
			cursor.insertText(line.simplified());
		}
	}

    cursor.endEditBlock();
}

void ScenarioTextEdit::cleanScenarioTypeFromBlock()
{
	QTextCursor cursor = textCursor();
	ScenarioTextBlockStyle oldBlockStyle(scenarioBlockType());

	//
	// Удалить завершающий блок группы сцен
	//
	if (oldBlockStyle.isEmbeddableHeader()) {
		QTextCursor cursor = textCursor();
		cursor.movePosition(QTextCursor::NextBlock);

		// ... открытые группы на пути поиска необходимого для обновления блока
		int openedGroups = 0;
		bool isFooterUpdated = false;
		do {
			ScenarioTextBlockStyle::Type currentType =
					ScenarioTextBlockStyle::forBlock(cursor.block());

			if (currentType == oldBlockStyle.embeddableFooter()) {
				if (openedGroups == 0) {
					cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
					cursor.deleteChar();
					cursor.deletePreviousChar();
					isFooterUpdated = true;
				} else {
					--openedGroups;
				}
			} else if (currentType == oldBlockStyle.blockType()) {
				// ... встретилась новая группа
				++openedGroups;
			}
			cursor.movePosition(QTextCursor::EndOfBlock);
			cursor.movePosition(QTextCursor::NextBlock);
		} while (!isFooterUpdated
				 && !cursor.atEnd());
	}

	//
	// Удалить заголовок если происходит смена стиля в текущем блоке
	// в противном случае его не нужно удалять
	//
	if (oldBlockStyle.hasHeader()) {
		QTextCursor headerCursor = cursor;
		headerCursor.movePosition(QTextCursor::StartOfBlock);
		headerCursor.movePosition(QTextCursor::Left);
		if (ScenarioTextBlockStyle::forBlock(headerCursor.block()) == oldBlockStyle.headerType()) {
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

void ScenarioTextEdit::applyScenarioTypeToBlock(ScenarioTextBlockStyle::Type _blockType)
{
	QTextCursor cursor = textCursor();
	ScenarioTextBlockStyle newBlockStyle(_blockType, cursor.charFormat().font());

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
		//
		// Запомним позицию курсора в блоке
		//
		int cursorPosition = cursor.position();

		QString blockText = cursor.block().text();
		if (!newBlockStyle.prefix().isEmpty()
			&& !blockText.startsWith(newBlockStyle.prefix())) {
			cursor.movePosition(QTextCursor::StartOfBlock);
			cursor.insertText(newBlockStyle.prefix());

			cursorPosition += newBlockStyle.prefix().length();
		}
		if (!newBlockStyle.postfix().isEmpty()
			&& !blockText.endsWith(newBlockStyle.postfix())) {
			cursor.movePosition(QTextCursor::EndOfBlock);
			cursor.insertText(newBlockStyle.postfix());
		}

		cursor.setPosition(cursorPosition);
		setTextCursorReimpl(cursor);
	}

	//
	// Вставим заголовок, если необходимо
	//
	if (newBlockStyle.hasHeader()) {
		ScenarioTextBlockStyle headerStyle(newBlockStyle.headerType(), cursor.charFormat().font());

		cursor.movePosition(QTextCursor::StartOfBlock);
		cursor.insertBlock();
		cursor.movePosition(QTextCursor::Left);

		cursor.setBlockCharFormat(headerStyle.charFormat());
		cursor.setBlockFormat(headerStyle.blockFormat());

		cursor.insertText(newBlockStyle.header());
	}

	//
	// Для заголовка группы нужно создать завершение
	//
	if (newBlockStyle.isEmbeddableHeader()) {
		ScenarioTextBlockStyle footerStyle(newBlockStyle.embeddableFooter(), cursor.charFormat().font());

		//
		// Запомним позицию курсора
		//
		int lastCursorPosition = textCursor().position();

		cursor.movePosition(QTextCursor::EndOfBlock);
		cursor.insertBlock();

		cursor.setBlockCharFormat(footerStyle.charFormat());
		cursor.setBlockFormat(footerStyle.blockFormat());

		//
		// т.к. вставлен блок, нужно вернуть курсор на место
		//
		cursor.setPosition(lastCursorPosition);
		setTextCursor(cursor);


		QKeyEvent empyEvent(QEvent::KeyPress, -1, Qt::NoModifier);
		keyPressEvent(&empyEvent);
	}
}

void ScenarioTextEdit::applyScenarioGroupTypeToGroupBlock(ScenarioTextBlockStyle::Type _blockType)
{
	const QFont font(textCursor().charFormat().font());

	ScenarioTextBlockStyle oldBlockStyle(scenarioBlockType());
	ScenarioTextBlockStyle newBlockHeaderStyle(_blockType, font);
	ScenarioTextBlockStyle newBlockFooterStyle(newBlockHeaderStyle.embeddableFooter(), font);

	//
	// Сменим стиль заголовочного блока
	//
	{
		QTextCursor cursor = textCursor();

		//
		// Обновим стили
		//
		cursor.setBlockCharFormat(newBlockHeaderStyle.charFormat());
		cursor.setBlockFormat(newBlockHeaderStyle.blockFormat());

		//
		// Применим стиль текста ко всему блоку, выделив его,
		// т.к. в блоке могут находиться фрагменты в другом стиле
		//
		cursor.select(QTextCursor::BlockUnderCursor);
		cursor.setCharFormat(newBlockHeaderStyle.charFormat());
		cursor.clearSelection();
	}

	//
	// Обновим стиль завершающего блока группы
	//
	{
		QTextCursor cursor = textCursor();
		cursor.movePosition(QTextCursor::NextBlock);

		// ... открытые группы на пути поиска необходимого для обновления блока
		int openedGroups = 0;
		bool isFooterUpdated = false;
		do {
			ScenarioTextBlockStyle::Type currentType =
					ScenarioTextBlockStyle::forBlock(cursor.block());

			if (currentType == oldBlockStyle.embeddableFooter()) {
				if (openedGroups == 0) {
					//
					// Обновим стили
					//
					cursor.setBlockCharFormat(newBlockFooterStyle.charFormat());
					cursor.setBlockFormat(newBlockFooterStyle.blockFormat());

					//
					// Применим стиль текста ко всему блоку, выделив его,
					// т.к. в блоке могут находиться фрагменты в другом стиле
					//
					cursor.select(QTextCursor::BlockUnderCursor);
					cursor.setCharFormat(newBlockFooterStyle.charFormat());
					cursor.clearSelection();
					isFooterUpdated = true;
				} else {
					--openedGroups;
				}
			} else if (currentType == oldBlockStyle.blockType()) {
				// ... встретилась новая группа
				++openedGroups;
			}

			cursor.movePosition(QTextCursor::EndOfBlock);
			cursor.movePosition(QTextCursor::NextBlock);
		} while (!isFooterUpdated
				 && !cursor.atEnd());
	}
}

void ScenarioTextEdit::updateEnteredText(QKeyEvent* _event)
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

			//
			// Если была попытка ввести несколько пробелов подряд, отменяем последнее действие
			//
			if (cursorBackwardText.endsWith("  ")) {
				cursor.deletePreviousChar();
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

void ScenarioTextEdit::initDocument(QTextDocument* _document)
{
	if (_document != 0) {
		//
		// Настраиваем документ редактора
		//
		_document->setDefaultFont(PAGE_FONT);
		_document->setDocumentMargin(PAGE_MARGIN);
	}
}

void ScenarioTextEdit::initEditor()
{
	//
	// Настраиваем редактор
	//
	setFont(PAGE_FONT);

	//
	// Настроим стиль первого блока, если необходимо
	//
	QTextCursor cursor(document());
	setTextCursor(cursor);
	if (BusinessLogic::ScenarioTextBlockStyle::forBlock(cursor.block())
		== BusinessLogic::ScenarioTextBlockStyle::Undefined) {
		applyScenarioTypeToBlockText(ScenarioTextBlockStyle::TimeAndPlace);
	}
}

void ScenarioTextEdit::initView()
{
	//
	// Параметры внешнего вида
	//
	const int MINIMUM_WIDTH = 400;
	const int MINIMUM_HEIGHT = 100;

	setMinimumSize(MINIMUM_WIDTH, MINIMUM_HEIGHT);
}

void ScenarioTextEdit::initConnections()
{
	//
	// При перемещении курсора может меняться стиль блока
	//
	connect(this, SIGNAL(cursorPositionChanged()), this, SIGNAL(currentStyleChanged()), Qt::UniqueConnection);
}
