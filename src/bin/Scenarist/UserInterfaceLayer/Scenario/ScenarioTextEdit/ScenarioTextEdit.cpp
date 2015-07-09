#include "ScenarioTextEdit.h"
#include "ScenarioTextEditPrivate.h"

#include "Handlers/KeyPressHandlerFacade.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h>
#include <BusinessLayer/ScenarioDocument/ScenarioReviewModel.h>

#include <3rd_party/Helpers/TextEditHelper.h>


#include <QAbstractItemView>
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QDateTime>
#include <QCompleter>
#include <QKeyEvent>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QScrollBar>
#include <QStringListModel>
#include <QStyleHints>
#include <QTextBlock>
#include <QTextCursor>
#include <QTimer>
#include <QWheelEvent>

using UserInterface::ScenarioTextEdit;
using UserInterface::ShortcutsManager;
using namespace BusinessLogic;

namespace {
	/**
	 * @brief Флаг для перерисовки текста редактора при первом отображении
	 * @note см. paintEvent для детальной информации
	 */
	static bool s_firstRepaintUpdate = true;

	/**
	 * @brief Флаг положения курсора
	 * @note Используется для корректировки скрола при совместном редактировании
	 */
	const char* CURSOR_RECT = "cursorRect";

	/**
	 * @brief Получить цвет для курсора соавтора
	 */
	static QColor cursorColor(int _index) {
		QColor color;
		switch (_index) {
			case 0: color = Qt::red; break;
			case 1: color = Qt::darkGreen; break;
			case 2: color = Qt::blue; break;
			case 3: color = Qt::darkCyan; break;
			case 4: color = Qt::magenta; break;
			case 5: color = Qt::darkMagenta; break;
			case 6: color = Qt::darkRed; break;
			case 7: color = Qt::darkYellow; break;
		}
		return color;
	}
}


ScenarioTextEdit::ScenarioTextEdit(QWidget* _parent) :
	CompletableTextEdit(_parent),
	m_mouseClicks(0),
	m_lastMouseClickTime(0),
	m_storeDataWhenEditing(true),
	m_showSceneNumbers(false),
	m_shortcutsManager(new ShortcutsManager(this))
{
	setAttribute(Qt::WA_KeyCompression);

	m_document = new ScenarioTextDocument(this, 0);
	setDocument(m_document);
	initEditor();

	initView();
	initConnections();
}

void ScenarioTextEdit::setScenarioDocument(ScenarioTextDocument* _document)
{
	removeEditorConnections();

	m_document = _document;
	setDocument(m_document);

	if (m_document != 0) {
		initEditor();
	}

	setHighlighterDocument(m_document);

	TextEditHelper::beautifyDocument(m_document);

	s_firstRepaintUpdate = true;
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

		ScenarioBlockStyle oldStyle = ScenarioTemplateFacade::getTemplate().blockStyle(scenarioBlockType());
		ScenarioBlockStyle newStyle = ScenarioTemplateFacade::getTemplate().blockStyle(_blockType);

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

	ScenarioBlockStyle newBlockStyle = ScenarioTemplateFacade::getTemplate().blockStyle(_blockType);

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
	cursor.mergeCharFormat(newBlockStyle.charFormat());

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

void ScenarioTextEdit::updateShortcuts()
{
	m_shortcutsManager->update();
}

QString ScenarioTextEdit::shortcut(ScenarioBlockStyle::Type _forType) const
{
	return m_shortcutsManager->shortcut(_forType);
}

QMenu* ScenarioTextEdit::createContextMenu(const QPoint& _pos)
{
	//
	// Формируем стандартное меню, чтобы добраться до действий отмены и повтора последнего действия
	// и присоединить их к собственным функциям повтора/отмены послденего действия
	//
	QMenu* menu = CompletableTextEdit::createContextMenu(_pos);
	foreach (QAction* menuAction, menu->findChildren<QAction*>()) {
		if (menuAction->text().endsWith(QKeySequence(QKeySequence::Undo).toString(QKeySequence::NativeText))) {
			menuAction->disconnect();
			connect(menuAction, SIGNAL(triggered()), this, SLOT(undoReimpl()));
			menuAction->setEnabled(m_document->isUndoAvailableReimpl());
		} else if (menuAction->text().endsWith(QKeySequence(QKeySequence::Redo).toString(QKeySequence::NativeText))) {
			menuAction->disconnect();
			connect(menuAction, SIGNAL(triggered()), this, SLOT(redoReimpl()));
			menuAction->setEnabled(m_document->isRedoAvailableReimpl());
		}
	}

	return menu;
}

void ScenarioTextEdit::setAdditionalCursors(const QMap<QString, int>& _cursors)
{
	if (m_additionalCursors != _cursors) {
		//
		// Обновим позиции
		//
		QMutableMapIterator<QString, int> iter(m_additionalCursors);
		while (iter.hasNext()) {
			iter.next();

			const QString username = iter.key();
			const int cursorPosition = iter.value();

			//
			// Если такой пользователь есть, то возможно необходимо обновить значение
			//
			if (_cursors.contains(username)) {
				const int newCursorPosition = _cursors.value(username);
				if (cursorPosition != newCursorPosition) {
					iter.setValue(newCursorPosition);
					m_additionalCursorsCorrected.insert(username, newCursorPosition);
				}
			}
			//
			// Если нет, удаляем
			//
			else {
				iter.remove();
				m_additionalCursorsCorrected.remove(username);
			}
		}

		//
		// Добавим новых
		//
		foreach (const QString& username, _cursors.keys()) {
			if (!m_additionalCursors.contains(username)) {
				const int cursorPosition = _cursors.value(username);
				m_additionalCursors.insert(username, cursorPosition);
				m_additionalCursorsCorrected.insert(username, cursorPosition);
			}
		}
	}
}

void ScenarioTextEdit::ensureCursorVisibleReimpl(bool _upAndDown)
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
	// Если необходимо прокручиваем ещё немного
	//
	{
		const int DETECT_DELTA = 10;
		const int SCROLL_DELTA = 200;
		QRect cursorRect = this->cursorRect();
		if (cursorRect.y() - DETECT_DELTA <= 0
			&& _upAndDown) {
			verticalScrollBar()->setValue(verticalScrollBar()->value() - SCROLL_DELTA);
		}
		else if (cursorRect.height() + cursorRect.y() + DETECT_DELTA >= viewport()->height()) {
			verticalScrollBar()->setValue(verticalScrollBar()->value() + SCROLL_DELTA);
		}
	}
}

void ScenarioTextEdit::undoReimpl()
{
	m_document->undoReimpl();
}

void ScenarioTextEdit::redoReimpl()
{
	m_document->redoReimpl();
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
		if (!keyPressEventReimpl(_event)) {
			SpellCheckTextEdit::keyPressEvent(_event);
		}

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

bool ScenarioTextEdit::keyPressEventReimpl(QKeyEvent* _event)
{
	bool isEventHandled = true;
	//
	// Переопределяем
	//
	// ... отмену последнего действия
	//
	if (_event == QKeySequence::Undo) {
		undoReimpl();
	}
	//
	// ... повтор последнего действия
	//
	else if (_event == QKeySequence::Redo) {
		redoReimpl();
	}
	//
	// ... перевод курсора к концу строки
	//
	else if (_event == QKeySequence::MoveToEndOfLine) {
		QTextCursor cursor = textCursor();
		const int startY = cursorRect(cursor).y();
		while (!cursor.atEnd()) {
			cursor.movePosition(QTextCursor::Right);
			if (cursorRect(cursor).y() > startY) {
				cursor.movePosition(QTextCursor::Left);
				setTextCursor(cursor);
				break;
			}
		}
		setTextCursor(cursor);
	}
	//
	// ... перевод курсора к началу строки
	//
	else if (_event == QKeySequence::MoveToStartOfLine) {
		QTextCursor cursor = textCursor();
		const int startY = cursorRect(cursor).y();
		while (!cursor.atEnd()) {
			cursor.movePosition(QTextCursor::Left);
			if (cursorRect(cursor).y() < startY) {
				cursor.movePosition(QTextCursor::Right);
				setTextCursor(cursor);
				break;
			}
		}
		setTextCursor(cursor);
    }
    //
    // Поднятие/опускание регистра букв
    //
    else if (_event->modifiers().testFlag(Qt::ControlModifier)
             && (_event->key() == Qt::Key_Up
                 || _event->key() == Qt::Key_Down)
             && textCursor().hasSelection()) {
        const bool toUpper = _event->key() == Qt::Key_Up;
        const int from = qMin(textCursor().selectionStart(), textCursor().selectionEnd());
        const int to = qMax(textCursor().selectionStart(), textCursor().selectionEnd());
        QTextCursor cursor = textCursor();
        for (int position = from; position < to; ++position) {
            cursor.setPosition(position);
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            cursor.insertText(toUpper ? cursor.selectedText().toUpper() : cursor.selectedText().toLower());
        }
    }
    else {
		isEventHandled = false;
	}

	return isEventHandled;
}

void ScenarioTextEdit::paintEvent(QPaintEvent* _event)
{
	//
	// Если в документе формат первого блока имеет отступ сверху, это приводит
	// к некорректной прорисовке текста, это баг Qt...
	// Поэтому приходится отлавливать этот момент и вручную корректировать
	//
	if (isVisible() && s_firstRepaintUpdate) {
		s_firstRepaintUpdate = false;

		QTimer::singleShot(10, this, SLOT(aboutCorrectRepaint()));
	}


	CompletableTextEdit::paintEvent(_event);


	//
	// Прорисовка дополнительных элементов редактора
	//
	{
		//
		// Декорации слева от текста
		//
		{
			//
			// Определить область прорисовки слева от текста
			//
			const int left = 0;
			const int right = document()->rootFrame()->frameFormat().leftMargin() - 10;


			QPainter painter(viewport());

			QTextBlock block = document()->begin();
			const QRectF viewportGeometry = viewport()->geometry();
			const int leftDelta = -horizontalScrollBar()->value();

			QTextCursor cursor(document());
			while (block.isValid()) {
				//
				// Стиль текущего блока
				//
				const ScenarioBlockStyle::Type blockType = ScenarioBlockStyle::forBlock(block);

				{
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
						// Прорисовка символа пустой строки
						//
						if (block.text().simplified().isEmpty()) {
							//
							// Определим область для отрисовки и выведем символ в редактор
							//
							QPointF topLeft(left + leftDelta, cursorR.top());
							QPointF bottomRight(right + leftDelta, cursorR.bottom());
							QRectF rect(topLeft, bottomRight);
							painter.setFont(cursor.charFormat().font());
							painter.drawText(rect, Qt::AlignRight | Qt::AlignTop, "» ");
						}
						//
						// Остальные декорации
						//
						else {
							//
							// Прорисовка номеров сцен, если необходимо
							//
							if (m_showSceneNumbers
								&& blockType == ScenarioBlockStyle::TimeAndPlace) {
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
					}
				}

				block = block.next();
			}
		}

		//
		// Курсоры соавторов
		//
		{
			if (!m_additionalCursors.isEmpty()
				&& m_document != 0) {
				QPainter painter(viewport());
				painter.setFont(QFont("Sans", 8));
				painter.setPen(Qt::white);

				const QRectF viewportGeometry = viewport()->geometry();
				QPoint mouseCursorPos = mapFromGlobal(QCursor::pos());
				mouseCursorPos.setY(mouseCursorPos.y() + viewport()->mapFromParent(QPoint(0,0)).y());
				int cursorIndex = 0;
				foreach (const QString& username, m_additionalCursorsCorrected.keys()) {
					QTextCursor cursor(m_document);
					m_document->setCursorPosition(cursor, m_additionalCursorsCorrected.value(username));
					const QRect cursorR = cursorRect(cursor);

					//
					// Если курсор на экране
					//
					// ... ниже верхней границы
					if ((cursorR.top() > 0 || cursorR.bottom() > 0)
						// ... и выше нижней
						&& cursorR.top() < viewportGeometry.bottom()) {
						//
						// ... рисуем его
						//
						painter.fillRect(cursorR, ::cursorColor(cursorIndex));

						//
						// ... декорируем
						//
						{
							//
							// Если мышь около него, то выводим имя соавтора
							//
							QRect extandedCursorR = cursorR;
							extandedCursorR.setWidth(5);
							if (extandedCursorR.contains(mouseCursorPos)) {
								const QRect usernameRect(
									cursorR.left() - 1,
									cursorR.top() - painter.fontMetrics().height() - 2,
									painter.fontMetrics().width(username) + 2,
									painter.fontMetrics().height() + 2);
								painter.fillRect(usernameRect, ::cursorColor(cursorIndex));
								painter.drawText(usernameRect, Qt::AlignCenter, username);
							}
							//
							// Если нет, то рисуем небольшой квадратик
							//
							else {
								painter.fillRect(cursorR.left() - 2, cursorR.top() - 5, 5, 5,
									::cursorColor(cursorIndex));
							}
						}
					}

					++cursorIndex;
				}
			}
		}
	}
}

void ScenarioTextEdit::mousePressEvent(QMouseEvent* _event)
{
	//
	// Событие о клике приходит на 1, 3, 5 и т.д. кликов
	//

	const qint64 curMouseClickTime = QDateTime::currentMSecsSinceEpoch();
	const qint64 timeDelta = curMouseClickTime - m_lastMouseClickTime;
	if (timeDelta <= (QApplication::styleHints()->mouseDoubleClickInterval())) {
		m_mouseClicks += 2;
	} else {
		m_mouseClicks = 1;
	}
	m_lastMouseClickTime = curMouseClickTime;

	//
	// Тройной клик обрабатываем самостоятельно
	//
	if (m_mouseClicks == 3) {
		QTextCursor cursor = textCursor();
		cursor.movePosition(QTextCursor::StartOfBlock);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		setTextCursor(cursor);
		_event->accept();
	} else {
		CompletableTextEdit::mousePressEvent(_event);
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
	// Если есть выделение, удаляем выделенный текст
	//
	if (cursor.hasSelection()) {
		//
		// Запомним стиль блока начала выделения
		//
		QTextCursor helper = cursor;
		helper.setPosition(cursor.selectionStart());
		ScenarioBlockStyle::Type type = ScenarioBlockStyle::forBlock(helper.block());
		cursor.deleteChar();
		changeScenarioBlockType(type);
	}

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
		bool isFirstLine = true;
		foreach (const QString& line, textToInsert.split("\n", QString::SkipEmptyParts)) {
			//
			// Первую строку вставляем в текущий блок
			//
			if (isFirstLine) {
				isFirstLine = false;
			}
			//
			// А для всех остальных создаём блок описания действия
			//
			else {
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

	s_firstRepaintUpdate = true;
}

void ScenarioTextEdit::aboutCorrectRepaint()
{
	QTextCursor cursor(document());
	cursor.beginEditBlock();
	cursor.setBlockFormat(cursor.blockFormat());
	cursor.movePosition(QTextCursor::End);
	cursor.setBlockFormat(cursor.blockFormat());
	cursor.endEditBlock();
}

void ScenarioTextEdit::aboutCorrectAdditionalCursors(int _position, int _charsRemoved, int _charsAdded)
{
	if (_charsAdded != _charsRemoved) {
		foreach (const QString& username, m_additionalCursorsCorrected.keys()) {
			int additionalCursorPosition = m_additionalCursorsCorrected.value(username);
			if (additionalCursorPosition > _position) {
				additionalCursorPosition += _charsAdded - _charsRemoved;
				m_additionalCursorsCorrected[username] = additionalCursorPosition;
			}
		}
	}
}

void ScenarioTextEdit::aboutSelectionChanged()
{
	//
	// Если включена опция для редактора
	//

	//
	// Отобразить вспомогатаельную панель
	//
}

void ScenarioTextEdit::aboutSaveEditorState()
{
	setProperty(CURSOR_RECT, cursorRect());
}

void ScenarioTextEdit::aboutLoadEditorState()
{
	const QRect prevCursorRect = property(CURSOR_RECT).toRect();
	QRect currentCursorRect = cursorRect();

	//
	// Корректируем позицию курсора, пока
	// - не восстановим предыдущее значение
	// - не сдвинем прокрутку в самый верх
	// - не сдвинем прокрутку в самый низ
	//
	while (prevCursorRect.y() != currentCursorRect.y()
		   && verticalScrollBar()->value() != verticalScrollBar()->minimum()
		   && verticalScrollBar()->value() != verticalScrollBar()->maximum()) {
		int verticalDelta = 0;
		if (prevCursorRect.y() < currentCursorRect.y()) {
			verticalDelta = 1;
		} else {
			verticalDelta = -1;
		}
		verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
		currentCursorRect = cursorRect();
	}
}

void ScenarioTextEdit::cleanScenarioTypeFromBlock()
{
	QTextCursor cursor = textCursor();
	ScenarioBlockStyle oldBlockStyle = ScenarioTemplateFacade::getTemplate().blockStyle(scenarioBlockType());

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
	ScenarioBlockStyle newBlockStyle = ScenarioTemplateFacade::getTemplate().blockStyle(_blockType);

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
		ScenarioBlockStyle headerStyle = ScenarioTemplateFacade::getTemplate().blockStyle(newBlockStyle.headerType());

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
		ScenarioBlockStyle footerStyle = ScenarioTemplateFacade::getTemplate().blockStyle(newBlockStyle.embeddableFooter());

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
	ScenarioBlockStyle oldBlockStyle = ScenarioTemplateFacade::getTemplate().blockStyle(scenarioBlockType());
	ScenarioBlockStyle newBlockHeaderStyle = ScenarioTemplateFacade::getTemplate().blockStyle(_blockType);
	ScenarioBlockStyle newBlockFooterStyle = ScenarioTemplateFacade::getTemplate().blockStyle(newBlockHeaderStyle.embeddableFooter());

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
			QString endOfSentancePattern = QString("([.]|[?]|[!]|[…]) %1$").arg(eventText);
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

	initEditorConnections();
}

void ScenarioTextEdit::initEditorConnections()
{
	if (m_document != 0) {
		connect(m_document, SIGNAL(contentsChange(int,int,int)),
				this, SLOT(aboutCorrectAdditionalCursors(int,int,int)));
		connect(m_document, SIGNAL(beforePatchApply()), this, SLOT(aboutSaveEditorState()));
		connect(m_document, SIGNAL(afterPatchApply()), this, SLOT(aboutLoadEditorState()));
		connect(m_document, SIGNAL(reviewChanged()), this, SIGNAL(reviewChanged()));
	}
}

void ScenarioTextEdit::removeEditorConnections()
{
	if (m_document != 0) {
		disconnect(m_document, SIGNAL(contentsChange(int,int,int)),
				   this, SLOT(aboutCorrectAdditionalCursors(int,int,int)));
		disconnect(m_document, SIGNAL(beforePatchApply()), this, SLOT(aboutSaveEditorState()));
		disconnect(m_document, SIGNAL(afterPatchApply()), this, SLOT(aboutLoadEditorState()));
		disconnect(m_document, SIGNAL(reviewChanged()), this, SIGNAL(reviewChanged()));
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

	updateShortcuts();
}

void ScenarioTextEdit::initConnections()
{
	//
	// При перемещении курсора может меняться стиль блока
	//
	connect(this, SIGNAL(cursorPositionChanged()), this, SIGNAL(currentStyleChanged()), Qt::UniqueConnection);
}
