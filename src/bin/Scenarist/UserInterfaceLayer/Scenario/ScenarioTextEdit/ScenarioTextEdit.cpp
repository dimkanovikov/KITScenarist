#include "ScenarioTextEdit.h"

#include "Handlers/KeyPressHandlerFacade.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h>

#include <3rd_party/Helpers/TextEditHelper.h>


#include <QAbstractItemView>
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QDateTime>
#include <QCompleter>
#include <QKeyEvent>
#include <QMimeData>
#include <QPainter>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextBlock>
#include <QTextCursor>
#include <QTimer>
#include <QWheelEvent>

using UserInterface::ScenarioTextEdit;
using namespace BusinessLogic;


ScenarioTextEdit::ScenarioTextEdit(QWidget* _parent) :
	CompletableTextEdit(_parent),
	m_storeDataWhenEditing(true),
	m_showSceneNumbers(false)
{
	m_document = new ScenarioTextDocument(this, 0);
	setDocument(m_document);
	initEditor();

	initView();
	initConnections();
}

void ScenarioTextEdit::setScenarioDocument(ScenarioTextDocument* _document)
{
	m_document = _document;
	setDocument(m_document);

	if (m_document != 0) {
		initEditor();
	}

	setHighlighterDocument(m_document);

	TextEditHelper::beautifyDocument(m_document);
}

void ScenarioTextEdit::addScenarioBlock(ScenarioBlockStyle::Type _blockType)
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

void ScenarioTextEdit::changeScenarioBlockType(ScenarioBlockStyle::Type _blockType)
{
	if (scenarioBlockType() == _blockType) {
		return;
	}

	QTextCursor cursor = textCursor();
	cursor.beginEditBlock();

	//
	// Нельзя сменить стиль заголовка титра и конечных элементов групп и папок
	//
	bool canChangeType =
			(scenarioBlockType() != ScenarioBlockStyle::TitleHeader)
			&& (scenarioBlockType() != ScenarioBlockStyle::SceneGroupFooter)
			&& (scenarioBlockType() != ScenarioBlockStyle::FolderFooter);

	//
	// Если текущий вид можно сменить
	//
	if (canChangeType) {
		//
		// Закроем подсказку
		//
		closeCompleter();

		ScenarioBlockStyle oldStyle = ScenarioStyleFacade::style().blockStyle(scenarioBlockType());
		ScenarioBlockStyle newStyle = ScenarioStyleFacade::style().blockStyle(_blockType);

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

void ScenarioTextEdit::applyScenarioTypeToBlockText(ScenarioBlockStyle::Type _blockType)
{
	QTextCursor cursor = textCursor();
	cursor.beginEditBlock();

	ScenarioBlockStyle newBlockStyle = ScenarioStyleFacade::style().blockStyle(_blockType);

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

ScenarioBlockStyle::Type ScenarioTextEdit::scenarioBlockType() const
{
	return ScenarioBlockStyle::forBlock(textCursor().block());
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

bool ScenarioTextEdit::showSceneNumbers() const
{
	return m_showSceneNumbers;
}

void ScenarioTextEdit::setShowSceneNumbers(bool _show)
{
	if (m_showSceneNumbers != _show) {
		m_showSceneNumbers = _show;
	}
}

void ScenarioTextEdit::ensureCursorVisibleReimpl()
{
	//
	// Применяем стандартное поведение
	//
	ensureCursorVisible();

	//
	// Необходимо подождать, пока в приложение произойдёт перестройка размера линии прокрутки
	// для того, чтобы иметь возможность прокрутить её ниже стандартной позиции редактора
	//
	QApplication::processEvents();

	//
	// Если курсор в конце документа прокручиваем ещё немного
	//
	{
		const int DETECT_DELTA = 10;
		const int SCROLL_DELTA = 200;
		QRect cursorRect = this->cursorRect();
		if (cursorRect.height() + cursorRect.y() + DETECT_DELTA >= viewport()->height()) {
			verticalScrollBar()->setValue(verticalScrollBar()->value() + SCROLL_DELTA);
		}
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
		TextEditHelper::beautifyDocument(textCursor(), _event->text());
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
		ensureCursorVisibleReimpl();
	}
}

void ScenarioTextEdit::paintEvent(QPaintEvent* _event)
{
	//
	// FIXME:
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

	//
	// Прорисовка номеров сцен, если необходимо
	//
	if (m_showSceneNumbers) {
		//
		// Определить область прорисовки номеров
		//
		const int left = 0;
		const int right = document()->rootFrame()->frameFormat().leftMargin() - 10;


		QPainter painter(viewport());


		QTextBlock block = document()->begin();
		const QRectF viewportGeometry = viewport()->geometry();
		const int leftDelta = -horizontalScrollBar()->value();

		QTextCursor cursor(document());
		while (block.isValid()) {
			if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::TimeAndPlace) {
				cursor.setPosition(block.position());
				QRect cursorR = cursorRect(cursor);

				//
				// Курсор на экране
				//
				// ... ниже верхней границы
				if ((cursorR.top() > 0 || cursorR.bottom() > 0)
					// ... и выше нижней
					&& cursorR.top() < viewportGeometry.bottom()) {

					//
					// Определим номер сцены
					//
					QTextBlockUserData* textBlockData = block.userData();
					if (ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData)) {
						const QString sceneNumber = QString::number(info->sceneNumber()) + ".";

						//
						// Определим область для отрисовки и выведем номер сцены в редактор
						//
						QPointF topLeft(left + leftDelta, cursorR.top());
						QPointF bottomRight(right + leftDelta, cursorR.bottom());
						QRectF rect(topLeft, bottomRight);
						painter.setFont(cursor.charFormat().font());
						painter.drawText(rect, Qt::AlignRight | Qt::AlignTop, sceneNumber);
					}
				}
			}

			block = block.next();
		}
	}
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
	// Если простой текст, то вставляем его, как описание действия
	//
	else if (_source->hasText()) {
		QString textToInsert = _source->text();
		foreach (const QString& line, textToInsert.split("\n", QString::SkipEmptyParts)) {
			if (cursor.block().text().isEmpty()) {
				changeScenarioBlockType(ScenarioBlockStyle::Action);
			} else {
				moveCursor(QTextCursor::EndOfBlock);
				addScenarioBlock(ScenarioBlockStyle::Action);
			}
			cursor.insertText(line.simplified());
		}
	}

	cursor.endEditBlock();
}

void ScenarioTextEdit::resizeEvent(QResizeEvent* _event)
{
	CompletableTextEdit::resizeEvent(_event);

	QTimer::singleShot(10, this, SLOT(ensureCursorVisibleReimpl()));
}

void ScenarioTextEdit::cleanScenarioTypeFromBlock()
{
	QTextCursor cursor = textCursor();
	ScenarioBlockStyle oldBlockStyle = ScenarioStyleFacade::style().blockStyle(scenarioBlockType());

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
			ScenarioBlockStyle::Type currentType =
					ScenarioBlockStyle::forBlock(cursor.block());

			if (currentType == oldBlockStyle.embeddableFooter()) {
				if (openedGroups == 0) {
					cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
					cursor.deleteChar();
					cursor.deletePreviousChar();
					isFooterUpdated = true;
				} else {
					--openedGroups;
				}
			} else if (currentType == oldBlockStyle.type()) {
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
		if (ScenarioBlockStyle::forBlock(headerCursor.block()) == oldBlockStyle.headerType()) {
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

void ScenarioTextEdit::applyScenarioTypeToBlock(ScenarioBlockStyle::Type _blockType)
{
	QTextCursor cursor = textCursor();
	ScenarioBlockStyle newBlockStyle = ScenarioStyleFacade::style().blockStyle(_blockType);

	//
	// Обновим стили
	//
	cursor.setBlockCharFormat(newBlockStyle.charFormat());
	cursor.setBlockFormat(newBlockStyle.blockFormat());

	//
	// Применим стиль текста ко всему блоку, выделив его,
	// т.к. в блоке могут находиться фрагменты в другом стиле
	//
	cursor.movePosition(QTextCursor::StartOfBlock);
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
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
		ScenarioBlockStyle headerStyle = ScenarioStyleFacade::style().blockStyle(newBlockStyle.headerType());

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
		ScenarioBlockStyle footerStyle = ScenarioStyleFacade::style().blockStyle(newBlockStyle.embeddableFooter());

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

void ScenarioTextEdit::applyScenarioGroupTypeToGroupBlock(ScenarioBlockStyle::Type _blockType)
{
	ScenarioBlockStyle oldBlockStyle = ScenarioStyleFacade::style().blockStyle(scenarioBlockType());
	ScenarioBlockStyle newBlockHeaderStyle = ScenarioStyleFacade::style().blockStyle(_blockType);
	ScenarioBlockStyle newBlockFooterStyle = ScenarioStyleFacade::style().blockStyle(newBlockHeaderStyle.embeddableFooter());

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
			ScenarioBlockStyle::Type currentType =
					ScenarioBlockStyle::forBlock(cursor.block());

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
			} else if (currentType == oldBlockStyle.type()) {
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
			|| cursorBackwardText == (currentCharFormat.stringProperty(ScenarioBlockStyle::PropertyPrefix)
									  + eventText)) {
			//
			// Сформируем правильное представление строки
			//
			bool isFirstUpperCase = currentCharFormat.boolProperty(ScenarioBlockStyle::PropertyIsFirstUppercase);
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
	Q_UNUSED(_text);

	//
	// FIXME проработать словарь сокращений
	//

	return false;
}

void ScenarioTextEdit::initEditor()
{
	//
	// Настроим стиль первого блока, если необходимо
	//
	QTextCursor cursor(document());
	setTextCursor(cursor);
	if (BusinessLogic::ScenarioBlockStyle::forBlock(cursor.block())
		== BusinessLogic::ScenarioBlockStyle::Undefined) {
		applyScenarioTypeToBlockText(ScenarioBlockStyle::TimeAndPlace);
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
