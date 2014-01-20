#include "ScenarioTextEdit.h"

#include "Handlers/KeyPressHandlerFacade.h"
#include "MimeData/MimeDataProcessor.h"

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
	CompletableTextEdit(_parent),
	m_textUpdateInProgress(false)
{
	initEditor();
	initView();
	initConnections();
}

void ScenarioTextEdit::addScenarioBlock(ScenarioTextBlockStyle::Type _blockType)
{
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
}

void ScenarioTextEdit::changeScenarioBlockType(ScenarioTextBlockStyle::Type _blockType)
{
	textCursor().beginEditBlock();

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

		//
		// Обработаем предшествующий установленный стиль
		//
		cleanScenarioTypeFromBlock();

		//
		// Применим новый стиль к блоку
		//
		applyScenarioTypeToBlock(_blockType);

		//
		// Уведомим о том, что стиль сменился
		//
		emit currentStyleChanged();
	}

	textCursor().endEditBlock();
}

void ScenarioTextEdit::applyScenarioTypeToBlockText(ScenarioTextBlockStyle::Type _blockType)
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
}

ScenarioTextBlockStyle::Type ScenarioTextEdit::scenarioBlockType(const QTextBlock& _block) const
{
	return (ScenarioTextBlockStyle::Type)_block.blockFormat().intProperty(ScenarioTextBlockStyle::PropertyType);
}

ScenarioTextBlockStyle::Type ScenarioTextEdit::scenarioBlockType() const
{
	return scenarioBlockType(textCursor().block());
}

bool ScenarioTextEdit::textUpdateInProgress() const
{
	return m_textUpdateInProgress;
}

void ScenarioTextEdit::setTextUpdateInProgress(bool _inProgress)
{
	if (m_textUpdateInProgress != _inProgress) {
		m_textUpdateInProgress = _inProgress;
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
	//
	QTextCursor cursor = textCursor();

	//
	// Начнём блок операций
	//
	if (_event->key() != -1) {
		cursor.beginEditBlock();
	}

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
	if (_event->key() != -1) {
		cursor.endEditBlock();
	}

	//
	// Убедимся, что курсор виден
	//
	if (handler->needEnshureCursorVisible()) {
		ensureCursorVisible();
	}

	//
	// Если необходимо уведомим об изменении структуры
	//
	if (handler->structureChanged()) {
		emit structureChanged();
	}
}

void ScenarioTextEdit::wheelEvent(QWheelEvent* _event)
{
	if (_event->modifiers() & Qt::ControlModifier) {
		if (_event->orientation() == Qt::Vertical) {
			//
			// zoomRange > 0 - Текст увеличивается
			// zoomRange < 0 - Текст уменьшается
			//
			int zoomRange = (_event->delta() / 120) * 2;
			zoomIn(zoomRange);

			_event->accept();
		}
	} else {
		QTextEdit::wheelEvent(_event);
	}
}

void ScenarioTextEdit::dropEvent(QDropEvent* _event)
{
	QTextCursor cursor = textCursor();

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
}

bool ScenarioTextEdit::canInsertFromMimeData(const QMimeData* _source) const
{
	bool canInsert = false;
	if (_source->formats().contains(MimeDataProcessor::SCENARIO_MIME_TYPE)
		|| _source->hasText()) {
		canInsert = true;
	}
	return canInsert;
}

QMimeData* ScenarioTextEdit::createMimeDataFromSelection() const
{
	QMimeData* mimeData = new QMimeData;

	//
	// Если выделен текст из разных блоков, поместим
	// в буфер данные о них в специальном формате
	//
	{
		QTextCursor cursor = textCursor();
		cursor.setPosition(textCursor().selectionStart());
		int startSelectionBlockNumber = cursor.blockNumber();
		cursor.setPosition(textCursor().selectionEnd());
		int endSelectionBlockNumber = cursor.blockNumber();

		if (startSelectionBlockNumber != endSelectionBlockNumber) {
			mimeData->setData(
						MimeDataProcessor::SCENARIO_MIME_TYPE,
						MimeDataProcessor::createMimeFromSelection(this).toUtf8());
		}
	}

	mimeData->setData("text/plain", textCursor().selectedText().toUtf8());
	return mimeData;
}

void ScenarioTextEdit::insertFromMimeData(const QMimeData* _source)
{
	if (_source->formats().contains(MimeDataProcessor::SCENARIO_MIME_TYPE)) {
		MimeDataProcessor::insertFromMime(this, _source);
	} else if (_source->hasText()) {
		QString textToInsert = _source->text();
		foreach (const QString& line, textToInsert.split("\n", QString::SkipEmptyParts)) {
			addScenarioBlock(ScenarioTextBlockStyle::NoprintableText);
			textCursor().insertText(line.simplified());
		}
	}
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
					scenarioBlockType(cursor.block());

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

			cursor.movePosition(QTextCursor::NextBlock);
		} while (!isFooterUpdated);
	}

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

void ScenarioTextEdit::applyScenarioTypeToBlock(ScenarioTextBlockStyle::Type _blockType)
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

	//
	// Для заголовка группы нужно создать завершение
	//
	if (newBlockStyle.isEmbeddableHeader()) {
		ScenarioTextBlockStyle footerStyle(newBlockStyle.embeddableFooter());

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

void ScenarioTextEdit::initEditor()
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
	changeScenarioBlockType(ScenarioTextBlockStyle::TimeAndPlace);
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
	connect(this, SIGNAL(cursorPositionChanged()), this, SIGNAL(currentStyleChanged()));
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
