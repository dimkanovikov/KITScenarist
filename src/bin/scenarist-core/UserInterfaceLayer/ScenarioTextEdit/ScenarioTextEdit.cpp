#include "ScenarioTextEdit.h"
#include "ScenarioTextEditPrivate.h"

#include "Handlers/KeyPressHandlerFacade.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h>
#include <BusinessLayer/ScenarioDocument/ScenarioReviewModel.h>

#include <3rd_party/Helpers/TextEditHelper.h>
#include <3rd_party/Helpers/ColorHelper.h>

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
#include <QTextDocumentFragment>
#include <QTimer>
#include <QWheelEvent>

using UserInterface::ScenarioTextEdit;
using UserInterface::ShortcutsManager;
using namespace BusinessLogic;

namespace {
    /**
     * @brief Флаг положения курсора
     * @note Используется для корректировки скрола при совместном редактировании
     */
    const char* CURSOR_RECT = "cursorRect";
}


ScenarioTextEdit::ScenarioTextEdit(QWidget* _parent) :
    CompletableTextEdit(_parent),
    m_mouseClicks(0),
    m_lastMouseClickTime(0),
    m_storeDataWhenEditing(true),
    m_showSceneNumbers(false),
    m_highlightCurrentLine(false),
    m_capitalizeFirstWord(false),
    m_correctDoubleCapitals(false),
    m_replaceThreeDots(false),
    m_smartQuotes(false),
    m_showSuggestionsInEmptyBlocks(false),
    m_textSelectionEnable(true),
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

    //
    // Удалим курсоры
    //
    m_additionalCursors.clear();
    m_additionalCursorsCorrected.clear();

    m_document = _document;
    setDocument(m_document);
    setHighlighterDocument(m_document);

    if (m_document != 0) {
        initEditor();

        TextEditHelper::beautifyDocument(m_document, m_replaceThreeDots, m_smartQuotes);
    }
}

void ScenarioTextEdit::addScenarioBlock(ScenarioBlockStyle::Type _blockType)
{
    textCursor().beginEditBlock();

    //
    // Если работаем в режиме поэпизодника и добавляется заголовок новой сцены, группы сцен,
    // или папки, то нужно сдвинуть курсор до конца текущей сцены
    //
    if (outlineMode()
        && scenarioBlockType() == ScenarioBlockStyle::SceneDescription
        && (_blockType == ScenarioBlockStyle::SceneHeading
            || _blockType == ScenarioBlockStyle::FolderHeader)) {
        ScenarioBlockStyle::Type currentBlockType = scenarioBlockType();
        while (!textCursor().atEnd()
               && currentBlockType != ScenarioBlockStyle::SceneHeading
               && currentBlockType != ScenarioBlockStyle::FolderFooter
               && currentBlockType != ScenarioBlockStyle::FolderHeader) {
            moveCursor(QTextCursor::NextBlock);
            moveCursor(QTextCursor::EndOfBlock);
            currentBlockType = scenarioBlockType();
        }
        //
        // Если дошли не до конца документа, а до начала новой сцены,
        // возвращаем курсор в конец предыдущего блока
        //
        if (!textCursor().atEnd()
            || currentBlockType == ScenarioBlockStyle::FolderFooter) {
            moveCursor(QTextCursor::PreviousBlock);
            moveCursor(QTextCursor::EndOfBlock);
        }
    }

    //
    // Если работает в режиме не поэпизодника, то блок нужно сместить после всех блоков
    // описания текущей сцены
    //
    if (!outlineMode()
        && _blockType != ScenarioBlockStyle::SceneDescription) {
        ScenarioBlockStyle::Type nextBlockType = ScenarioBlockStyle::forBlock(textCursor().block().next());
        while (!textCursor().atEnd()
               && nextBlockType == ScenarioBlockStyle::SceneDescription) {
            moveCursor(QTextCursor::NextBlock);
            moveCursor(QTextCursor::EndOfBlock);
            nextBlockType = ScenarioBlockStyle::forBlock(textCursor().block().next());
        }
    }

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

    textCursor().endEditBlock();
}

void ScenarioTextEdit::changeScenarioBlockType(ScenarioBlockStyle::Type _blockType, bool _forced)
{
    //
    // Если работаем в режиме поэпизодника, то запрещаем менять стиль блока на персонажей, реплики и пр.
    //
    if (outlineMode()
        && (_blockType == ScenarioBlockStyle::Action
            || _blockType == ScenarioBlockStyle::Character
            || _blockType == ScenarioBlockStyle::Parenthetical
            || _blockType == ScenarioBlockStyle::Dialogue
            || _blockType == ScenarioBlockStyle::Transition
            || _blockType == ScenarioBlockStyle::Note
            || _blockType == ScenarioBlockStyle::TitleHeader
            || _blockType == ScenarioBlockStyle::Title
            || _blockType == ScenarioBlockStyle::NoprintableText)) {
        return;
    }

    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    //
    // Если работаем в режиме поэпизодника и описание сцены меняется на заголовок сцены,
    // или папки, и после текущего блока не идёт блок с описанием сцены (т.е. мы внутри сцены с текстом),
    // то текущий блок нужно перенести в конец текущей сцены
    //
    const ScenarioBlockStyle::Type nextBlockType = ScenarioBlockStyle::forBlock(cursor.block().next());
    if (outlineMode()
        && scenarioBlockType() == ScenarioBlockStyle::SceneDescription
        && (_blockType == ScenarioBlockStyle::SceneHeading
            || _blockType == ScenarioBlockStyle::FolderHeader)
        && nextBlockType != ScenarioBlockStyle::SceneDescription) {
        //
        // Сохраним текст блока, а сам блок удалим
        //
        const QString blockText = cursor.block().text();
        moveCursor(QTextCursor::StartOfBlock);
        moveCursor(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        moveCursor(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        textCursor().deleteChar();

        //
        // Если блок был не в конце документа, перейдём к предыдущему
        //
        if (!textCursor().atEnd()) {
            moveCursor(QTextCursor::PreviousBlock);
            moveCursor(QTextCursor::EndOfBlock);
        }

        //
        // Вставляем блок
        //
        addScenarioBlock(_blockType);

        //
        // Восстанавливаем текст
        //
        textCursor().insertText(blockText);

        //
        // Уведомим о том, что стиль сменился
        //
        emit currentStyleChanged();
    }

    if (scenarioBlockType() != _blockType) {
        //
        // Нельзя сменить стиль заголовка титра и конечных элементов групп и папок
        //
        bool canChangeType =
                _forced
                || ((scenarioBlockType() != ScenarioBlockStyle::TitleHeader)
                    && (scenarioBlockType() != ScenarioBlockStyle::FolderFooter));

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

    //
    // Принудительно обновим ревизию блока
    //
    ScenarioTextDocument::updateBlockRevision(cursor);

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

bool ScenarioTextEdit::highlightCurrentLine() const
{
    return m_highlightCurrentLine;
}

void ScenarioTextEdit::setHighlightCurrentLine(bool _highlight)
{
    if (m_highlightCurrentLine != _highlight) {
        m_highlightCurrentLine = _highlight;
    }
}

void ScenarioTextEdit::setAutoReplacing(bool _capitalizeFirstWord, bool _correctDoubleCapitals,
    bool _replaceThreeDots, bool _smartQuotes)
{
    if (m_capitalizeFirstWord != _capitalizeFirstWord) {
        m_capitalizeFirstWord = _capitalizeFirstWord;
    }
    if (m_correctDoubleCapitals != _correctDoubleCapitals) {
        m_correctDoubleCapitals = _correctDoubleCapitals;
    }
    if (m_replaceThreeDots != _replaceThreeDots) {
        m_replaceThreeDots = _replaceThreeDots;
    }
    if (m_smartQuotes != _smartQuotes) {
        m_smartQuotes = _smartQuotes;
    }
}

void ScenarioTextEdit::setShowSuggestionsInEmptyBlocks(bool _show)
{
    if (m_showSuggestionsInEmptyBlocks != _show) {
        m_showSuggestionsInEmptyBlocks = _show;
    }
}

void ScenarioTextEdit::setTextSelectionEnable(bool _enable)
{
    if (m_textSelectionEnable != _enable) {
        m_textSelectionEnable = _enable;
    }
}

bool ScenarioTextEdit::outlineMode() const
{
    return m_document == 0 ? true : m_document->outlineMode();
}

void ScenarioTextEdit::setOutlineMode(bool _outlineMode)
{
    if (m_document != 0) {
        m_document->setOutlineMode(_outlineMode);
        relayoutDocument();

        //
        // Убедимся, что курсор виден, а так же делаем так, чтобы он не скакал по экрану
        //
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
        ensureCursorVisible();
    }
}

QList<ScenarioBlockStyle::Type> ScenarioTextEdit::visibleBlocksTypes() const
{
    return m_document == 0 ? QList<ScenarioBlockStyle::Type>() : m_document->visibleBlocksTypes();
}

void ScenarioTextEdit::updateShortcuts()
{
    m_shortcutsManager->update();
}

QString ScenarioTextEdit::shortcut(ScenarioBlockStyle::Type _forType) const
{
    return m_shortcutsManager->shortcut(_forType);
}

QMenu* ScenarioTextEdit::createContextMenu(const QPoint& _pos, QWidget* _parent)
{
    //
    // Формируем стандартное меню, чтобы добраться до действий отмены и повтора последнего действия
    // и присоединить их к собственным функциям повтора/отмены послденего действия
    //
    QMenu* menu = CompletableTextEdit::createContextMenu(_pos, _parent);
    foreach (QAction* menuAction, menu->findChildren<QAction*>()) {
        if (menuAction->text().endsWith(QKeySequence(QKeySequence::Undo).toString(QKeySequence::NativeText))) {
            menuAction->disconnect();
            connect(menuAction, &QAction::triggered, this, &ScenarioTextEdit::undoRequest);
            menuAction->setEnabled(m_document->isUndoAvailableReimpl());
        } else if (menuAction->text().endsWith(QKeySequence(QKeySequence::Redo).toString(QKeySequence::NativeText))) {
            menuAction->disconnect();
            connect(menuAction, &QAction::triggered, this, &ScenarioTextEdit::redoRequest);
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

void ScenarioTextEdit::scrollToAdditionalCursor(int _additionalCursorIndex)
{
    QTextCursor cursor(m_document);
    m_document->setCursorPosition(cursor, (m_additionalCursorsCorrected.begin() + _additionalCursorIndex).value());
    ensureCursorVisible(cursor);
}

void ScenarioTextEdit::keyPressEvent(QKeyEvent* _event)
{
    //
    // Отмену и повтор последнего действия, делаем без последующей обработки
    //
    // Если так не делать, то это приведёт к вставке странных символов, которые непонятно откуда берутся :(
    // Например:
    // 1. после реплики идёт время и место
    // 2. вставляем после реплики описание действия
    // 3. отменяем последнее действие
    // 4. в последующем времени и месте появляется символ "кружочек со стрелочкой"
    //
    // FIXME: разобраться
    //
    if (_event == QKeySequence::Undo
        || _event == QKeySequence::Redo) {
        if (_event == QKeySequence::Undo) {
            emit undoRequest();
        }
        else if (_event == QKeySequence::Redo) {
            emit redoRequest();
        }
        _event->accept();
        return;
    }

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
    handler->prepareForHandle(_event);

    //
    // Отправить событие в базовый класс
    //
    if (handler->needSendEventToBaseClass()) {
        if (!keyPressEventReimpl(_event)) {
            SpellCheckTextEdit::keyPressEvent(_event);
        }

        updateEnteredText(_event);

        TextEditHelper::beautifyDocument(textCursor(), _event->text(), m_replaceThreeDots, m_smartQuotes);
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

    //
    // Подготовим следующий блок к обработке
    //
    if (handler->needPrehandle()) {
        handler->prehandle();
    }
}

void ScenarioTextEdit::inputMethodEvent(QInputMethodEvent *_event)
{
    CompletableTextEdit::inputMethodEvent(_event);

    if (!_event->commitString().isEmpty()) {
        QKeyEvent keyEvent(QKeyEvent::KeyPress, Qt::Key_unknown, Qt::NoModifier, _event->commitString());
        updateEnteredText(&keyEvent);
    }

}

bool ScenarioTextEdit::keyPressEventReimpl(QKeyEvent* _event)
{
    bool isEventHandled = true;
    //
    // Переопределяем
    //
    // ... перевод курсора к следующему символу
    //
    if (_event == QKeySequence::MoveToNextChar) {
        moveCursor(QTextCursor::NextCharacter);
        while (!textCursor().atEnd()
               && !textCursor().block().isVisible()) {
            moveCursor(QTextCursor::NextBlock);
        }
    }
    //
    // ... перевод курсора к предыдущему символу
    //
    else if (_event == QKeySequence::MoveToPreviousChar) {
        moveCursor(QTextCursor::PreviousCharacter);
        while (!textCursor().atStart()
               && !textCursor().block().isVisible()) {
            moveCursor(QTextCursor::StartOfBlock);
            moveCursor(QTextCursor::PreviousCharacter);
        }
    }
    //
    // ... перевод курсора к концу строки
    //
    else if (_event == QKeySequence::MoveToEndOfLine
             || _event == QKeySequence::SelectEndOfLine) {
        QTextCursor cursor = textCursor();
        const int startY = cursorRect(cursor).y();
        const QTextCursor::MoveMode keepAncor =
            _event->modifiers().testFlag(Qt::ShiftModifier)
                ? QTextCursor::KeepAnchor
                : QTextCursor::MoveAnchor;
        while (!cursor.atBlockEnd()) {
            cursor.movePosition(QTextCursor::NextCharacter, keepAncor);
            if (cursorRect(cursor).y() > startY) {
                cursor.movePosition(QTextCursor::PreviousCharacter, keepAncor);
                setTextCursor(cursor);
                break;
            }
        }
        setTextCursor(cursor);
    }
    //
    // ... перевод курсора к началу строки
    //
    else if (_event == QKeySequence::MoveToStartOfLine
             || _event == QKeySequence::SelectStartOfLine) {
        QTextCursor cursor = textCursor();
        const int startY = cursorRect(cursor).y();
        const QTextCursor::MoveMode keepAncor =
            _event->modifiers().testFlag(Qt::ShiftModifier)
                ? QTextCursor::KeepAnchor
                : QTextCursor::MoveAnchor;
        while (!cursor.atBlockStart()) {
            cursor.movePosition(QTextCursor::PreviousCharacter, keepAncor);
            if (cursorRect(cursor).y() < startY) {
                cursor.movePosition(QTextCursor::NextCharacter, keepAncor);
                setTextCursor(cursor);
                break;
            }
        }
        setTextCursor(cursor);
    }
    //
    // Поднятие/опускание регистра букв
    // Работает в три шага:
    // 1. ВСЕ ЗАГЛАВНЫЕ
    // 2. Первая заглавная
    // 3. все строчные
    //
    else if (_event->modifiers().testFlag(Qt::ControlModifier)
             && (_event->key() == Qt::Key_Up
                 || _event->key() == Qt::Key_Down)) {
        //
        // Нужно ли убирать выделение после операции
        //
        bool clearSelection = false;
        //
        // Если выделения нет, работаем со словом под курсором
        //
        QTextCursor cursor = textCursor();
        const int sourcePosition = cursor.position();
        if (!cursor.hasSelection()) {
            cursor.select(QTextCursor::WordUnderCursor);
            clearSelection = true;
        }

        const bool toUpper = _event->key() == Qt::Key_Up;
        const QString selectedText = cursor.selectedText();
        const QChar firstChar = selectedText.at(0);
        const bool firstToUpper = firstChar.toUpper() != firstChar;
        const bool textInUpper = (selectedText.length() > 1) && (selectedText.toUpper() == selectedText);
        const int fromPosition = qMin(cursor.selectionStart(), cursor.selectionEnd());
        const int toPosition = qMax(cursor.selectionStart(), cursor.selectionEnd());
        for (int position = fromPosition; position < toPosition; ++position) {
            cursor.setPosition(position);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            if (toUpper) {
                if (firstToUpper) {
                    cursor.insertText(position == fromPosition ? cursor.selectedText().toUpper() : cursor.selectedText().toLower());
                } else {
                    cursor.insertText(cursor.selectedText().toUpper());
                }
            } else {
                if (textInUpper) {
                    cursor.insertText(position == fromPosition ? cursor.selectedText().toUpper() : cursor.selectedText().toLower());
                } else {
                    cursor.insertText(cursor.selectedText().toLower());
                }
            }
        }

        if (clearSelection) {
            cursor.setPosition(sourcePosition);
        } else {
            cursor.setPosition(fromPosition);
            cursor.setPosition(toPosition, QTextCursor::KeepAnchor);
        }
        setTextCursor(cursor);
    }
    //
    // Делаем собственную обработку операции вставить, т.к. стандартная всегда
    // уводит полосу прокрутки в начало
    //
    else if (_event == QKeySequence::Paste) {
        const int lastVBarValue = verticalScrollBar()->value();
        paste();
        verticalScrollBar()->setValue(lastVBarValue);
    }
#ifdef Q_OS_MAC
    //
    // Особая комбинация для вставки точки независимо от раскладки
    //
    else if (_event->modifiers().testFlag(Qt::MetaModifier)
             && _event->modifiers().testFlag(Qt::AltModifier)
             && _event->key() == Qt::Key_Period) {
        insertPlainText(".");
    }
    //
    // Особая комбинация для вставки запятой независимо от раскладки
    //
    else if (_event->modifiers().testFlag(Qt::MetaModifier)
             && _event->modifiers().testFlag(Qt::AltModifier)
             && _event->key() == Qt::Key_Comma) {
        insertPlainText(",");
    }
#endif
    else {
        isEventHandled = false;
    }

    return isEventHandled;
}

void ScenarioTextEdit::paintEvent(QPaintEvent* _event)
{
    //
    // Ширина области курсора, для отображения имени автора курсора
    //
    const unsigned cursorAreaWidth = 20;

    //
    // Подсветка строки
    //
    if (m_highlightCurrentLine) {
        const int width = viewport()->width();
        const QRect cursorR = cursorRect();
        const QRect highlightRect(0, cursorR.top(), width, cursorR.height());
        QColor lineColor = palette().highlight().color().lighter();
        lineColor.setAlpha(100);

        QPainter painter(viewport());
        painter.save();
        painter.fillRect(highlightRect, lineColor);
        painter.restore();
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
            const int pageLeft = 0;
            const int textLeft = document()->rootFrame()->frameFormat().leftMargin() - 10;
            const int textRight = viewport()->width() + horizontalScrollBar()->maximum()
                                  - document()->rootFrame()->frameFormat().rightMargin() + 10;


            QPainter painter(viewport());
            clipPageDecorationRegions(&painter);

            QTextBlock block = document()->begin();
            const QRectF viewportGeometry = viewport()->geometry();
            const int leftDelta = -horizontalScrollBar()->value();
            int lastBlockBottom = 0;
            int colorRectWidth = 0;
            QColor lastSceneColor;

            QTextCursor cursor(document());
            while (block.isValid()) {
                //
                // Стиль текущего блока
                //
                const ScenarioBlockStyle::Type blockType = ScenarioBlockStyle::forBlock(block);

                if (block.isVisible()) {
                    cursor.setPosition(block.position());
                    const QRect cursorR = cursorRect(cursor);
                    cursor.movePosition(QTextCursor::EndOfBlock);
                    const QRect cursorREnd = cursorRect(cursor);

                    //
                    // Определим цвет
                    //
                    if (blockType == ScenarioBlockStyle::SceneHeading
                        || blockType == ScenarioBlockStyle::FolderHeader) {
                        lastBlockBottom = cursorR.top() - (cursorR.height() / 2);
                        colorRectWidth = QFontMetrics(cursor.charFormat().font()).width(".");
                        lastSceneColor = QColor();
                        if (ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(block.userData())) {
                            if (!info->colors().isEmpty()) {
                                lastSceneColor = QColor(info->colors().split(";").first());
                            }
                        }
                    }

                    //
                    // Нарисуем цвет
                    //
                    if (lastSceneColor.isValid()) {
                        painter.save();
                        QPointF topLeft(textRight + leftDelta, lastBlockBottom);
                        QPointF bottomRight(textRight + colorRectWidth + leftDelta, cursorREnd.bottom());
                        QRectF rect(topLeft, bottomRight);
                        painter.setPen(lastSceneColor);
                        painter.setBrush(lastSceneColor);
                        painter.drawRect(rect);
                        painter.restore();
                    }

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
                            QPointF topLeft(pageLeft + leftDelta, cursorR.top());
                            QPointF bottomRight(textLeft + leftDelta, cursorR.bottom() + 2);
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
                                && blockType == ScenarioBlockStyle::SceneHeading) {
                                //
                                // Определим номер сцены
                                //
                                QTextBlockUserData* textBlockData = block.userData();
                                if (ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData)) {
                                    const QString sceneNumber = QString::number(info->sceneNumber()) + ".";

                                    //
                                    // Определим область для отрисовки и выведем номер сцены в редактор
                                    //
                                    QPointF topLeft(pageLeft + leftDelta, cursorR.top());
                                    QPointF bottomRight(textLeft + leftDelta, cursorR.bottom());
                                    QRectF rect(topLeft, bottomRight);
                                    painter.setFont(cursor.charFormat().font());
                                    painter.drawText(rect, Qt::AlignRight | Qt::AlignTop, sceneNumber);
                                }
                            }
                        }
                    }

                    lastBlockBottom = cursorREnd.bottom();
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
                        painter.fillRect(cursorR, ColorHelper::cursorColor(cursorIndex));

                        //
                        // ... декорируем
                        //
                        {
                            //
                            // Если мышь около него, то выводим имя соавтора
                            //
                            QRect extandedCursorR = cursorR;
                            extandedCursorR.setLeft(extandedCursorR.left() - cursorAreaWidth/2);
                            extandedCursorR.setWidth(cursorAreaWidth);
                            if (extandedCursorR.contains(mouseCursorPos)) {
                                const QRect usernameRect(
                                    cursorR.left() - 1,
                                    cursorR.top() - painter.fontMetrics().height() - 2,
                                    painter.fontMetrics().width(username) + 2,
                                    painter.fontMetrics().height() + 2);
                                painter.fillRect(usernameRect, ColorHelper::cursorColor(cursorIndex));
                                painter.drawText(usernameRect, Qt::AlignCenter, username);
                            }
                            //
                            // Если нет, то рисуем небольшой квадратик
                            //
                            else {
                                painter.fillRect(cursorR.left() - 2, cursorR.top() - 5, 5, 5,
                                    ColorHelper::cursorColor(cursorIndex));
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
    if (!selectBlockOnTripleClick(_event)) {
        CompletableTextEdit::mousePressEvent(_event);
    }
}

void ScenarioTextEdit::mouseDoubleClickEvent(QMouseEvent* _event)
{
    if (!selectBlockOnTripleClick(_event)) {
        CompletableTextEdit::mouseDoubleClickEvent(_event);
    }
}

void ScenarioTextEdit::mouseMoveEvent(QMouseEvent* _event)
{
    if (m_textSelectionEnable) {
        CompletableTextEdit::mouseMoveEvent(_event);
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

    mimeData->setData("text/plain", textCursor().selection().toPlainText().toUtf8());
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

bool ScenarioTextEdit::canComplete() const
{
    bool result = true;
    //
    // Если нельзя показывать в пустих блоках, проверяем не пуст ли блок
    //
    if (!m_showSuggestionsInEmptyBlocks) {
        result = textCursor().block().text().isEmpty() == false;
    }

    return result;
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
    //
    // FIXME: задумано это для того, чтобы твой курсор не смещался вниз, если вверху текст редактирует соавтор,
    //		  но при отмене собственных изменений работает ужасно, поэтому пока отключим данный код.
    //		  Возможно это из-за того, что при отмене собственных изменений курсор не отправляется к
    //		  месту, где заканчивается изменение, а остаётся в своей позиции. Но тогда нужно как-то
    //		  разводить собственные патчи и патчи соавторов
    //
//	QApplication::processEvents();
//	const QRect prevCursorRect = property(CURSOR_RECT).toRect();
//	QRect currentCursorRect = cursorRect();

//	//
//	// Корректируем позицию курсора, пока
//	// - не восстановим предыдущее значение
//	// - не сдвинем прокрутку в самый верх
//	// - не сдвинем прокрутку в самый низ
//	//
//	while (prevCursorRect.y() != currentCursorRect.y()
//		   && verticalScrollBar()->value() != verticalScrollBar()->minimum()
//		   && verticalScrollBar()->value() != verticalScrollBar()->maximum()) {
//		int verticalDelta = 0;
//		if (prevCursorRect.y() < currentCursorRect.y()) {
//			verticalDelta = 1;
//		} else {
//			verticalDelta = -1;
//		}
//		verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
//		currentCursorRect = cursorRect();
//		qDebug() << cursorRect() << prevCursorRect;
//	}

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
                    //
                    // Запомним стиль предыдущего блока
                    //
                    cursor.movePosition(QTextCursor::PreviousBlock);
                    ScenarioBlockStyle::Type previousBlockType = ScenarioBlockStyle::forBlock(cursor.block());
                    cursor.movePosition(QTextCursor::NextBlock);
                    //
                    // Удаляем закрывающий блок
                    //
                    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                    cursor.deleteChar();
                    cursor.deletePreviousChar();
                    //
                    // Восстановим стиль предыдущего блока
                    //
                    if (ScenarioBlockStyle::forBlock(cursor.block()) != previousBlockType) {
                        QTextCursor lastTextCursor = textCursor();
                        setTextCursor(cursor);
                        applyScenarioTypeToBlockText(previousBlockType);
                        setTextCursor(lastTextCursor);
                    }
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
        headerCursor.movePosition(QTextCursor::PreviousCharacter);
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
    // + сохраняем форматирование выделений
    //
    {
        cursor.movePosition(QTextCursor::StartOfBlock);

        //
        // Если в блоке есть выделения, обновляем цвет только тех частей, которые не входят в выделения
        //
        QTextBlock currentBlock = cursor.block();
        if (!currentBlock.textFormats().isEmpty()) {
            foreach (const QTextLayout::FormatRange& range, currentBlock.textFormats()) {
                if (!range.format.boolProperty(ScenarioBlockStyle::PropertyIsReviewMark)) {
                    cursor.setPosition(currentBlock.position() + range.start);
                    cursor.setPosition(cursor.position() + range.length, QTextCursor::KeepAnchor);
                    cursor.mergeCharFormat(newBlockStyle.charFormat());
                }
            }
            cursor.movePosition(QTextCursor::EndOfBlock);
        }
        //
        // Если выделений нет, обновляем блок целиком
        //
        else {
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            cursor.mergeCharFormat(newBlockStyle.charFormat());
        }

        cursor.clearSelection();
    }

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
    // Принудительно обновим ревизию блока
    //
    ScenarioTextDocument::updateBlockRevision(cursor);

    //
    // Вставим заголовок, если необходимо
    //
    if (newBlockStyle.hasHeader()) {
        ScenarioBlockStyle headerStyle = ScenarioTemplateFacade::getTemplate().blockStyle(newBlockStyle.headerType());

        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.insertBlock();
        cursor.movePosition(QTextCursor::PreviousCharacter);

        cursor.setBlockCharFormat(headerStyle.charFormat());
        cursor.setBlockFormat(headerStyle.blockFormat());

        cursor.insertText(newBlockStyle.header());
    }

    //
    // Для заголовка папки нужно создать завершение, захватив всё содержимое сцены
    //
    if (newBlockStyle.isEmbeddableHeader()) {
        ScenarioBlockStyle footerStyle = ScenarioTemplateFacade::getTemplate().blockStyle(newBlockStyle.embeddableFooter());

        //
        // Запомним позицию курсора
        //
        int lastCursorPosition = textCursor().position();

        //
        // Ищем конец сцены
        //
        do {
            cursor.movePosition(QTextCursor::EndOfBlock);
            cursor.movePosition(QTextCursor::NextBlock);
        } while (!cursor.atEnd()
                 && ScenarioBlockStyle::forBlock(cursor.block()) != ScenarioBlockStyle::SceneHeading
                 && ScenarioBlockStyle::forBlock(cursor.block()) != ScenarioBlockStyle::FolderHeader
                 && ScenarioBlockStyle::forBlock(cursor.block()) != ScenarioBlockStyle::FolderFooter);

        //
        // Если забежали на блок следующей сцены, вернёмся на один символ назад
        //
        if (!cursor.atEnd() && cursor.atBlockStart()) {
            cursor.movePosition(QTextCursor::PreviousCharacter);
        }

        //
        // Когда дошли до конца сцены, вставляем закрывающий блок
        //
        cursor.insertBlock();
        cursor.setBlockCharFormat(footerStyle.charFormat());
        cursor.setBlockFormat(footerStyle.blockFormat());

        //
        // т.к. вставлен блок, нужно вернуть курсор на место
        //
        cursor.setPosition(lastCursorPosition);
        setTextCursor(cursor);

        //
        // Эмулируем нажатие кнопки клавиатуры, чтобы обновился футер стиля
        //
        QKeyEvent empyEvent(QEvent::KeyPress, -1, Qt::NoModifier);
        keyPressEvent(&empyEvent);
    }

    cursor.endEditBlock();
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
    // ... текст после курсора
    QString cursorForwardText = currentBlockText.mid(cursor.positionInBlock());
    // ... стиль шрифта блока
    QTextCharFormat currentCharFormat = currentBlock.charFormat();
    // ... текст события
    QString eventText = _event->text();

    //
    // Если был введён текст
    //
    if (!eventText.isEmpty()) {
        //
        // Определяем необходимость установки верхнего регистра для первого символа блока
        //
        if (cursorBackwardText != " "
            && (cursorBackwardText == eventText
                || cursorBackwardText == (currentCharFormat.stringProperty(ScenarioBlockStyle::PropertyPrefix)
                                          + eventText))) {
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
        // Иначе обрабатываем по обычным правилам
        //
        else {
            //
            // Если перед нами конец предложения и не сокращение и после курсора нет текста
            //
            QString endOfSentancePattern = QString("([.]|[?]|[!]|[…]) %1$").arg(eventText);
            if (m_capitalizeFirstWord
                && cursorBackwardText.contains(QRegularExpression(endOfSentancePattern))
                && !stringEndsWithAbbrev(cursorBackwardText)
                && cursorForwardText.isEmpty()) {
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
            // Исправляем проблему ДВойных ЗАглавных
            //
            else if (m_correctDoubleCapitals) {
                QString right3Characters = cursorBackwardText.right(3).simplified();

                //
                // Если две из трёх последних букв находятся в верхнем регистре, то это наш случай
                //
                if (!right3Characters.contains(" ")
                    && right3Characters.length() == 3
                    && right3Characters != right3Characters.toUpper()
                    && right3Characters.left(2) == right3Characters.left(2).toUpper()
                    && right3Characters.left(2).at(0).isLetter()
                    && right3Characters.left(2).at(1).isLetter()
                    && eventText != eventText.toUpper()) {
                    //
                    // Сделаем предпоследнюю букву строчной
                    //
                    QString correctedText = right3Characters;
                    correctedText[correctedText.length() - 2] = correctedText[correctedText.length() - 2].toLower();

                    //
                    // Стираем предыдущий введённый текст
                    //
                    for (int repeats = 0; repeats < correctedText.length(); ++repeats) {
                        cursor.deletePreviousChar();
                    }

                    //
                    // Выводим необходимый
                    //
                    cursor.insertText(correctedText);
                    setTextCursor(cursor);
                }
            }

            //
            // Если была попытка ввести несколько пробелов подряд, или пробел в начале строки,
            // удаляем этот лишний пробел
            //
            if (cursorBackwardText == " "
                || cursorBackwardText.endsWith("  ")) {
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

bool ScenarioTextEdit::selectBlockOnTripleClick(QMouseEvent* _event)
{
    if (_event->button() == Qt::LeftButton) {
        const qint64 curMouseClickTime = QDateTime::currentMSecsSinceEpoch();
        const qint64 timeDelta = curMouseClickTime - m_lastMouseClickTime;
        if (timeDelta <= (QApplication::styleHints()->mouseDoubleClickInterval())) {
            m_mouseClicks += 1;
        } else {
            m_mouseClicks = 1;
        }
        m_lastMouseClickTime = curMouseClickTime;
    }

    //
    // Тройной клик обрабатываем самостоятельно
    //
    if (m_mouseClicks > 2) {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        setTextCursor(cursor);
        _event->accept();
        return true;
    }

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
        applyScenarioTypeToBlockText(ScenarioBlockStyle::SceneHeading);
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
    connect(this, &ScenarioTextEdit::cursorPositionChanged, this, &ScenarioTextEdit::currentStyleChanged, Qt::UniqueConnection);
}
