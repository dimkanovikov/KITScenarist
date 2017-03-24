#include "ScenarioTextEditWidget.h"

#include "ScenarioFastFormatWidget.h"
#include "ScenarioReviewPanel.h"
#include "ScenarioReviewView.h"

#include <UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEdit.h>
#include <UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEditHelpers.h>

#include <3rd_party/Helpers/ShortcutHelper.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/ScalableWrapper/ScalableWrapper.h>
#include <3rd_party/Widgets/SearchWidget/SearchWidget.h>
#include <3rd_party/Widgets/TabBar/TabBar.h>
#include <3rd_party/Widgets/WAF/Animation/Animation.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <QApplication>
#include <QComboBox>
#include <QCryptographicHash>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QShortcut>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTextBlock>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>

using UserInterface::ScenarioTextEditWidget;
using UserInterface::ScenarioReviewPanel;
using UserInterface::ScenarioReviewView;
using UserInterface::ScenarioTextEdit;
using BusinessLogic::ScenarioTemplateFacade;
using BusinessLogic::ScenarioTemplate;
using BusinessLogic::ScenarioBlockStyle;
using BusinessLogic::ScenarioTextBlockInfo;

namespace {
    /**
     * @brief Нерабочая позиция курсора
     */
    const int INVALID_CURSOR_POSITION = -1;

    /**
     * @brief Исходная позиция курсора после загрузки программы
     */
    int initCursorPosition = INVALID_CURSOR_POSITION;
}


ScenarioTextEditWidget::ScenarioTextEditWidget(QWidget* _parent) :
    QFrame(_parent),
    m_editor(new ScenarioTextEdit(this)),
    m_editorWrapper(new ScalableWrapper(m_editor, this)),
    m_toolbar(new QWidget(this)),
    m_outline(new FlatButton(this)),
    m_textStyles(new QComboBox(this)),
    m_undo(new FlatButton(this)),
    m_redo(new FlatButton(this)),
    m_search(new FlatButton(this)),
    m_fastFormat(new FlatButton(this)),
    m_review(new ScenarioReviewPanel(m_editor, this)),
    m_duration(new QLabel(this)),
    m_countersInfo(new QLabel(this)),
    m_searchLine(new SearchWidget(this, true)),
    m_fastFormatWidget(new ScenarioFastFormatWidget(this)),
    m_reviewView(new ScenarioReviewView(this))
{
    initView();
    initConnections();
    initStyleSheet();
}

QWidget* ScenarioTextEditWidget::toolbar() const
{
    return m_toolbar;
}

BusinessLogic::ScenarioTextDocument* ScenarioTextEditWidget::scenarioDocument() const
{
    return qobject_cast<BusinessLogic::ScenarioTextDocument*>(m_editor->document());
}

void ScenarioTextEditWidget::setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document, bool _isDraft)
{
    removeEditorConnections();

    m_editor->setScenarioDocument(_document);
    m_editor->setWatermark(_isDraft ? tr("DRAFT") : QString::null);

    initEditorConnections();
}

void ScenarioTextEditWidget::setDuration(const QString& _duration)
{
    m_duration->setText(_duration);
}

void ScenarioTextEditWidget::setCountersInfo(const QString& _counters)
{
    m_countersInfo->setText(_counters);
}

void ScenarioTextEditWidget::setShowScenesNumbers(bool _show)
{
    m_editor->setShowSceneNumbers(_show);
}

void ScenarioTextEditWidget::setHighlightCurrentLine(bool _highlight)
{
    m_editor->setHighlightCurrentLine(_highlight);
}

void ScenarioTextEditWidget::setAutoReplacing(bool _capitalizeFirstWord,
    bool _correctDoubleCapitals, bool _replaceThreeDots, bool _smartQuotes)
{
    m_editor->setAutoReplacing(_capitalizeFirstWord, _correctDoubleCapitals, _replaceThreeDots,
        _smartQuotes);
}

void ScenarioTextEditWidget::setUsePageView(bool _use)
{
    //
    // Установка постраничного режима так же тянет за собой ряд настроек
    //
    QMarginsF pageMargins(15, 5, 5, 5);
    Qt::Alignment pageNumbersAlign;
    if (_use) {
        m_editor->setPageFormat(ScenarioTemplateFacade::getTemplate().pageSizeId());
        pageMargins = ScenarioTemplateFacade::getTemplate().pageMargins();
        pageNumbersAlign = ScenarioTemplateFacade::getTemplate().numberingAlignment();
    }

    m_editor->setUsePageMode(_use);
    m_editor->setPageMargins(pageMargins);
    m_editor->setPageNumbersAlignment(pageNumbersAlign);

    //
    // В дополнение установим шрифт по умолчанию для документа (шрифтом будет рисоваться нумерация)
    //
    m_editor->document()->setDefaultFont(
        ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::Action).font());
}

void ScenarioTextEditWidget::setUseSpellChecker(bool _use)
{
    m_editor->setUseSpellChecker(_use);
}

void ScenarioTextEditWidget::setShowSuggestionsInEmptyBlocks(bool _show)
{
    m_editor->setShowSuggestionsInEmptyBlocks(_show);
}

void ScenarioTextEditWidget::setSpellCheckLanguage(int _language)
{
    m_editor->setSpellCheckLanguage((SpellChecker::Language)_language);
}

void ScenarioTextEditWidget::setTextEditColors(const QColor& _textColor, const QColor& _backgroundColor)
{
    m_editor->viewport()->setStyleSheet(QString("color: %1; background-color: %2;").arg(_textColor.name(), _backgroundColor.name()));
    m_editor->setStyleSheet(QString("#scenarioEditor { color: %1; }").arg(_textColor.name()));
}

void ScenarioTextEditWidget::setTextEditZoomRange(qreal _zoomRange)
{
    m_editorWrapper->setZoomRange(_zoomRange);
}

int ScenarioTextEditWidget::cursorPosition() const
{
    return m_editor->textCursor().position();
}

void ScenarioTextEditWidget::setCursorPosition(int _position)
{
    //
    // Если виджет пока ещё не видно, откладываем событие назначения позиции до этого момента.
    // Делаем это потому что иногда установка курсора происходит до первой отрисовки обёртки
    // масштабирования, что приводит в свою очередь к тому, что полосы прокрутки остаются в начале.
    //
    if (!isVisible()) {
        //
        // ... но делаем это только в случае, когда курсор устанавливается в виджет в первый раз,
        //     если позиция курсора потом меняется ещё раз, то устанавливаем её
        //
        if (initCursorPosition == INVALID_CURSOR_POSITION) {
            initCursorPosition = _position;
        }
        if (initCursorPosition == _position) {
            QTimer::singleShot(300, Qt::PreciseTimer, [=] {
                setCursorPosition(_position);
            });
            return;
        }
    }

    //
    // Устанавливаем позицию курсора
    //
    QTextCursor cursor = m_editor->textCursor();

    //
    // Если это новая позиция
    //
    if (cursor.position() != _position) {
        //
        // Устанавливаем реальную позицию
        //
        if (_position < m_editor->document()->characterCount()) {
            cursor.setPosition(_position);
        } else {
            cursor.movePosition(QTextCursor::End);
        }

        //
        // Возвращаем курсор в поле зрения
        //
        m_editor->ensureCursorVisible(cursor);
        m_editorWrapper->setFocus();
    }
    //
    // Если нужно обновить в текущей позиции курсора просто имитируем отправку сигнала
    //
    else {
        emit m_editor->cursorPositionChanged();
    }
}

void ScenarioTextEditWidget::setCurrentBlockType(int _type)
{
    m_editor->changeScenarioBlockType((BusinessLogic::ScenarioBlockStyle::Type)_type);
}

void ScenarioTextEditWidget::addItem(int _position, int _type, const QString& _header, const QString& _title,
    const QColor& _color, const QString& _description)
{
    QTextCursor cursor = m_editor->textCursor();
    cursor.beginEditBlock();

    cursor.setPosition(_position);
    m_editor->setTextCursor(cursor);
    ScenarioBlockStyle::Type type = (ScenarioBlockStyle::Type)_type;
    bool blockWasAdded = false;
    //
    // Если в позиции пустой блок, изменим его
    //
    if (cursor.block().text().isEmpty()) {
        m_editor->changeScenarioBlockType(type);
        blockWasAdded = false;
    }
    //
    // В противном случае добавим новый
    //
    else {
        m_editor->addScenarioBlock(type);
        blockWasAdded = true;
    }

    //
    // Устанавливаем текст в блок
    //
    m_editor->insertPlainText(!_header.isEmpty() ? _header : _title);

    //
    // Устанавливаем цвет и описание в параметры сцены
    //
    cursor = m_editor->textCursor();
    QTextBlockUserData* textBlockData = cursor.block().userData();
    ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
    if (info == 0) {
        info = new ScenarioTextBlockInfo;
    }
    info->setTitle(!_title.isEmpty() ? _title : _header);
    if (_color.isValid()) {
        info->setColors(_color.name());
    }
    info->setDescription(_description);
    cursor.block().setUserData(info);
    //
    // ... и в сам текст
    //
    if (!_description.isEmpty()) {
        m_editor->addScenarioBlock(ScenarioBlockStyle::SceneDescription);
        QTextDocument doc;
        doc.setHtml(_description);
        m_editor->insertPlainText(doc.toPlainText());
    }

    //
    // Если это группирующий блок, то вставим и закрывающий текст
    //
    if (ScenarioTemplateFacade::getTemplate().blockStyle(type).isEmbeddableHeader()) {
        ScenarioBlockStyle::Type footerType = ScenarioTemplateFacade::getTemplate().blockStyle(type).embeddableFooter();
        cursor = m_editor->textCursor();
        //
        // Но сначала дойдём до подготовленного заранее закрывающего блока
        //
        while (!cursor.atEnd()
               && ScenarioBlockStyle::forBlock(cursor.block()) != footerType) {
            cursor.movePosition(QTextCursor::NextBlock);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        }
        cursor.insertText(Helpers::footerText(!_header.isEmpty() ? _header : _title));
    }

    //
    // А теперь скроем блоки с описанием сцены, если мы не в режиме битов
    //
    const bool isSceneDescriptionVisible = m_editor->visibleBlocksTypes().contains(ScenarioBlockStyle::SceneDescription);
    if (!isSceneDescriptionVisible) {
        cursor.setPosition(_position);
        cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, blockWasAdded ? 2 : 1);
        while (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneDescription
               && !cursor.atEnd()) {
            cursor.block().setVisible(isSceneDescriptionVisible);
            cursor.movePosition(QTextCursor::EndOfBlock);
            cursor.movePosition(QTextCursor::NextBlock);
        }
    }

    cursor.endEditBlock();

    //
    // Фокусируемся на редакторе
    //
    m_editorWrapper->setFocus();
    m_editor->ensureCursorVisible(cursor);
}

void ScenarioTextEditWidget::editItem(int _startPosition, int _endPosition, int _type,
    const QString& _title, const QString& _colors, const QString& _description)
{
    QTextCursor cursor = m_editor->textCursor();
    cursor.beginEditBlock();

    //
    // Идём в начало сцены
    //
    cursor.setPosition(_startPosition);
    m_editor->setTextCursor(cursor);

    //
    // Проверяем тип блока, если нужно - меняем на новый
    //
    ScenarioBlockStyle::Type type = (ScenarioBlockStyle::Type)_type;
    if (ScenarioBlockStyle::forBlock(cursor.block()) != type) {
        m_editor->changeScenarioBlockType(type);
    }

    //
    // Если не задан заголовок, установим его таким же, как и название
    //
    if (cursor.block().text().isEmpty()) {
        cursor.insertText(_title);
    }

    //
    // Устанавливаем название блока и описание
    //
    if (ScenarioTextBlockInfo* blockInfo = dynamic_cast<ScenarioTextBlockInfo*>(cursor.block().userData())) {
        blockInfo->setTitle(_title);
        blockInfo->setColors(_colors);
        blockInfo->setDescription(_description);
        cursor.block().setUserData(blockInfo);
    }

    //
    // Обновляем описание в самом тексте
    //
    cursor.setPosition(_startPosition);
    //
    // ... сперва выделив старое описание, если после текущего блока есть другие блоки
    //
    if (cursor.movePosition(QTextCursor::NextBlock)) {
        if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneCharacters) {
            cursor.movePosition(QTextCursor::NextBlock);
        }
        while (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneDescription
               && cursor.position() <= _endPosition
               && !cursor.atEnd()) {
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
        }
    } else {
        cursor.movePosition(QTextCursor::EndOfBlock);
    }
    //
    // ... шаг назад, если до этого мы перескочили в следующий блок
    //
    if (cursor.atBlockStart()
        && ScenarioBlockStyle::forBlock(cursor.block()) != ScenarioBlockStyle::SceneDescription) {
        cursor.movePosition(QTextCursor::PreviousCharacter, cursor.hasSelection() ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
    }
    //
    // ... а потом вставив новое
    //
    if (cursor.hasSelection()) {
        cursor.removeSelectedText();
        m_editor->setTextCursor(cursor);
    } else {
        m_editor->setTextCursor(cursor);
        m_editor->addScenarioBlock(ScenarioBlockStyle::SceneDescription);
    }
    QTextDocument doc;
    doc.setHtml(_description);
    m_editor->insertPlainText(doc.toPlainText());

    //
    // А теперь скроем блоки с описанием сцены, если мы не в режиме битов
    //
    const bool isSceneDescriptionVisible = m_editor->visibleBlocksTypes().contains(ScenarioBlockStyle::SceneDescription);
    if (!isSceneDescriptionVisible) {
        cursor.setPosition(_startPosition);
        cursor.movePosition(QTextCursor::NextBlock);
        while (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneDescription
               && !cursor.atEnd()) {
            cursor.block().setVisible(isSceneDescriptionVisible);
            cursor.movePosition(QTextCursor::EndOfBlock);
            cursor.movePosition(QTextCursor::NextBlock);
        }
    }

    cursor.endEditBlock();
}

void ScenarioTextEditWidget::removeText(int _from, int _to)
{
    QTextCursor cursor = m_editor->textCursor();
    cursor.beginEditBlock();

    //
    // Стираем текст
    //
    cursor.setPosition(_from);
    cursor.setPosition(_to, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    //
    // Если остаётся пустой блок, стираем его тоже
    //
    if (cursor.block().text().isEmpty()) {
        if (cursor.atStart()) {
            cursor.deleteChar();
        } else {
            cursor.deletePreviousChar();
        }
    }

    cursor.endEditBlock();
}

void ScenarioTextEditWidget::updateStylesElements()
{
    //
    // Обновить выпадающий список стилей сценария
    //
    initStylesCombo();

    //
    // Обновить виджет быстрого форматирования
    //
    m_fastFormatWidget->reinitBlockStyles();
}

void ScenarioTextEditWidget::updateShortcuts()
{
    m_editor->updateShortcuts();
    updateStylesCombo();
}

void ScenarioTextEditWidget::setAdditionalCursors(const QMap<QString, int>& _cursors)
{
    m_editor->setAdditionalCursors(_cursors);
}

void ScenarioTextEditWidget::setCommentOnly(bool _isCommentOnly)
{
    m_textStyles->setEnabled(!_isCommentOnly);
    m_undo->setVisible(!_isCommentOnly);
    m_redo->setVisible(!_isCommentOnly);
    m_fastFormat->setVisible(!_isCommentOnly);
    m_editor->setReadOnly(_isCommentOnly);
    m_searchLine->setSearchOnly(_isCommentOnly);

    if (_isCommentOnly) {
        //
        // Закроем панель быстрого форматирования, если она была открыта
        //
        m_fastFormat->setChecked(false);
        m_fastFormatWidget->hide();
    }
}

void ScenarioTextEditWidget::scrollToAdditionalCursor(int _additionalCursorIndex)
{
    m_editor->scrollToAdditionalCursor(_additionalCursorIndex);
}

void ScenarioTextEditWidget::aboutShowSearch()
{
    const bool visible = m_search->isChecked();
    if (m_searchLine->isVisible() != visible) {
        const bool FIX = true;
        WAF::Animation::slide(m_searchLine, WAF::FromBottomToTop, FIX, !FIX, visible);
        QTimer::singleShot(300, [=] { m_searchLine->setVisible(visible); });
    }

    if (visible) {
        m_searchLine->selectText();
        m_searchLine->setFocus();
    } else {
        m_editorWrapper->setFocus();
    }
}

void ScenarioTextEditWidget::aboutShowFastFormat()
{
    m_fastFormatWidget->setVisible(m_fastFormat->isChecked());
    if (m_fastFormatWidget->isVisible()) {
        m_fastFormatWidget->setFocus();
        m_fastFormatWidget->selectCurrentBlock();
    }
}

void ScenarioTextEditWidget::updateTextMode(bool _outlineMode)
{
    m_editor->setOutlineMode(_outlineMode);

    initStylesCombo();

    //
    // Включаем/выключаем доступ к панели быстрого форматирования
    //
    if (_outlineMode) {
        m_fastFormat->hide();
        m_fastFormatWidget->hide();
    } else {
        m_fastFormat->show();
        if (m_fastFormat->isChecked()) {
            m_fastFormatWidget->show();
        }
    }

    emit textModeChanged();
}

void ScenarioTextEditWidget::aboutUpdateTextStyle()
{
    ScenarioBlockStyle::Type currentType = m_editor->scenarioBlockType();
    if (currentType == ScenarioBlockStyle::TitleHeader) {
        currentType = ScenarioBlockStyle::Title;
    } else if (currentType == ScenarioBlockStyle::FolderFooter) {
        currentType = ScenarioBlockStyle::FolderHeader;
    }

    for (int itemIndex = 0; itemIndex < m_textStyles->count(); ++itemIndex) {
        ScenarioBlockStyle::Type itemType =
                (ScenarioBlockStyle::Type)m_textStyles->itemData(itemIndex).toInt();
        if (itemType == currentType) {
            m_textStyles->setCurrentIndex(itemIndex);
            break;
        }
    }
}

void ScenarioTextEditWidget::aboutChangeTextStyle()
{
    ScenarioBlockStyle::Type type =
            (ScenarioBlockStyle::Type)m_textStyles->itemData(m_textStyles->currentIndex()).toInt();

    //
    // Меняем стиль блока, если это возможно
    //
    m_editor->changeScenarioBlockType(type);
    m_editorWrapper->setFocus();
}

void ScenarioTextEditWidget::aboutCursorPositionChanged()
{
    emit cursorPositionChanged(m_editor->textCursor().position());
}

void ScenarioTextEditWidget::aboutTextChanged()
{
    if (!m_editor->document()->isEmpty()) {
        emit textChanged();
    }
}

void ScenarioTextEditWidget::aboutStyleChanged()
{
    emit textChanged();
}

void ScenarioTextEditWidget::initView()
{
    m_outline->setObjectName("scenarioOutlineMode");
    m_outline->setIcons(QIcon(":/Graphics/Icons/Editing/outline.png"));
    m_outline->setToolTip(tr("Outline mode"));
    m_outline->setCheckable(true);

    m_textStyles->setToolTip(tr("Current Text Block Style"));
    m_textStyles->setSizePolicy(m_textStyles->sizePolicy().horizontalPolicy(), QSizePolicy::Preferred);

    initStylesCombo();

    m_undo->setIcons(QIcon(":/Graphics/Icons/Editing/undo.png"));
    m_undo->setToolTip(ShortcutHelper::makeToolTip(tr("Undo last action"), "Ctrl+Z"));

    m_redo->setIcons(QIcon(":/Graphics/Icons/Editing/redo.png"));
    m_redo->setToolTip(ShortcutHelper::makeToolTip(tr("Redo last action"), "Shift+Ctrl+Z"));

    m_search->setObjectName("scenarioSearch");
    m_search->setIcons(QIcon(":/Graphics/Icons/Editing/search.png"));
    m_search->setToolTip(ShortcutHelper::makeToolTip(tr("Search and Replace"), "Ctrl+F"));
    m_search->setCheckable(true);

    m_fastFormat->setObjectName("scenarioFastFormat");
    m_fastFormat->setIcons(QIcon(":/Graphics/Icons/Editing/format.png"));
    m_fastFormat->setToolTip(tr("Text Fast Format"));
    m_fastFormat->setCheckable(true);

    m_review->setObjectName("scenarioReview");

    m_duration->setToolTip(tr("Duration from Start to Cursor Position | Full Duration"));
    m_duration->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_duration->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_editor->setObjectName("scenarioEditor");
    m_editor->setPageFormat(ScenarioTemplateFacade::getTemplate().pageSizeId());

    m_searchLine->setEditor(m_editor);
    m_searchLine->hide();

    m_fastFormatWidget->setEditor(m_editor);
    m_fastFormatWidget->hide();

    m_reviewView->setObjectName("reviewView");
    m_reviewView->setEditor(m_editor);
    m_reviewView->hide();

    QHBoxLayout* topLayout = new QHBoxLayout(m_toolbar);
    topLayout->setContentsMargins(QMargins());
    topLayout->setSpacing(0);
    topLayout->addWidget(m_outline);
    topLayout->addWidget(m_textStyles);
    topLayout->addWidget(m_undo);
    topLayout->addWidget(m_redo);
    topLayout->addWidget(m_search);
    topLayout->addWidget(m_fastFormat);
    topLayout->addWidget(m_review);
    topLayout->addWidget(m_duration);
    topLayout->addWidget(m_countersInfo);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(QMargins());
    mainLayout->addWidget(m_toolbar);
    mainLayout->addWidget(m_editorWrapper);
    mainLayout->addWidget(m_searchLine);

    QSplitter* mainSplitter = new QSplitter(this);
    mainSplitter->setObjectName("mainScenarioSplitter");
    mainSplitter->setHandleWidth(1);
    mainSplitter->setOpaqueResize(false);
    QWidget* mainLayoutWidget = new QWidget(this);
    mainLayoutWidget->setObjectName("mainLayoutWidget");
    mainLayoutWidget->setLayout(mainLayout);
    mainSplitter->addWidget(mainLayoutWidget);
    mainSplitter->addWidget(m_reviewView);
    mainSplitter->setSizes(QList<int>() << 3 << 1);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    layout->addWidget(mainSplitter);
    layout->addWidget(m_fastFormatWidget);

    setLayout(layout);
}

void ScenarioTextEditWidget::initStylesCombo()
{
    m_textStyles->clear();

    ScenarioTemplate usedTemplate = ScenarioTemplateFacade::getTemplate();
    const bool BEAUTIFY_NAME = true;

    static QList<ScenarioBlockStyle::Type> s_types =
        QList<ScenarioBlockStyle::Type>()
            << ScenarioBlockStyle::SceneHeading
            << ScenarioBlockStyle::SceneCharacters
            << ScenarioBlockStyle::Action
            << ScenarioBlockStyle::Character
            << ScenarioBlockStyle::Dialogue
            << ScenarioBlockStyle::Parenthetical
            << ScenarioBlockStyle::Title
            << ScenarioBlockStyle::Note
            << ScenarioBlockStyle::Transition
            << ScenarioBlockStyle::NoprintableText
            << ScenarioBlockStyle::FolderHeader
            << ScenarioBlockStyle::SceneDescription;

    foreach (ScenarioBlockStyle::Type type, s_types) {
        if (usedTemplate.blockStyle(type).isActive()
            && m_editor->visibleBlocksTypes().contains(type)) {
            m_textStyles->addItem(ScenarioBlockStyle::typeName(type, BEAUTIFY_NAME), type);
        }
    }

    updateStylesCombo();
}

void ScenarioTextEditWidget::updateStylesCombo()
{
    for (int index = 0; index < m_textStyles->count(); ++index) {
        ScenarioBlockStyle::Type blockType =
                (ScenarioBlockStyle::Type)m_textStyles->itemData(index).toInt();
        m_textStyles->setItemData(index, m_editor->shortcut(blockType), Qt::ToolTipRole);
    }
}

void ScenarioTextEditWidget::initConnections()
{
    QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(shortcut, &QShortcut::activated, [=] {
        //
        // Если поиск виден, но в нём нет фокуса - установим фокус в него
        // В остальных случаях просто покажем, или скроем поиск
        //
        if (m_search->isChecked()
            && m_searchLine->isVisible()
            && !m_searchLine->hasFocus()) {
            m_searchLine->selectText();
            m_searchLine->setFocus();
        }
        //
        // В противном случае сменим видимость
        //
        else {
            m_search->toggle();
        }
    });

    connect(m_textStyles, SIGNAL(activated(int)), this, SLOT(aboutChangeTextStyle()));
    connect(m_undo, &FlatButton::clicked, this, &ScenarioTextEditWidget::undoRequest);
    connect(m_redo, &FlatButton::clicked, this, &ScenarioTextEditWidget::redoRequest);
    connect(m_search, &FlatButton::toggled, this, &ScenarioTextEditWidget::aboutShowSearch);
    connect(m_fastFormat, SIGNAL(toggled(bool)), this, SLOT(aboutShowFastFormat()));
    connect(m_fastFormatWidget, &UserInterface::ScenarioFastFormatWidget::focusMovedToEditor,
            [=] { m_editorWrapper->setFocus(); });
    connect(m_review, SIGNAL(toggled(bool)), m_reviewView, SLOT(setVisible(bool)));
    connect(m_reviewView, &ScenarioReviewView::undoRequest, this, &ScenarioTextEditWidget::undoRequest);
    connect(m_reviewView, &ScenarioReviewView::redoRequest, this, &ScenarioTextEditWidget::redoRequest);

    initEditorConnections();
}

void ScenarioTextEditWidget::initEditorConnections()
{
    connect(m_outline, &FlatButton::toggled, this, &ScenarioTextEditWidget::updateTextMode);
    connect(m_editor, &ScenarioTextEdit::undoRequest, this, &ScenarioTextEditWidget::undoRequest);
    connect(m_editor, &ScenarioTextEdit::redoRequest, this, &ScenarioTextEditWidget::redoRequest);
    connect(m_editor, SIGNAL(currentStyleChanged()), this, SLOT(aboutUpdateTextStyle()));
    connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutUpdateTextStyle()));
    connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutCursorPositionChanged()));
    connect(m_editor, SIGNAL(textChanged()), this, SLOT(aboutTextChanged()));
    connect(m_editor, SIGNAL(styleChanged()), this, SLOT(aboutStyleChanged()));
    connect(m_editor, SIGNAL(reviewChanged()), this, SIGNAL(textChanged()));
    connect(m_editorWrapper, SIGNAL(zoomRangeChanged(qreal)), this, SIGNAL(zoomRangeChanged(qreal)));

    updateTextMode(m_outline->isChecked());
}

void ScenarioTextEditWidget::removeEditorConnections()
{
    disconnect(m_outline, &FlatButton::toggled, this, &ScenarioTextEditWidget::updateTextMode);
    disconnect(m_editor, &ScenarioTextEdit::undoRequest, this, &ScenarioTextEditWidget::undoRequest);
    disconnect(m_editor, &ScenarioTextEdit::redoRequest, this, &ScenarioTextEditWidget::redoRequest);
    disconnect(m_editor, SIGNAL(currentStyleChanged()), this, SLOT(aboutUpdateTextStyle()));
    disconnect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutUpdateTextStyle()));
    disconnect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutCursorPositionChanged()));
    disconnect(m_editor, SIGNAL(textChanged()), this, SLOT(aboutTextChanged()));
    disconnect(m_editor, SIGNAL(styleChanged()), this, SLOT(aboutStyleChanged()));
    disconnect(m_editor, SIGNAL(reviewChanged()), this, SIGNAL(textChanged()));
    disconnect(m_editorWrapper, SIGNAL(zoomRangeChanged(qreal)), this, SIGNAL(zoomRangeChanged(qreal)));
}

void ScenarioTextEditWidget::initStyleSheet()
{
    m_outline->setProperty("inTopPanel", true);

    m_textStyles->setProperty("inTopPanel", true);
    m_textStyles->setProperty("topPanelTopBordered", true);
    m_textStyles->setProperty("topPanelLeftBordered", true);
    m_textStyles->setProperty("topPanelRightBordered", true);

    m_undo->setProperty("inTopPanel", true);
    m_redo->setProperty("inTopPanel", true);
    m_search->setProperty("inTopPanel", true);
    m_fastFormat->setProperty("inTopPanel", true);

    m_duration->setProperty("inTopPanel", true);
    m_duration->setProperty("topPanelTopBordered", true);

    m_countersInfo->setProperty("inTopPanel", true);
    m_countersInfo->setProperty("topPanelTopBordered", true);
    m_countersInfo->setProperty("topPanelRightBordered", true);

    m_editorWrapper->setProperty("mainContainer", true);
}
