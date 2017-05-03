/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "PageTextEdit_p.h"
#include "qlineedit.h"
#include "qtextbrowser.h"

#ifndef QT_NO_TEXTEDIT
#include <qfont.h>
#include <qpainter.h>
#include <qevent.h>
#include <qdebug.h>
#include <qdrag.h>
#include <qclipboard.h>
#include <qmenu.h>
#include <qstyle.h>
#include <qtimer.h>
#ifndef QT_NO_ACCESSIBILITY
#include <qaccessible.h>
#endif
#include "private/qtextdocumentlayout_p.h"
#include "qtextdocument.h"
#include "private/qtextdocument_p.h"
#include "qtextlist.h"
#include "private/qwidgettextcontrol_p.h"

#include <qtextformat.h>
#include <qdatetime.h>
#include <qapplication.h>
#include <limits.h>
#include <math.h>
#include <qtexttable.h>
#include <qvariant.h>
#include <qscopedpointer.h>
#include <qpropertyanimation.h>

#endif

QT_BEGIN_NAMESPACE


#ifndef QT_NO_TEXTEDIT
static inline bool shouldEnableInputMethod(PageTextEdit *textedit)
{
    return !textedit->isReadOnly();
}

class PageTextEditControl : public QWidgetTextControl
{
public:
    inline PageTextEditControl(QObject *parent) : QWidgetTextControl(parent) {}

    virtual QMimeData *createMimeDataFromSelection() const Q_DECL_OVERRIDE {
        PageTextEdit *ed = qobject_cast<PageTextEdit *>(parent());
        if (!ed)
            return QWidgetTextControl::createMimeDataFromSelection();
        return ed->createMimeDataFromSelection();
    }
    virtual bool canInsertFromMimeData(const QMimeData *source) const Q_DECL_OVERRIDE {
        PageTextEdit *ed = qobject_cast<PageTextEdit *>(parent());
        if (!ed)
            return QWidgetTextControl::canInsertFromMimeData(source);
        return ed->canInsertFromMimeData(source);
    }
    virtual void insertFromMimeData(const QMimeData *source) Q_DECL_OVERRIDE {
        PageTextEdit *ed = qobject_cast<PageTextEdit *>(parent());
        if (!ed)
            QWidgetTextControl::insertFromMimeData(source);
        else
            ed->insertFromMimeData(source);
    }
};

PageTextEditPrivate::PageTextEditPrivate()
    : control(0),
      autoFormatting(PageTextEdit::AutoNone), tabChangesFocus(false),
      lineWrap(PageTextEdit::WidgetWidth), lineWrapColumnOrWidth(0),
      wordWrap(QTextOption::WrapAtWordBoundaryOrAnywhere), clickCausedFocus(0),
      textFormat(Qt::AutoText), m_usePageMode(false), m_addBottomSpace(true),
      m_showPageNumbers(true), m_pageNumbersAlignment(Qt::AlignTop | Qt::AlignRight)
{
    ignoreAutomaticScrollbarAdjustment = false;
    preferRichText = false;
    showCursorOnInitialShow = true;
    inDrag = false;
}

void PageTextEditPrivate::createAutoBulletList()
{
    QTextCursor cursor = control->textCursor();
    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();

    QTextListFormat listFmt;
    listFmt.setStyle(QTextListFormat::ListDisc);
    listFmt.setIndent(blockFmt.indent() + 1);

    blockFmt.setIndent(0);
    cursor.setBlockFormat(blockFmt);

    cursor.createList(listFmt);

    cursor.endEditBlock();
    control->setTextCursor(cursor);
}

void PageTextEditPrivate::init(const QString &html)
{
    Q_Q(PageTextEdit);
    control = new PageTextEditControl(q);
    control->setPalette(q->palette());

    QObject::connect(control, SIGNAL(microFocusChanged()), q, SLOT(updateMicroFocus()));
    QObject::connect(control, SIGNAL(documentSizeChanged(QSizeF)), q, SLOT(_q_adjustScrollbars()));
    QObject::connect(control, SIGNAL(updateRequest(QRectF)), q, SLOT(_q_repaintContents(QRectF)));
    QObject::connect(control, SIGNAL(visibilityRequest(QRectF)), q, SLOT(_q_ensureVisible(QRectF)));
    QObject::connect(control, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
                     q, SLOT(_q_currentCharFormatChanged(QTextCharFormat)));

    QObject::connect(control, SIGNAL(textChanged()), q, SIGNAL(textChanged()));
    QObject::connect(control, SIGNAL(undoAvailable(bool)), q, SIGNAL(undoAvailable(bool)));
    QObject::connect(control, SIGNAL(redoAvailable(bool)), q, SIGNAL(redoAvailable(bool)));
    QObject::connect(control, SIGNAL(copyAvailable(bool)), q, SIGNAL(copyAvailable(bool)));
    QObject::connect(control, SIGNAL(selectionChanged()), q, SIGNAL(selectionChanged()));
    QObject::connect(control, SIGNAL(cursorPositionChanged()), q, SLOT(_q_cursorPositionChanged()));

    QObject::connect(control, SIGNAL(textChanged()), q, SLOT(updateMicroFocus()));

    QTextDocument *doc = control->document();
    // set a null page size initially to avoid any relayouting until the textedit
    // is shown. relayoutDocument() will take care of setting the page size to the
    // viewport dimensions later.
    doc->setPageSize(QSize(0, 0));
    doc->documentLayout()->setPaintDevice(viewport);
    doc->setDefaultFont(q->font());
    doc->setUndoRedoEnabled(false); // flush undo buffer.
    doc->setUndoRedoEnabled(true);

    if (!html.isEmpty())
        control->setHtml(html);

    hbar->setSingleStep(20);
    vbar->setSingleStep(20);

    viewport->setBackgroundRole(QPalette::Base);
    q->setAcceptDrops(true);
    q->setFocusPolicy(Qt::WheelFocus);
    q->setAttribute(Qt::WA_KeyCompression);
    q->setAttribute(Qt::WA_InputMethodEnabled);
    q->setInputMethodHints(Qt::ImhMultiLine);
#ifndef QT_NO_CURSOR
    viewport->setCursor(Qt::IBeamCursor);
#endif
#ifdef Q_DEAD_CODE_FROM_QT4_WIN
    setSingleFingerPanEnabled(true);
#endif
}

void PageTextEditPrivate::_q_repaintContents(const QRectF &contentsRect)
{
    if (!contentsRect.isValid()) {
        viewport->update();
        return;
    }
    const int xOffset = horizontalOffset();
    const int yOffset = verticalOffset();
    const QRectF visibleRect(xOffset, yOffset, viewport->width(), viewport->height());

    QRect r = contentsRect.intersected(visibleRect).toAlignedRect();
    if (r.isEmpty())
        return;

    r.translate(-xOffset, -yOffset);
    viewport->update(r);
}

void PageTextEditPrivate::_q_cursorPositionChanged()
{
    Q_Q(PageTextEdit);
    emit q->cursorPositionChanged();
#ifndef QT_NO_ACCESSIBILITY
    QAccessibleTextCursorEvent event(q, q->textCursor().position());
    QAccessible::updateAccessibility(&event);
#endif
}

void PageTextEditPrivate::pageUpDown(QTextCursor::MoveOperation op, QTextCursor::MoveMode moveMode)
{
    QTextCursor cursor = control->textCursor();
    bool moved = false;
    qreal lastY = control->cursorRect(cursor).top();
    qreal distance = 0;
    // move using movePosition to keep the cursor's x
    do {
        qreal y = control->cursorRect(cursor).top();
        distance += qAbs(y - lastY);
        lastY = y;
        moved = cursor.movePosition(op, moveMode);
    } while (moved && distance < viewport->height());

    if (moved) {
        if (op == QTextCursor::Up) {
            cursor.movePosition(QTextCursor::Down, moveMode);
            vbar->triggerAction(QAbstractSlider::SliderPageStepSub);
        } else {
            cursor.movePosition(QTextCursor::Up, moveMode);
            vbar->triggerAction(QAbstractSlider::SliderPageStepAdd);
        }
    }
    control->setTextCursor(cursor);
}

#ifndef QT_NO_SCROLLBAR
static QSize documentSize(QWidgetTextControl *control)
{
    QTextDocument *doc = control->document();
    QAbstractTextDocumentLayout *layout = doc->documentLayout();

    QSize docSize;

    if (QTextDocumentLayout *tlayout = qobject_cast<QTextDocumentLayout *>(layout)) {
        docSize = tlayout->dynamicDocumentSize().toSize();
        int percentageDone = tlayout->layoutStatus();
        // extrapolate height
        if (percentageDone > 0)
            docSize.setHeight(docSize.height() * 100 / percentageDone);
    } else {
        docSize = layout->documentSize().toSize();
    }

    return docSize;
}

void PageTextEditPrivate::_q_adjustScrollbars()
{
    if (ignoreAutomaticScrollbarAdjustment)
        return;
    ignoreAutomaticScrollbarAdjustment = true; // avoid recursion, #106108

    QSize viewportSize = viewport->size();
    QSize docSize = documentSize(control);

    // due to the recursion guard we have to repeat this step a few times,
    // as adding/removing a scroll bar will cause the document or viewport
    // size to change
    // ideally we should loop until the viewport size and doc size stabilize,
    // but in corner cases they might fluctuate, so we need to limit the
    // number of iterations
    for (int i = 0; i < 4; ++i) {
        hbar->setRange(0, docSize.width() - viewportSize.width());
        hbar->setPageStep(viewportSize.width());

        //
        // В постраничном режиме показываем страницу целиком
        //
        if (m_usePageMode) {
            const int pageHeight = m_pageMetrics.pxPageSize().height();
            const int documentHeight = pageHeight * control->document()->pageCount();
            const int maximumValue = documentHeight - viewport->height();
            vbar->setRange(0, maximumValue);
        }
        //
        // В обычном режиме просто добавляем немного дополнительной прокрутки для удобства
        //
        else {
            const int SCROLL_DELTA = 800;
            int maximumValue =
                    docSize.height() - viewportSize.height()
                    + (m_addBottomSpace ? SCROLL_DELTA : 0);
            vbar->setRange(0, maximumValue);
        }
        vbar->setPageStep(viewportSize.height());

        // if we are in left-to-right mode widening the document due to
        // lazy layouting does not require a repaint. If in right-to-left
        // the scroll bar has the value zero and it visually has the maximum
        // value (it is visually at the right), then widening the document
        // keeps it at value zero but visually adjusts it to the new maximum
        // on the right, hence we need an update.
        if (q_func()->isRightToLeft())
            viewport->update();

        _q_showOrHideScrollBars();

        const QSize oldViewportSize = viewportSize;
        const QSize oldDocSize = docSize;

        // make sure the document is layouted if the viewport width changes
        viewportSize = viewport->size();
        if (viewportSize.width() != oldViewportSize.width() && !m_usePageMode)
            relayoutDocument();

        docSize = documentSize(control);
        if (viewportSize == oldViewportSize && docSize == oldDocSize)
            break;
    }
    ignoreAutomaticScrollbarAdjustment = false;
}
#endif

// rect is in content coordinates
void PageTextEditPrivate::_q_ensureVisible(const QRectF &_rect)
{
    const QRect rect = _rect.toRect();
    if ((vbar->isVisible() && vbar->maximum() < rect.bottom())
        || (hbar->isVisible() && hbar->maximum() < rect.right()))
        _q_adjustScrollbars();
    const int visibleWidth = viewport->width();
    const int visibleHeight = viewport->height();
    const bool rtl = q_func()->isRightToLeft();

    if (rect.x() < horizontalOffset()) {
        if (rtl)
            hbar->setValue(hbar->maximum() - rect.x());
        else
            hbar->setValue(rect.x());
    } else if (rect.x() + rect.width() > horizontalOffset() + visibleWidth) {
        if (rtl)
            hbar->setValue(hbar->maximum() - (rect.x() + rect.width() - visibleWidth));
        else
            hbar->setValue(rect.x() + rect.width() - visibleWidth);
    }

    if (rect.y() < verticalOffset())
        vbar->setValue(rect.y());
    else if (rect.y() + rect.height() > verticalOffset() + visibleHeight)
        vbar->setValue(rect.y() + rect.height() - visibleHeight);
}

/*!
    \class PageTextEdit
    \brief The PageTextEdit class provides a widget that is used to edit and display
    both plain and rich text.

    \ingroup richtext-processing
    \inmodule QtWidgets

    \tableofcontents

    \section1 Introduction and Concepts

    PageTextEdit is an advanced WYSIWYG viewer/editor supporting rich
    text formatting using HTML-style tags. It is optimized to handle
    large documents and to respond quickly to user input.

    PageTextEdit works on paragraphs and characters. A paragraph is a
    formatted string which is word-wrapped to fit into the width of
    the widget. By default when reading plain text, one newline
    signifies a paragraph. A document consists of zero or more
    paragraphs. The words in the paragraph are aligned in accordance
    with the paragraph's alignment. Paragraphs are separated by hard
    line breaks. Each character within a paragraph has its own
    attributes, for example, font and color.

    PageTextEdit can display images, lists and tables. If the text is
    too large to view within the text edit's viewport, scroll bars will
    appear. The text edit can load both plain text and rich text files.
    Rich text is described using a subset of HTML 4 markup, refer to the
    \l {Supported HTML Subset} page for more information.

    If you just need to display a small piece of rich text use QLabel.

    The rich text support in Qt is designed to provide a fast, portable and
    efficient way to add reasonable online help facilities to
    applications, and to provide a basis for rich text editors. If
    you find the HTML support insufficient for your needs you may consider
    the use of Qt WebKit, which provides a full-featured web browser
    widget.

    The shape of the mouse cursor on a PageTextEdit is Qt::IBeamCursor by default.
    It can be changed through the viewport()'s cursor property.

    \section1 Using PageTextEdit as a Display Widget

    PageTextEdit can display a large HTML subset, including tables and
    images.

    The text is set or replaced using setHtml() which deletes any
    existing text and replaces it with the text passed in the
    setHtml() call. If you call setHtml() with legacy HTML, and then
    call toHtml(), the text that is returned may have different markup,
    but will render the same. The entire text can be deleted with clear().

    Text itself can be inserted using the QTextCursor class or using the
    convenience functions insertHtml(), insertPlainText(), append() or
    paste(). QTextCursor is also able to insert complex objects like tables
    or lists into the document, and it deals with creating selections
    and applying changes to selected text.

    By default the text edit wraps words at whitespace to fit within
    the text edit widget. The setLineWrapMode() function is used to
    specify the kind of line wrap you want, or \l NoWrap if you don't
    want any wrapping. Call setLineWrapMode() to set a fixed pixel width
    \l FixedPixelWidth, or character column (e.g. 80 column) \l
    FixedColumnWidth with the pixels or columns specified with
    setLineWrapColumnOrWidth(). If you use word wrap to the widget's width
    \l WidgetWidth, you can specify whether to break on whitespace or
    anywhere with setWordWrapMode().

    The find() function can be used to find and select a given string
    within the text.

    If you want to limit the total number of paragraphs in a PageTextEdit,
    as for example it is often useful in a log viewer, then you can use
    QTextDocument's maximumBlockCount property for that.

    \section2 Read-only Key Bindings

    When PageTextEdit is used read-only the key bindings are limited to
    navigation, and text may only be selected with the mouse:
    \table
    \header \li Keypresses \li Action
    \row \li Up        \li Moves one line up.
    \row \li Down        \li Moves one line down.
    \row \li Left        \li Moves one character to the left.
    \row \li Right        \li Moves one character to the right.
    \row \li PageUp        \li Moves one (viewport) page up.
    \row \li PageDown        \li Moves one (viewport) page down.
    \row \li Home        \li Moves to the beginning of the text.
    \row \li End                \li Moves to the end of the text.
    \row \li Alt+Wheel
         \li Scrolls the page horizontally (the Wheel is the mouse wheel).
    \row \li Ctrl+Wheel        \li Zooms the text.
    \row \li Ctrl+A            \li Selects all text.
    \endtable

    The text edit may be able to provide some meta-information. For
    example, the documentTitle() function will return the text from
    within HTML \c{<title>} tags.

    \note Zooming into HTML documents only works if the font-size is not set to a fixed size.

    \section1 Using PageTextEdit as an Editor

    All the information about using PageTextEdit as a display widget also
    applies here.

    The current char format's attributes are set with setFontItalic(),
    setFontWeight(), setFontUnderline(), setFontFamily(),
    setFontPointSize(), setTextColor() and setCurrentFont(). The current
    paragraph's alignment is set with setAlignment().

    Selection of text is handled by the QTextCursor class, which provides
    functionality for creating selections, retrieving the text contents or
    deleting selections. You can retrieve the object that corresponds with
    the user-visible cursor using the textCursor() method. If you want to set
    a selection in PageTextEdit just create one on a QTextCursor object and
    then make that cursor the visible cursor using setTextCursor(). The selection
    can be copied to the clipboard with copy(), or cut to the clipboard with
    cut(). The entire text can be selected using selectAll().

    When the cursor is moved and the underlying formatting attributes change,
    the currentCharFormatChanged() signal is emitted to reflect the new attributes
    at the new cursor position.

    The textChanged() signal is emitted whenever the text changes (as a result
    of setText() or through the editor itself).

    PageTextEdit holds a QTextDocument object which can be retrieved using the
    document() method. You can also set your own document object using setDocument().

    QTextDocument provides an \l {QTextDocument::isModified()}{isModified()}
    function which will return true if the text has been modified since it was
    either loaded or since the last call to setModified with false as argument.
    In addition it provides methods for undo and redo.

    \section2 Drag and Drop

    PageTextEdit also supports custom drag and drop behavior. By default,
    PageTextEdit will insert plain text, HTML and rich text when the user drops
    data of these MIME types onto a document. Reimplement
    canInsertFromMimeData() and insertFromMimeData() to add support for
    additional MIME types.

    For example, to allow the user to drag and drop an image onto a PageTextEdit,
    you could the implement these functions in the following way:

    \snippet textdocument-imagedrop/textedit.cpp 0

    We add support for image MIME types by returning true. For all other
    MIME types, we use the default implementation.

    \snippet textdocument-imagedrop/textedit.cpp 1

    We unpack the image from the QVariant held by the MIME source and insert
    it into the document as a resource.

    \section2 Editing Key Bindings

    The list of key bindings which are implemented for editing:
    \table
    \header \li Keypresses \li Action
    \row \li Backspace \li Deletes the character to the left of the cursor.
    \row \li Delete \li Deletes the character to the right of the cursor.
    \row \li Ctrl+C \li Copy the selected text to the clipboard.
    \row \li Ctrl+Insert \li Copy the selected text to the clipboard.
    \row \li Ctrl+K \li Deletes to the end of the line.
    \row \li Ctrl+V \li Pastes the clipboard text into text edit.
    \row \li Shift+Insert \li Pastes the clipboard text into text edit.
    \row \li Ctrl+X \li Deletes the selected text and copies it to the clipboard.
    \row \li Shift+Delete \li Deletes the selected text and copies it to the clipboard.
    \row \li Ctrl+Z \li Undoes the last operation.
    \row \li Ctrl+Y \li Redoes the last operation.
    \row \li Left \li Moves the cursor one character to the left.
    \row \li Ctrl+Left \li Moves the cursor one word to the left.
    \row \li Right \li Moves the cursor one character to the right.
    \row \li Ctrl+Right \li Moves the cursor one word to the right.
    \row \li Up \li Moves the cursor one line up.
    \row \li Down \li Moves the cursor one line down.
    \row \li PageUp \li Moves the cursor one page up.
    \row \li PageDown \li Moves the cursor one page down.
    \row \li Home \li Moves the cursor to the beginning of the line.
    \row \li Ctrl+Home \li Moves the cursor to the beginning of the text.
    \row \li End \li Moves the cursor to the end of the line.
    \row \li Ctrl+End \li Moves the cursor to the end of the text.
    \row \li Alt+Wheel \li Scrolls the page horizontally (the Wheel is the mouse wheel).
    \endtable

    To select (mark) text hold down the Shift key whilst pressing one
    of the movement keystrokes, for example, \e{Shift+Right}
    will select the character to the right, and \e{Shift+Ctrl+Right} will select the word to the right, etc.

    \sa QTextDocument, QTextCursor, {Application Example},
        {Syntax Highlighter Example}, {Rich Text Processing}
*/

/*!
    \property PageTextEdit::plainText
    \since 4.3

    This property gets and sets the text editor's contents as plain
    text. Previous contents are removed and undo/redo history is reset
    when the property is set.

    If the text edit has another content type, it will not be replaced
    by plain text if you call toPlainText(). The only exception to this
    is the non-break space, \e{nbsp;}, that will be converted into
    standard space.

    By default, for an editor with no contents, this property contains
    an empty string.

    \sa html
*/

/*!
    \property PageTextEdit::undoRedoEnabled
    \brief whether undo and redo are enabled

    Users are only able to undo or redo actions if this property is
    true, and if there is an action that can be undone (or redone).
*/

/*!
    \enum PageTextEdit::LineWrapMode

    \value NoWrap
    \value WidgetWidth
    \value FixedPixelWidth
    \value FixedColumnWidth
*/

/*!
    \enum PageTextEdit::AutoFormattingFlag

    \value AutoNone Don't do any automatic formatting.
    \value AutoBulletList Automatically create bullet lists (e.g. when
    the user enters an asterisk ('*') in the left most column, or
    presses Enter in an existing list item.
    \value AutoAll Apply all automatic formatting. Currently only
    automatic bullet lists are supported.
*/


/*!
    Constructs an empty PageTextEdit with parent \a
    parent.
*/
PageTextEdit::PageTextEdit(QWidget *parent)
    : QAbstractScrollArea(*new PageTextEditPrivate, parent)
{
    Q_D(PageTextEdit);
    d->init();
}

/*!
    \internal
*/
PageTextEdit::PageTextEdit(PageTextEditPrivate &dd, QWidget *parent)
    : QAbstractScrollArea(dd, parent)
{
    Q_D(PageTextEdit);
    d->init();
}

/*!
    Constructs a PageTextEdit with parent \a parent. The text edit will display
    the text \a text. The text is interpreted as html.
*/
PageTextEdit::PageTextEdit(const QString &text, QWidget *parent)
    : QAbstractScrollArea(*new PageTextEditPrivate, parent)
{
    Q_D(PageTextEdit);
    d->init(text);
}



/*!
    Destructor.
*/
PageTextEdit::~PageTextEdit()
{
}

/*!
    Returns the point size of the font of the current format.

    \sa setFontFamily(), setCurrentFont(), setFontPointSize()
*/
qreal PageTextEdit::fontPointSize() const
{
    Q_D(const PageTextEdit);
    return d->control->textCursor().charFormat().fontPointSize();
}

/*!
    Returns the font family of the current format.

    \sa setFontFamily(), setCurrentFont(), setFontPointSize()
*/
QString PageTextEdit::fontFamily() const
{
    Q_D(const PageTextEdit);
    return d->control->textCursor().charFormat().fontFamily();
}

/*!
    Returns the font weight of the current format.

    \sa setFontWeight(), setCurrentFont(), setFontPointSize(), QFont::Weight
*/
int PageTextEdit::fontWeight() const
{
    Q_D(const PageTextEdit);
    return d->control->textCursor().charFormat().fontWeight();
}

/*!
    Returns \c true if the font of the current format is underlined; otherwise returns
    false.

    \sa setFontUnderline()
*/
bool PageTextEdit::fontUnderline() const
{
    Q_D(const PageTextEdit);
    return d->control->textCursor().charFormat().fontUnderline();
}

/*!
    Returns \c true if the font of the current format is italic; otherwise returns
    false.

    \sa setFontItalic()
*/
bool PageTextEdit::fontItalic() const
{
    Q_D(const PageTextEdit);
    return d->control->textCursor().charFormat().fontItalic();
}

/*!
    Returns the text color of the current format.

    \sa setTextColor()
*/
QColor PageTextEdit::textColor() const
{
    Q_D(const PageTextEdit);
    return d->control->textCursor().charFormat().foreground().color();
}

/*!
    \since 4.4

    Returns the text background color of the current format.

    \sa setTextBackgroundColor()
*/
QColor PageTextEdit::textBackgroundColor() const
{
    Q_D(const PageTextEdit);
    return d->control->textCursor().charFormat().background().color();
}

/*!
    Returns the font of the current format.

    \sa setCurrentFont(), setFontFamily(), setFontPointSize()
*/
QFont PageTextEdit::currentFont() const
{
    Q_D(const PageTextEdit);
    return d->control->textCursor().charFormat().font();
}

/*!
    Sets the alignment of the current paragraph to \a a. Valid
    alignments are Qt::AlignLeft, Qt::AlignRight,
    Qt::AlignJustify and Qt::AlignCenter (which centers
    horizontally).
*/
void PageTextEdit::setAlignment(Qt::Alignment a)
{
    Q_D(PageTextEdit);
    QTextBlockFormat fmt;
    fmt.setAlignment(a);
    QTextCursor cursor = d->control->textCursor();
    cursor.mergeBlockFormat(fmt);
    d->control->setTextCursor(cursor);
}

/*!
    Returns the alignment of the current paragraph.

    \sa setAlignment()
*/
Qt::Alignment PageTextEdit::alignment() const
{
    Q_D(const PageTextEdit);
    return d->control->textCursor().blockFormat().alignment();
}

/*!
    \property PageTextEdit::document
    \brief the underlying document of the text editor.

    \note The editor \e{does not take ownership of the document} unless it
    is the document's parent object. The parent object of the provided document
    remains the owner of the object. If the previously assigned document is a
    child of the editor then it will be deleted.
*/
void PageTextEdit::setDocument(QTextDocument *document)
{
    Q_D(PageTextEdit);
    d->control->setDocument(document);
    d->updateDefaultTextOption();
    d->updateDocumentGeometry();
    d->relayoutDocument();
}

QTextDocument *PageTextEdit::document() const
{
    Q_D(const PageTextEdit);
    return d->control->document();
}

/*!
    \since 5.2

    \property PageTextEdit::placeholderText
    \brief the editor placeholder text

    Setting this property makes the editor display a grayed-out
    placeholder text as long as the document() is empty.

    By default, this property contains an empty string.

    \sa document()
*/
QString PageTextEdit::placeholderText() const
{
    Q_D(const PageTextEdit);
    return d->placeholderText;
}

void PageTextEdit::setPlaceholderText(const QString &placeholderText)
{
    Q_D(PageTextEdit);
    if (d->placeholderText != placeholderText) {
        d->placeholderText = placeholderText;
        if (d->control->document()->isEmpty())
            d->viewport->update();
    }
}

/*!
    Sets the visible \a cursor.
*/
void PageTextEdit::setTextCursor(const QTextCursor &cursor)
{
    doSetTextCursor(cursor);
}

/*!
    \internal

     This provides a hook for subclasses to intercept cursor changes.
*/

void PageTextEdit::doSetTextCursor(const QTextCursor &cursor)
{
    Q_D(PageTextEdit);
    d->control->setTextCursor(cursor);
}

/*!
    Returns a copy of the QTextCursor that represents the currently visible cursor.
    Note that changes on the returned cursor do not affect PageTextEdit's cursor; use
    setTextCursor() to update the visible cursor.
 */
QTextCursor PageTextEdit::textCursor() const
{
    Q_D(const PageTextEdit);
    return d->control->textCursor();
}

/*!
    Sets the font family of the current format to \a fontFamily.

    \sa fontFamily(), setCurrentFont()
*/
void PageTextEdit::setFontFamily(const QString &fontFamily)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(fontFamily);
    mergeCurrentCharFormat(fmt);
}

/*!
    Sets the point size of the current format to \a s.

    Note that if \a s is zero or negative, the behavior of this
    function is not defined.

    \sa fontPointSize(), setCurrentFont(), setFontFamily()
*/
void PageTextEdit::setFontPointSize(qreal s)
{
    QTextCharFormat fmt;
    fmt.setFontPointSize(s);
    mergeCurrentCharFormat(fmt);
}

/*!
    \fn void PageTextEdit::setFontWeight(int weight)

    Sets the font weight of the current format to the given \a weight,
    where the value used is in the range defined by the QFont::Weight
    enum.

    \sa fontWeight(), setCurrentFont(), setFontFamily()
*/
void PageTextEdit::setFontWeight(int w)
{
    QTextCharFormat fmt;
    fmt.setFontWeight(w);
    mergeCurrentCharFormat(fmt);
}

/*!
    If \a underline is true, sets the current format to underline;
    otherwise sets the current format to non-underline.

    \sa fontUnderline()
*/
void PageTextEdit::setFontUnderline(bool underline)
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(underline);
    mergeCurrentCharFormat(fmt);
}

/*!
    If \a italic is true, sets the current format to italic;
    otherwise sets the current format to non-italic.

    \sa fontItalic()
*/
void PageTextEdit::setFontItalic(bool italic)
{
    QTextCharFormat fmt;
    fmt.setFontItalic(italic);
    mergeCurrentCharFormat(fmt);
}

/*!
    Sets the text color of the current format to \a c.

    \sa textColor()
*/
void PageTextEdit::setTextColor(const QColor &c)
{
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(c));
    mergeCurrentCharFormat(fmt);
}

/*!
    \since 4.4

    Sets the text background color of the current format to \a c.

    \sa textBackgroundColor()
*/
void PageTextEdit::setTextBackgroundColor(const QColor &c)
{
    QTextCharFormat fmt;
    fmt.setBackground(QBrush(c));
    mergeCurrentCharFormat(fmt);
}

/*!
    Sets the font of the current format to \a f.

    \sa currentFont(), setFontPointSize(), setFontFamily()
*/
void PageTextEdit::setCurrentFont(const QFont &f)
{
    QTextCharFormat fmt;
    fmt.setFont(f);
    mergeCurrentCharFormat(fmt);
}

/*!
    \since 4.2

    Undoes the last operation.

    If there is no operation to undo, i.e. there is no undo step in
    the undo/redo history, nothing happens.

    \sa redo()
*/
void PageTextEdit::undo()
{
    Q_D(PageTextEdit);
    d->control->undo();
}

void PageTextEdit::redo()
{
    Q_D(PageTextEdit);
    d->control->redo();
}

/*!
    \fn void PageTextEdit::redo()
    \since 4.2

    Redoes the last operation.

    If there is no operation to redo, i.e. there is no redo step in
    the undo/redo history, nothing happens.

    \sa undo()
*/

#ifndef QT_NO_CLIPBOARD
/*!
    Copies the selected text to the clipboard and deletes it from
    the text edit.

    If there is no selected text nothing happens.

    \sa copy(), paste()
*/

void PageTextEdit::cut()
{
    Q_D(PageTextEdit);
    d->control->cut();
}

/*!
    Copies any selected text to the clipboard.

    \sa copyAvailable()
*/

void PageTextEdit::copy()
{
    Q_D(PageTextEdit);
    d->control->copy();
}

/*!
    Pastes the text from the clipboard into the text edit at the
    current cursor position.

    If there is no text in the clipboard nothing happens.

    To change the behavior of this function, i.e. to modify what
    PageTextEdit can paste and how it is being pasted, reimplement the
    virtual canInsertFromMimeData() and insertFromMimeData()
    functions.

    \sa cut(), copy()
*/

void PageTextEdit::paste()
{
    Q_D(PageTextEdit);
    d->control->paste();
}
#endif

/*!
    Deletes all the text in the text edit.

    Note that the undo/redo history is cleared by this function.

    \sa cut(), setPlainText(), setHtml()
*/
void PageTextEdit::clear()
{
    Q_D(PageTextEdit);
    // clears and sets empty content
    d->control->clear();
}


/*!
    Selects all text.

    \sa copy(), cut(), textCursor()
 */
void PageTextEdit::selectAll()
{
    Q_D(PageTextEdit);
    d->control->selectAll();
}

/*! \internal
*/
bool PageTextEdit::event(QEvent *e)
{
    Q_D(PageTextEdit);
#ifndef QT_NO_CONTEXTMENU
    if (e->type() == QEvent::ContextMenu
        && static_cast<QContextMenuEvent *>(e)->reason() == QContextMenuEvent::Keyboard) {
        Q_D(PageTextEdit);
        ensureCursorVisible();
        const QPoint cursorPos = cursorRect().center();
        QContextMenuEvent ce(QContextMenuEvent::Keyboard, cursorPos, d->viewport->mapToGlobal(cursorPos));
        ce.setAccepted(e->isAccepted());
        const bool result = QAbstractScrollArea::event(&ce);
        e->setAccepted(ce.isAccepted());
        return result;
    } else if (e->type() == QEvent::ShortcutOverride
               || e->type() == QEvent::ToolTip) {
        d->sendControlEvent(e);
    }
#endif // QT_NO_CONTEXTMENU
#ifdef QT_KEYPAD_NAVIGATION
    if (e->type() == QEvent::EnterEditFocus || e->type() == QEvent::LeaveEditFocus) {
        if (QApplication::keypadNavigationEnabled())
            d->sendControlEvent(e);
    }
#endif
    return QAbstractScrollArea::event(e);
}

/*! \internal
*/

void PageTextEdit::timerEvent(QTimerEvent *e)
{
    Q_D(PageTextEdit);
    if (e->timerId() == d->autoScrollTimer.timerId()) {
        QRect visible = d->viewport->rect();
        QPoint pos;
        if (d->inDrag) {
            pos = d->autoScrollDragPos;
            visible.adjust(qMin(visible.width()/3,20), qMin(visible.height()/3,20),
                           -qMin(visible.width()/3,20), -qMin(visible.height()/3,20));
        } else {
            const QPoint globalPos = QCursor::pos();
            pos = d->viewport->mapFromGlobal(globalPos);
            QMouseEvent ev(QEvent::MouseMove, pos, mapTo(topLevelWidget(), pos), globalPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            mouseMoveEvent(&ev);
        }
        int deltaY = qMax(pos.y() - visible.top(), visible.bottom() - pos.y()) - visible.height();
        int deltaX = qMax(pos.x() - visible.left(), visible.right() - pos.x()) - visible.width();
        int delta = qMax(deltaX, deltaY);
        if (delta >= 0) {
            if (delta < 7)
                delta = 7;
            int timeout = 4900 / (delta * delta);
            d->autoScrollTimer.start(timeout, this);

            if (deltaY > 0)
                d->vbar->triggerAction(pos.y() < visible.center().y() ?
                                       QAbstractSlider::SliderSingleStepSub
                                       : QAbstractSlider::SliderSingleStepAdd);
            if (deltaX > 0)
                d->hbar->triggerAction(pos.x() < visible.center().x() ?
                                       QAbstractSlider::SliderSingleStepSub
                                       : QAbstractSlider::SliderSingleStepAdd);
        }
    }
#ifdef QT_KEYPAD_NAVIGATION
    else if (e->timerId() == d->deleteAllTimer.timerId()) {
        d->deleteAllTimer.stop();
        clear();
    }
#endif
}

/*!
    Changes the text of the text edit to the string \a text.
    Any previous text is removed.

    \a text is interpreted as plain text.

    Note that the undo/redo history is cleared by this function.

    \sa toPlainText()
*/

void PageTextEdit::setPlainText(const QString &text)
{
    Q_D(PageTextEdit);
    d->control->setPlainText(text);
    d->updateDocumentGeometry();
    d->preferRichText = false;
}

/*!
    QString PageTextEdit::toPlainText() const

    Returns the text of the text edit as plain text.

    \sa PageTextEdit::setPlainText()
 */
QString PageTextEdit::toPlainText() const
{
    Q_D(const PageTextEdit);
    return d->control->toPlainText();
}

/*!
    \property PageTextEdit::html

    This property provides an HTML interface to the text of the text edit.

    toHtml() returns the text of the text edit as html.

    setHtml() changes the text of the text edit.  Any previous text is
    removed and the undo/redo history is cleared. The input text is
    interpreted as rich text in html format.

    \note It is the responsibility of the caller to make sure that the
    text is correctly decoded when a QString containing HTML is created
    and passed to setHtml().

    By default, for a newly-created, empty document, this property contains
    text to describe an HTML 4.0 document with no body text.

    \sa {Supported HTML Subset}, plainText
*/

#ifndef QT_NO_TEXTHTMLPARSER
void PageTextEdit::setHtml(const QString &text)
{
    Q_D(PageTextEdit);
    d->control->setHtml(text);
    d->updateDocumentGeometry();
    d->preferRichText = true;
}

QString PageTextEdit::toHtml() const
{
    Q_D(const PageTextEdit);
    return d->control->toHtml();
}
#endif


/*! \reimp
*/
void PageTextEdit::keyPressEvent(QKeyEvent *e)
{
    Q_D(PageTextEdit);

#ifdef QT_KEYPAD_NAVIGATION
    switch (e->key()) {
        case Qt::Key_Select:
            if (QApplication::keypadNavigationEnabled()) {
                // code assumes linksaccessible + editable isn't meaningful
                if (d->control->textInteractionFlags() & Qt::TextEditable) {
                    setEditFocus(!hasEditFocus());
                } else {
                    if (!hasEditFocus())
                        setEditFocus(true);
                    else {
                        QTextCursor cursor = d->control->textCursor();
                        QTextCharFormat charFmt = cursor.charFormat();
                        if (!(d->control->textInteractionFlags() & Qt::LinksAccessibleByKeyboard)
                            || !cursor.hasSelection() || charFmt.anchorHref().isEmpty()) {
                            e->accept();
                            return;
                        }
                    }
                }
            }
            break;
        case Qt::Key_Back:
        case Qt::Key_No:
            if (!QApplication::keypadNavigationEnabled()
                    || (QApplication::keypadNavigationEnabled() && !hasEditFocus())) {
                e->ignore();
                return;
            }
            break;
        default:
            if (QApplication::keypadNavigationEnabled()) {
                if (!hasEditFocus() && !(e->modifiers() & Qt::ControlModifier)) {
                    if (e->text()[0].isPrint())
                        setEditFocus(true);
                    else {
                        e->ignore();
                        return;
                    }
                }
            }
            break;
    }
#endif
#ifndef QT_NO_SHORTCUT

    Qt::TextInteractionFlags tif = d->control->textInteractionFlags();

    if (tif & Qt::TextSelectableByKeyboard){
        if (e == QKeySequence::SelectPreviousPage) {
            e->accept();
            d->pageUpDown(QTextCursor::Up, QTextCursor::KeepAnchor);
            return;
        } else if (e ==QKeySequence::SelectNextPage) {
            e->accept();
            d->pageUpDown(QTextCursor::Down, QTextCursor::KeepAnchor);
            return;
        }
    }
    if (tif & (Qt::TextSelectableByKeyboard | Qt::TextEditable)) {
        if (e == QKeySequence::MoveToPreviousPage) {
            e->accept();
            d->pageUpDown(QTextCursor::Up, QTextCursor::MoveAnchor);
            return;
        } else if (e == QKeySequence::MoveToNextPage) {
            e->accept();
            d->pageUpDown(QTextCursor::Down, QTextCursor::MoveAnchor);
            return;
        }
    }

    if (!(tif & Qt::TextEditable)) {
        switch (e->key()) {
            case Qt::Key_Space:
                e->accept();
                if (e->modifiers() & Qt::ShiftModifier)
                    d->vbar->triggerAction(QAbstractSlider::SliderPageStepSub);
                else
                    d->vbar->triggerAction(QAbstractSlider::SliderPageStepAdd);
                break;
            default:
                d->sendControlEvent(e);
                if (!e->isAccepted() && e->modifiers() == Qt::NoModifier) {
                    if (e->key() == Qt::Key_Home) {
                        d->vbar->triggerAction(QAbstractSlider::SliderToMinimum);
                        e->accept();
                    } else if (e->key() == Qt::Key_End) {
                        d->vbar->triggerAction(QAbstractSlider::SliderToMaximum);
                        e->accept();
                    }
                }
                if (!e->isAccepted()) {
                    QAbstractScrollArea::keyPressEvent(e);
                }
        }
        return;
    }
#endif // QT_NO_SHORTCUT

    {
        QTextCursor cursor = d->control->textCursor();
        const QString text = e->text();
        if (cursor.atBlockStart()
            && (d->autoFormatting & AutoBulletList)
            && (text.length() == 1)
            && (text.at(0) == QLatin1Char('-') || text.at(0) == QLatin1Char('*'))
            && (!cursor.currentList())) {

            d->createAutoBulletList();
            e->accept();
            return;
        }
    }

    d->sendControlEvent(e);
#ifdef QT_KEYPAD_NAVIGATION
    if (!e->isAccepted()) {
        switch (e->key()) {
            case Qt::Key_Up:
            case Qt::Key_Down:
                if (QApplication::keypadNavigationEnabled()) {
                    // Cursor position didn't change, so we want to leave
                    // these keys to change focus.
                    e->ignore();
                    return;
                }
                break;
            case Qt::Key_Back:
                if (!e->isAutoRepeat()) {
                    if (QApplication::keypadNavigationEnabled()) {
                        if (document()->isEmpty() || !(d->control->textInteractionFlags() & Qt::TextEditable)) {
                            setEditFocus(false);
                            e->accept();
                        } else if (!d->deleteAllTimer.isActive()) {
                            e->accept();
                            d->deleteAllTimer.start(750, this);
                        }
                    } else {
                        e->ignore();
                        return;
                    }
                }
                break;
            default: break;
        }
    }
#endif
}

/*! \reimp
*/
void PageTextEdit::keyReleaseEvent(QKeyEvent *e)
{
#ifdef QT_KEYPAD_NAVIGATION
    Q_D(PageTextEdit);
    if (QApplication::keypadNavigationEnabled()) {
        if (!e->isAutoRepeat() && e->key() == Qt::Key_Back
            && d->deleteAllTimer.isActive()) {
            d->deleteAllTimer.stop();
            QTextCursor cursor = d->control->textCursor();
            QTextBlockFormat blockFmt = cursor.blockFormat();

            QTextList *list = cursor.currentList();
            if (list && cursor.atBlockStart()) {
                list->remove(cursor.block());
            } else if (cursor.atBlockStart() && blockFmt.indent() > 0) {
                blockFmt.setIndent(blockFmt.indent() - 1);
                cursor.setBlockFormat(blockFmt);
            } else {
                cursor.deletePreviousChar();
            }
            setTextCursor(cursor);
            e->accept();
            return;
        }
    }
#endif
    e->ignore();
}

/*!
    Loads the resource specified by the given \a type and \a name.

    This function is an extension of QTextDocument::loadResource().

    \sa QTextDocument::loadResource()
*/
QVariant PageTextEdit::loadResource(int type, const QUrl &name)
{
    Q_UNUSED(type);
    Q_UNUSED(name);
    return QVariant();
}

void PageTextEditPrivate::updateViewportMargins()
{
    Q_Q(PageTextEdit);

    //
    // Формируем параметры отображения
    //
    QMargins viewportMargins;

    if (m_usePageMode) {
        //
        // Настроить размер документа
        //

        int pageWidth = m_pageMetrics.pxPageSize().width();
        int pageHeight = m_pageMetrics.pxPageSize().height();

        //
        // Рассчитываем отступы для viewport
        //
        const int DEFAULT_TOP_MARGIN = 0;
        const int DEFAULT_BOTTOM_MARGIN = 0;
        {
            int leftMargin = 0;
            int rightMargin = 0;

            //
            // Если ширина редактора больше ширины страницы документа, расширим боковые отступы
            //
            if (q->width() > pageWidth) {
                const int BORDERS_WIDTH = 4;
                const int VERTICAL_SCROLLBAR_WIDTH = vbar->isVisible() ? vbar->width() : 0;
                // ... ширина рамки вьюпорта и самого редактора
                leftMargin = rightMargin =
                        (q->width() - pageWidth - VERTICAL_SCROLLBAR_WIDTH - BORDERS_WIDTH) / 2;
            }

            const int topMargin = DEFAULT_TOP_MARGIN;

            //
            // Нижний оступ может быть больше минимального значения, для случая,
            // когда весь документ и даже больше помещается на экране
            //
            int bottomMargin = DEFAULT_BOTTOM_MARGIN;
            const int documentHeight = pageHeight * control->document()->pageCount();
            if ((q->height() - documentHeight) > (DEFAULT_TOP_MARGIN + DEFAULT_BOTTOM_MARGIN)) {
                const int BORDERS_HEIGHT = 2;
                const int HORIZONTAL_SCROLLBAR_HEIGHT = hbar->isVisible() ? hbar->height() : 0;
                bottomMargin =
                    q->height() - documentHeight - HORIZONTAL_SCROLLBAR_HEIGHT - DEFAULT_TOP_MARGIN - BORDERS_HEIGHT;
            }

            //
            // Настроим сами отступы
            //
            viewportMargins = QMargins(leftMargin, topMargin, rightMargin, bottomMargin);
        }
    }

    q->setViewportMargins(viewportMargins);
    updateDocumentGeometry();
}

void PageTextEditPrivate::updateDocumentGeometry()
{
    Q_Q(PageTextEdit);

    //
    // Определим размер документа
    //
    QSizeF documentSize(q->width() - vbar->width(), -1);
    if (m_usePageMode) {
        int pageWidth = m_pageMetrics.pxPageSize().width();
        int pageHeight = m_pageMetrics.pxPageSize().height();
        documentSize = QSizeF(pageWidth, pageHeight);
    }

    //
    // Обновим размер документа
    //
    QTextDocument* doc = control->document();
    if (doc->pageSize() != documentSize) {
        doc->setPageSize(documentSize);
    }

    //
    // Заодно и отступы настроим
    //
    // ... у документа уберём их
    //
    if (doc->documentMargin() != 0) {
        doc->setDocumentMargin(0);
    }
    //
    // ... и настроим поля документа
    //
    QMarginsF rootFrameMargins = m_pageMetrics.pxPageMargins();
    QTextFrameFormat rootFrameFormat = doc->rootFrame()->frameFormat();
    if (rootFrameFormat.leftMargin() != rootFrameMargins.left()
        || rootFrameFormat.topMargin() != rootFrameMargins.top()
        || rootFrameFormat.rightMargin() != rootFrameMargins.right()
        || rootFrameFormat.bottomMargin() != rootFrameMargins.bottom()) {
        rootFrameFormat.setLeftMargin(rootFrameMargins.left());
        rootFrameFormat.setTopMargin(rootFrameMargins.top());
        rootFrameFormat.setRightMargin(rootFrameMargins.right());
        rootFrameFormat.setBottomMargin(rootFrameMargins.bottom());
        doc->rootFrame()->setFrameFormat(rootFrameFormat);
    }
}

/*! \reimp
*/
void PageTextEdit::resizeEvent(QResizeEvent *e)
{
    Q_D(PageTextEdit);

    if (d->lineWrap == NoWrap) {
        QTextDocument *doc = d->control->document();
        QVariant alignmentProperty = doc->documentLayout()->property("contentHasAlignment");

        if (!doc->pageSize().isNull()
            && alignmentProperty.type() == QVariant::Bool
            && !alignmentProperty.toBool()) {

            d->_q_adjustScrollbars();
            return;
        }
    }

    //
    // Если изменился размер
    //
    if (d->lineWrap != FixedPixelWidth
        && e->oldSize().width() != e->size().width()) {
        //
        // Обновим отступы вьюпорта
        //
        d->updateViewportMargins();
        //
        // и переформируем документ, если это не постраничный режим
        //
        if (d->m_usePageMode) {
            d->_q_adjustScrollbars();
        } else {
            d->relayoutDocument();
        }
    }
    else
        d->_q_adjustScrollbars();
}

void PageTextEditPrivate::relayoutDocument()
{
    updateViewportMargins();

    QTextDocument *doc = control->document();
    QAbstractTextDocumentLayout *layout = doc->documentLayout();

    if (QTextDocumentLayout *tlayout = qobject_cast<QTextDocumentLayout *>(layout)) {
        if (lineWrap == PageTextEdit::FixedColumnWidth)
            tlayout->setFixedColumnWidth(lineWrapColumnOrWidth);
        else
            tlayout->setFixedColumnWidth(-1);
    }

    QTextDocumentLayout *tlayout = qobject_cast<QTextDocumentLayout *>(layout);
    QSize lastUsedSize;
    if (tlayout)
        lastUsedSize = tlayout->dynamicDocumentSize().toSize();
    else
        lastUsedSize = layout->documentSize().toSize();

    // ignore calls to _q_adjustScrollbars caused by an emission of the
    // usedSizeChanged() signal in the layout, as we're calling it
    // later on our own anyway (or deliberately not) .
    const bool oldIgnoreScrollbarAdjustment = ignoreAutomaticScrollbarAdjustment;
    ignoreAutomaticScrollbarAdjustment = true;

    int width = viewport->width();
    if (lineWrap == PageTextEdit::FixedPixelWidth)
        width = lineWrapColumnOrWidth;
    else if (lineWrap == PageTextEdit::NoWrap) {
        QVariant alignmentProperty = doc->documentLayout()->property("contentHasAlignment");
        if (alignmentProperty.type() == QVariant::Bool && !alignmentProperty.toBool()) {

            width = 0;
        }
    }

    //
    // Сбрасываем размер, чтобы перерисовка произошла корректно
    //
    if (m_usePageMode) {
        QSizeF lastSize = doc->pageSize();
        doc->setPageSize(QSize(width, -1));
        doc->setPageSize(lastSize);
    } else {
        doc->setPageSize(QSize(width, -1));
    }

    if (tlayout)
        tlayout->ensureLayouted(verticalOffset() + viewport->height());

    ignoreAutomaticScrollbarAdjustment = oldIgnoreScrollbarAdjustment;

    QSize usedSize;
    if (tlayout)
        usedSize = tlayout->dynamicDocumentSize().toSize();
    else
        usedSize = layout->documentSize().toSize();

    // this is an obscure situation in the layout that can happen:
    // if a character at the end of a line is the tallest one and therefore
    // influencing the total height of the line and the line right below it
    // is always taller though, then it can happen that if due to line breaking
    // that tall character wraps into the lower line the document not only shrinks
    // horizontally (causing the character to wrap in the first place) but also
    // vertically, because the original line is now smaller and the one below kept
    // its size. So a layout with less width _can_ take up less vertical space, too.
    // If the wider case causes a vertical scroll bar to appear and the narrower one
    // (narrower because the vertical scroll bar takes up horizontal space)) to disappear
    // again then we have an endless loop, as _q_adjustScrollBars sets new ranges on the
    // scroll bars, the QAbstractScrollArea will find out about it and try to show/hide
    // the scroll bars again. That's why we try to detect this case here and break out.
    //
    // (if you change this please also check the layoutingLoop() testcase in
    // PageTextEdit's autotests)
    if (lastUsedSize.isValid()
        && !vbar->isHidden()
        && viewport->width() < lastUsedSize.width()
        && usedSize.height() < lastUsedSize.height()
        && usedSize.height() <= viewport->height())
        return;

    _q_adjustScrollbars();
}

void PageTextEditPrivate::paintPagesView(QPainter *_painter)
{
    Q_Q(PageTextEdit);

    //
    // Оформление рисуется только тогда, когда редактор находится в постраничном режиме
    //
    if (m_usePageMode) {
        _painter->save();

        //
        // Нарисовать линии разрыва страниц
        //

        qreal pageWidth = m_pageMetrics.pxPageSize().width();
        qreal pageHeight = m_pageMetrics.pxPageSize().height();

        QPen spacePen(control->palette().window(), 8);
        QPen borderPen(control->palette().dark(), 1);

        qreal curHeight = pageHeight - (vbar->value() % (int)pageHeight);
        //
        // Корректируем позицию правой границы
        //
        const int x = pageWidth + (q->width() % 2 == 0 ? 2 : 1);
        //
        // Смещение по горизонтали, если есть полоса прокрутки
        //
        const int horizontalDelta = hbar->value();

        //
        // Нарисовать верхнюю границу
        //
        if (curHeight - pageHeight >= 0) {
            _painter->setPen(borderPen);
            // ... верхняя
            _painter->drawLine(0, curHeight - pageHeight, x, curHeight - pageHeight);
        }

        while (curHeight <= q->height()) {
            //
            // Фон разрыва страниц
            //
            _painter->setPen(spacePen);
            _painter->drawLine(0, curHeight-4, q->width(), curHeight-4);

            //
            // Границы страницы
            //
            _painter->setPen(borderPen);
            // ... нижняя
            _painter->drawLine(0, curHeight-8, x, curHeight-8);
            // ... верхняя следующей страницы
            _painter->drawLine(0, curHeight, x, curHeight);
            // ... левая
            _painter->drawLine(0 - horizontalDelta, curHeight - pageHeight, 0 - horizontalDelta, curHeight - 8);
            // ... правая
            _painter->drawLine(x - horizontalDelta, curHeight - pageHeight, x - horizontalDelta, curHeight - 8);

            curHeight += pageHeight;
        }

        //
        // Нарисовать боковые границы страницы, когда страница не влезает в экран
        //
        if (curHeight >= q->height()) {
            //
            // Границы страницы
            //
            _painter->setPen(borderPen);
            // ... левая
            _painter->drawLine(0 - horizontalDelta, curHeight-pageHeight, 0 - horizontalDelta, q->height());
            // ... правая
            _painter->drawLine(x - horizontalDelta, curHeight-pageHeight, x - horizontalDelta, q->height());
        }

        _painter->restore();
    }
}

void PageTextEditPrivate::paintPageNumbers(QPainter* _painter)
{
    Q_Q(PageTextEdit);

    //
    // Номера страниц рисуются только тогда, когда редактор находится в постраничном режиме,
    // если заданы поля и включена опция отображения номеров
    //
    if (m_usePageMode && !m_pageMetrics.pxPageMargins().isNull() && m_showPageNumbers) {
        _painter->save();

        //
        // Нарисовать номера страниц
        //

        QSizeF pageSize(m_pageMetrics.pxPageSize());
        QMarginsF pageMargins(m_pageMetrics.pxPageMargins());

        _painter->setFont(control->document()->defaultFont());
        _painter->setPen(QPen(control->palette().text(), 1));

        //
        // Текущие высота и ширина которые отображаются на экране
        //
        qreal curHeight = pageSize.height() - (vbar->value() % (int)pageSize.height());

        //
        // Начало поля должно учитывать смещение полосы прокрутки
        //
        qreal leftMarginPosition = pageMargins.left() - hbar->value();
        //
        // Итоговая ширина поля
        //
        qreal marginWidth = pageSize.width() - pageMargins.left() - pageMargins.right();

        //
        // Номер первой видимой на экране страницы
        //
        int pageNumber = vbar->value() / (int)pageSize.height() + 1;

        //
        // Верхнее поле первой страницы на экране, когда не видно предыдущей страницы
        //
        if (curHeight - pageMargins.top() >= 0) {
            QRectF topMarginRect(leftMarginPosition, curHeight - pageSize.height(), marginWidth, pageMargins.top());
            paintPageNumber(_painter, topMarginRect, true, pageNumber);
        }

        //
        // Для всех видимых страниц
        //
        while (curHeight < q->height()) {
            //
            // Определить прямоугольник нижнего поля
            //
            QRect bottomMarginRect(leftMarginPosition, curHeight - pageMargins.bottom(), marginWidth, pageMargins.bottom());
            paintPageNumber(_painter, bottomMarginRect, false, pageNumber);

            //
            // Переход к следующей странице
            //
            ++pageNumber;

            //
            // Определить прямоугольник верхнего поля следующей страницы
            //
            QRect topMarginRect(leftMarginPosition, curHeight, marginWidth, pageMargins.top());
            paintPageNumber(_painter, topMarginRect, true, pageNumber);

            curHeight += pageSize.height();
        }

        _painter->restore();
    }
}

void PageTextEditPrivate::paintPageNumber(QPainter* _painter, const QRectF& _rect, bool _isHeader, int _number)
{
    //
    // Верхнее поле
    //
    if (_isHeader) {
        //
        // Если нумерация рисуется в верхнем поле
        //
        if (m_pageNumbersAlignment.testFlag(Qt::AlignTop)) {
            _painter->drawText(_rect, Qt::AlignVCenter | (m_pageNumbersAlignment ^ Qt::AlignTop),
                QString::number(_number));
        }
    }
    //
    // Нижнее поле
    //
    else {
        //
        // Если нумерация рисуется в нижнем поле
        //
        if (m_pageNumbersAlignment.testFlag(Qt::AlignBottom)) {
            _painter->drawText(_rect, Qt::AlignVCenter | (m_pageNumbersAlignment ^ Qt::AlignBottom),
                QString::number(_number));
        }
    }
}

void PageTextEditPrivate::paintWatermark(QPainter* _painter)
{
    if (!m_watermark.isEmpty()) {
        _painter->save();

        QColor watermarkColor = QColor("#C9C5C2");
        watermarkColor.setAlpha(50);

        //
        // Рисуем водяные знаки
        //
        _painter->setFont(QFont("Sans", 58, QFont::Black));
        _painter->setPen(watermarkColor);
        _painter->rotate(45);
        _painter->drawText(0, 0, m_watermarkMulti);

        _painter->restore();
    }
}

void PageTextEditPrivate::clipPageDecorationRegions(QPainter* _painter)
{
    Q_Q(PageTextEdit);

    //
    // Область обрезается только тогда, когда редактор находится в постраничном режиме и если заданы поля
    //
    if (m_usePageMode && !m_pageMetrics.pxPageMargins().isNull()) {
        QSizeF pageSize(m_pageMetrics.pxPageSize());
        QMarginsF pageMargins(m_pageMetrics.pxPageMargins());

        //
        // Текущие высота и ширина которые отображаются на экране
        //
        qreal curHeight = pageSize.height() - (vbar->value() % int(pageSize.height()));

        //
        // Начало поля должно учитывать смещение полосы прокрутки
        //
        qreal leftMarginPosition = 0;
        //
        // Итоговая ширина поля
        //
        qreal marginWidth = pageSize.width();

        //
        // Формируем область
        //
        QRegion clipPath(viewport->rect());

        //
        // Верхнее поле первой страницы на экране, когда не видно предыдущей страницы
        //
        if (curHeight - pageMargins.top() >= 0) {
            QRect topMarginRect(leftMarginPosition, curHeight - pageSize.height(), marginWidth, pageMargins.top());
            clipPath = clipPath.xored(topMarginRect);
        }

        //
        // Для всех видимых страниц
        //
        while (curHeight < q->height()) {
            //
            // Определить прямоугольник нижнего поля
            //
            QRect bottomMarginRect(leftMarginPosition, curHeight - pageMargins.bottom(), marginWidth, pageMargins.bottom());
            clipPath = clipPath.xored(bottomMarginRect);

            //
            // Определить прямоугольник верхнего поля следующей страницы
            //
            QRect topMarginRect(leftMarginPosition, curHeight, marginWidth, pageMargins.top());
            clipPath = clipPath.xored(topMarginRect);

            curHeight += pageSize.height();
        }

        //
        // Определить прямоугольник нижнего поля когда на экране одна страница
        //
        QRect bottomMarginRect(leftMarginPosition, curHeight - pageMargins.bottom(), marginWidth, pageMargins.bottom());
        clipPath = clipPath.xored(bottomMarginRect);

        _painter->setClipRegion(clipPath);
    }
}

void PageTextEditPrivate::sendControlMouseEvent(QMouseEvent* e)
{
    QScopedPointer<QMouseEvent> correctedEvent(
        new QMouseEvent(e->type(), correctMousePosition(e->pos()), e->button(), e->buttons(), e->modifiers()));
    sendControlEvent(correctedEvent.data());
    if (correctedEvent->isAccepted()) {
        e->accept();
    } else {
        e->ignore();
    }
}

void PageTextEditPrivate::sendControlContextMenuEvent(QContextMenuEvent* e)
{
    QScopedPointer<QContextMenuEvent> correctedEvent(
        new QContextMenuEvent(e->reason(), correctMousePosition(e->pos())));
    sendControlEvent(correctedEvent.data());
    if (correctedEvent->isAccepted()) {
        e->accept();
    } else {
        e->ignore();
    }
}

QPoint PageTextEditPrivate::correctMousePosition(const QPoint& _eventPos)
{
    Q_Q(PageTextEdit);

    //
    // Когда в документе присутствуют невидимые блоки, то при щелчке мышъю на межстрочном отступе
    // курсор улетает далеко вниз от того места где был произведён щелчёк, по видимому это какой-то
    // внутренний баг, исправить который можно только через QTextDocumentLayout. Сделать это малой
    // кровью мне не удалось, поэтому пришлось придумать данную заплатку.
    //
    // Суть её заключается в том, чтобы найти ближайшую корректную позицию мыши.
    //

    QPoint localPos = viewport->mapFromParent(_eventPos);
    QTextCursor cursor = q->textCursor();
    //
    // Если получили блок над указателем
    //
    if (!cursor.isNull()
        && q->cursorRect(cursor).bottom() < localPos.y()) {
        //
        // Ищем ближайший к указателю блок
        //
        cursor.movePosition(QTextCursor::StartOfBlock);
        int minSpace = INT_MAX;
        bool firstStep = true;
        do {
            //
            // Если это не первый проход, смещаем курсор
            //
            if (firstStep) {
                firstStep = false;
            } else {
                cursor.movePosition(cursor.atBlockEnd() ? QTextCursor::NextBlock : QTextCursor::EndOfBlock);
            }

            if (cursor.block().isVisible()) {
                const int space = abs(localPos.y() - q->cursorRect(cursor).center().y());
                //
                // Если верхний и нижний блоки одинаково близки, используем нижний
                //
                if (minSpace >= space) {
                    minSpace = space;
                } else {
                    if (cursor.atBlockStart()) {
                        do {
                            cursor.movePosition(QTextCursor::PreviousBlock);
                            cursor.movePosition(QTextCursor::StartOfBlock);
                        } while (!cursor.atStart() && !cursor.block().isVisible());
                    }
                    break;
                }
            }
        } while (!cursor.atEnd());
    }
    //
    // Если получили блок под указателем
    //
    else {
        //
        // Ищем ближайший к указателю блок
        //
        cursor.movePosition(QTextCursor::EndOfBlock);
        int minSpace = INT_MAX;
        bool firstStep = true;
        do {
            //
            // Если это не первый проход, смещаем курсор
            //
            if (firstStep) {
                firstStep = false;
            } else {
                cursor.movePosition(cursor.atBlockStart() ? QTextCursor::PreviousCharacter : QTextCursor::StartOfBlock);
            }

            if (cursor.block().isVisible()) {
                const int space = abs(localPos.y() - q->cursorRect(cursor).center().y());
                //
                // Всегда используем более низкий блок
                //
                if (minSpace > space
                    || (minSpace == space && cursor.atBlockStart())) {
                    minSpace = space;
                } else {
                    if (cursor.atBlockEnd()) {
                        do {
                            cursor.movePosition(QTextCursor::NextBlock);
                            cursor.movePosition(QTextCursor::EndOfBlock);
                        } while (!cursor.atEnd() && !cursor.block().isVisible());
                    }
                    break;
                }
            }
        } while (!cursor.atStart());
    }

    //
    // Обрабатываем случай, когда курсор убежал в самый низ и стоит в невидимом блоке.
    // Просто идём назад, до первого видимого блока
    //
    if (cursor.atEnd() && !cursor.block().isVisible()) {
        while (!cursor.atStart() && !cursor.block().isVisible()) {
            cursor.movePosition(QTextCursor::PreviousBlock);
            cursor.movePosition(QTextCursor::StartOfBlock);
        }
    }

    //
    // Если блок не пуст
    //
    if (!cursor.block().text().isEmpty()) {
        //
        // Ищем наилучшее совпадение внутри блока, т.к. он может быть многострочным
        //
        cursor.movePosition(QTextCursor::StartOfBlock);
        int minSpace = INT_MAX;
        do {
            const int space = abs(localPos.y() - q->cursorRect(cursor).center().y());
            if (minSpace >= space) {
                minSpace = space;
            } else {
                if (!cursor.atBlockStart()) {
                    cursor.movePosition(QTextCursor::PreviousCharacter);
                }
                break;
            }
            cursor.movePosition(QTextCursor::NextCharacter);
        } while (!cursor.atBlockEnd());
    }

    //
    // Настраиваем координаты найденной точки
    //
    localPos.setY(q->cursorRect(cursor).center().y());
    localPos = viewport->mapToParent(localPos);

    return localPos;
//	return new QMouseEvent(_event->type(), localPos, _event->button(), _event->buttons(), _event->modifiers());
}

void PageTextEditPrivate::paint(QPainter *p, QPaintEvent *e)
{
    paintPagesView(p);
    paintPageNumbers(p);
    paintWatermark(p);

    const int xOffset = horizontalOffset();
    const int yOffset = verticalOffset();

    QRect r = e->rect();
    p->translate(-xOffset, -yOffset);
    r.translate(xOffset, yOffset);

    QTextDocument *doc = control->document();
    QTextDocumentLayout *layout = qobject_cast<QTextDocumentLayout *>(doc->documentLayout());

    // the layout might need to expand the root frame to
    // the viewport if NoWrap is set
    if (layout)
        layout->setViewport(viewport->rect());

    control->drawContents(p, r, q_func());

    if (layout)
        layout->setViewport(QRect());

    if (!placeholderText.isEmpty() && doc->isEmpty()) {
        QColor col = control->palette().text().color();
        col.setAlpha(128);
        p->setPen(col);
        const int margin = int(doc->documentMargin());
        p->drawText(viewport->rect().adjusted(margin, margin, -margin, -margin), Qt::AlignTop | Qt::TextWordWrap, placeholderText);
    }
}

/*! \fn void PageTextEdit::paintEvent(QPaintEvent *event)

This event handler can be reimplemented in a subclass to receive paint events passed in \a event.
It is usually unnecessary to reimplement this function in a subclass of PageTextEdit.

\warning The underlying text document must not be modified from within a reimplementation
of this function.
*/
void PageTextEdit::paintEvent(QPaintEvent *e)
{
    Q_D(PageTextEdit);
    QPainter p(d->viewport);
    d->paint(&p, e);
}

void PageTextEditPrivate::_q_currentCharFormatChanged(const QTextCharFormat &fmt)
{
    Q_Q(PageTextEdit);
    emit q->currentCharFormatChanged(fmt);
}

void PageTextEditPrivate::updateDefaultTextOption()
{
    QTextDocument *doc = control->document();

    QTextOption opt = doc->defaultTextOption();
    QTextOption::WrapMode oldWrapMode = opt.wrapMode();

    if (lineWrap == PageTextEdit::NoWrap)
        opt.setWrapMode(QTextOption::NoWrap);
    else
        opt.setWrapMode(wordWrap);

    if (opt.wrapMode() != oldWrapMode)
        doc->setDefaultTextOption(opt);
}

/*! \reimp
*/
void PageTextEdit::mousePressEvent(QMouseEvent *e)
{
    Q_D(PageTextEdit);
#ifdef QT_KEYPAD_NAVIGATION
    if (QApplication::keypadNavigationEnabled() && !hasEditFocus())
        setEditFocus(true);
#endif
    d->sendControlMouseEvent(e);
}

/*! \reimp
*/
void PageTextEdit::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(PageTextEdit);
    d->inDrag = false; // paranoia
    d->sendControlMouseEvent(e);
    const QPoint pos = e->pos();
    if (!(e->buttons() & Qt::LeftButton))
        return;
    if (e->source() == Qt::MouseEventNotSynthesized) {
        const QRect visible = d->viewport->rect();
        if (visible.contains(pos))
            d->autoScrollTimer.stop();
        else if (!d->autoScrollTimer.isActive())
            d->autoScrollTimer.start(100, this);
    }
}

/*! \reimp
*/
void PageTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(PageTextEdit);
    d->sendControlMouseEvent(e);
    if (e->source() == Qt::MouseEventNotSynthesized && d->autoScrollTimer.isActive()) {
        d->autoScrollTimer.stop();
        ensureCursorVisible();
    }
    if (!isReadOnly() && rect().contains(e->pos()))
        d->handleSoftwareInputPanel(e->button(), d->clickCausedFocus);
    d->clickCausedFocus = 0;
}

/*! \reimp
*/
void PageTextEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_D(PageTextEdit);
    d->sendControlMouseEvent(e);
}

/*! \reimp
*/
bool PageTextEdit::focusNextPrevChild(bool next)
{
    Q_D(const PageTextEdit);
    if (!d->tabChangesFocus && d->control->textInteractionFlags() & Qt::TextEditable)
        return false;
    return QAbstractScrollArea::focusNextPrevChild(next);
}

#ifndef QT_NO_CONTEXTMENU
/*!
  \fn void PageTextEdit::contextMenuEvent(QContextMenuEvent *event)

  Shows the standard context menu created with createStandardContextMenu().

  If you do not want the text edit to have a context menu, you can set
  its \l contextMenuPolicy to Qt::NoContextMenu. If you want to
  customize the context menu, reimplement this function. If you want
  to extend the standard context menu, reimplement this function, call
  createStandardContextMenu() and extend the menu returned.

  Information about the event is passed in the \a event object.

  \snippet code/src_gui_widgets_qtextedit.cpp 0
*/
void PageTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    Q_D(PageTextEdit);
    d->sendControlContextMenuEvent(e);
}
#endif // QT_NO_CONTEXTMENU

#ifndef QT_NO_DRAGANDDROP
/*! \reimp
*/
void PageTextEdit::dragEnterEvent(QDragEnterEvent *e)
{
    Q_D(PageTextEdit);
    d->inDrag = true;
    d->sendControlEvent(e);
}

/*! \reimp
*/
void PageTextEdit::dragLeaveEvent(QDragLeaveEvent *e)
{
    Q_D(PageTextEdit);
    d->inDrag = false;
    d->autoScrollTimer.stop();
    d->sendControlEvent(e);
}

/*! \reimp
*/
void PageTextEdit::dragMoveEvent(QDragMoveEvent *e)
{
    Q_D(PageTextEdit);
    d->autoScrollDragPos = e->pos();
    if (!d->autoScrollTimer.isActive())
        d->autoScrollTimer.start(100, this);
    d->sendControlEvent(e);
}

/*! \reimp
*/
void PageTextEdit::dropEvent(QDropEvent *e)
{
    Q_D(PageTextEdit);
    d->inDrag = false;
    d->autoScrollTimer.stop();
    d->sendControlEvent(e);
}

#endif // QT_NO_DRAGANDDROP

/*! \reimp
 */
void PageTextEdit::inputMethodEvent(QInputMethodEvent *e)
{
    Q_D(PageTextEdit);
#ifdef QT_KEYPAD_NAVIGATION
    if (d->control->textInteractionFlags() & Qt::TextEditable
        && QApplication::keypadNavigationEnabled()
        && !hasEditFocus())
        setEditFocus(true);
#endif
    d->sendControlEvent(e);
    ensureCursorVisible();
}

/*!\reimp
*/
void PageTextEdit::scrollContentsBy(int dx, int dy)
{
    Q_D(PageTextEdit);
    if (isRightToLeft())
        dx = -dx;
    d->viewport->scroll(dx, dy);
}

/*!\reimp
*/
QVariant PageTextEdit::inputMethodQuery(Qt::InputMethodQuery property) const
{
    return inputMethodQuery(property, QVariant());
}

/*!\internal
 */
QVariant PageTextEdit::inputMethodQuery(Qt::InputMethodQuery query, QVariant argument) const
{
    Q_D(const PageTextEdit);
    QVariant v;
    switch (query) {
    case Qt::ImHints:
        v = QWidget::inputMethodQuery(query);
        break;
    default:
        v = d->control->inputMethodQuery(query, argument);
        const QPoint offset(-d->horizontalOffset(), -d->verticalOffset());
        if (v.type() == QVariant::RectF)
            v = v.toRectF().toRect().translated(offset);
        else if (v.type() == QVariant::PointF)
            v = v.toPointF().toPoint() + offset;
        else if (v.type() == QVariant::Rect)
            v = v.toRect().translated(offset);
        else if (v.type() == QVariant::Point)
            v = v.toPoint() + offset;
    }

    return v;
}

/*! \reimp
*/
void PageTextEdit::focusInEvent(QFocusEvent *e)
{
    Q_D(PageTextEdit);
    if (e->reason() == Qt::MouseFocusReason) {
        d->clickCausedFocus = 1;
    }
    QAbstractScrollArea::focusInEvent(e);
    d->sendControlEvent(e);
}

/*! \reimp
*/
void PageTextEdit::focusOutEvent(QFocusEvent *e)
{
    Q_D(PageTextEdit);
    QAbstractScrollArea::focusOutEvent(e);
    d->sendControlEvent(e);
}

/*! \reimp
*/
void PageTextEdit::showEvent(QShowEvent *)
{
    Q_D(PageTextEdit);
    if (!d->anchorToScrollToWhenVisible.isEmpty()) {
        scrollToAnchor(d->anchorToScrollToWhenVisible);
        d->anchorToScrollToWhenVisible.clear();
        d->showCursorOnInitialShow = false;
    } else if (d->showCursorOnInitialShow) {
        d->showCursorOnInitialShow = false;
        ensureCursorVisible();
    }
}

/*! \reimp
*/
void PageTextEdit::changeEvent(QEvent *e)
{
    Q_D(PageTextEdit);
    QAbstractScrollArea::changeEvent(e);
    if (e->type() == QEvent::ApplicationFontChange
        || e->type() == QEvent::FontChange) {
        d->control->document()->setDefaultFont(font());
    }  else if(e->type() == QEvent::ActivationChange) {
        if (!isActiveWindow())
            d->autoScrollTimer.stop();
    } else if (e->type() == QEvent::EnabledChange) {
        e->setAccepted(isEnabled());
        d->control->setPalette(palette());
        d->sendControlEvent(e);
    } else if (e->type() == QEvent::PaletteChange) {
        d->control->setPalette(palette());
    } else if (e->type() == QEvent::LayoutDirectionChange) {
        d->sendControlEvent(e);
    }
}

/*! \reimp
*/
#ifndef QT_NO_WHEELEVENT
void PageTextEdit::wheelEvent(QWheelEvent *e)
{
    Q_D(PageTextEdit);
    if (!(d->control->textInteractionFlags() & Qt::TextEditable)) {
        if (e->modifiers() & Qt::ControlModifier) {
            float delta = e->angleDelta().y() / 120.f;
            zoomInF(delta);
            return;
        }
    }
    QAbstractScrollArea::wheelEvent(e);
    updateMicroFocus();
}
#endif

#ifndef QT_NO_CONTEXTMENU
/*!  This function creates the standard context menu which is shown
  when the user clicks on the text edit with the right mouse
  button. It is called from the default contextMenuEvent() handler.
  The popup menu's ownership is transferred to the caller.

  We recommend that you use the createStandardContextMenu(QPoint) version instead
  which will enable the actions that are sensitive to where the user clicked.
*/

QMenu *PageTextEdit::createStandardContextMenu(QWidget* _parent)
{
    Q_D(PageTextEdit);
    return d->control->createStandardContextMenu(QPoint(), _parent == 0 ? this : _parent);
}

/*!
  \since 4.4
  This function creates the standard context menu which is shown
  when the user clicks on the text edit with the right mouse
  button. It is called from the default contextMenuEvent() handler
  and it takes the \a position in document coordinates where the mouse click was.
  This can enable actions that are sensitive to the position where the user clicked.
  The popup menu's ownership is transferred to the caller.
*/

QMenu *PageTextEdit::createStandardContextMenu(const QPoint &position, QWidget* _parent)
{
    Q_D(PageTextEdit);
    return d->control->createStandardContextMenu(position, _parent == 0 ? this : _parent);
}
#endif // QT_NO_CONTEXTMENU

/*!
  returns a QTextCursor at position \a pos (in viewport coordinates).
*/
QTextCursor PageTextEdit::cursorForPosition(const QPoint &pos) const
{
    Q_D(const PageTextEdit);
    return d->control->cursorForPosition(d->mapToContents(pos));
}

/*!
  returns a rectangle (in viewport coordinates) that includes the
  \a cursor.
 */
QRect PageTextEdit::cursorRect(const QTextCursor &cursor) const
{
    Q_D(const PageTextEdit);
    if (cursor.isNull())
        return QRect();

    QRect r = d->control->cursorRect(cursor).toRect();
    r.translate(-d->horizontalOffset(),-d->verticalOffset());
    return r;
}

/*!
  returns a rectangle (in viewport coordinates) that includes the
  cursor of the text edit.
 */
QRect PageTextEdit::cursorRect() const
{
    Q_D(const PageTextEdit);
    QRect r = d->control->cursorRect().toRect();
    r.translate(-d->horizontalOffset(),-d->verticalOffset());
    return r;
}


/*!
    Returns the reference of the anchor at position \a pos, or an
    empty string if no anchor exists at that point.
*/
QString PageTextEdit::anchorAt(const QPoint& pos) const
{
    Q_D(const PageTextEdit);
    return d->control->anchorAt(d->mapToContents(pos));
}

/*!
   \property PageTextEdit::overwriteMode
   \since 4.1
   \brief whether text entered by the user will overwrite existing text

   As with many text editors, the text editor widget can be configured
   to insert or overwrite existing text with new text entered by the user.

   If this property is \c true, existing text is overwritten, character-for-character
   by new text; otherwise, text is inserted at the cursor position, displacing
   existing text.

   By default, this property is \c false (new text does not overwrite existing text).
*/

bool PageTextEdit::overwriteMode() const
{
    Q_D(const PageTextEdit);
    return d->control->overwriteMode();
}

void PageTextEdit::setOverwriteMode(bool overwrite)
{
    Q_D(PageTextEdit);
    d->control->setOverwriteMode(overwrite);
}

/*!
    \property PageTextEdit::tabStopWidth
    \brief the tab stop width in pixels
    \since 4.1

    By default, this property contains a value of 80 pixels.
*/

int PageTextEdit::tabStopWidth() const
{
    Q_D(const PageTextEdit);
    return qRound(d->control->document()->defaultTextOption().tabStop());
}

void PageTextEdit::setTabStopWidth(int width)
{
    Q_D(PageTextEdit);
    QTextOption opt = d->control->document()->defaultTextOption();
    if (opt.tabStop() == width || width < 0)
        return;
    opt.setTabStop(width);
    d->control->document()->setDefaultTextOption(opt);
}

/*!
    \since 4.2
    \property PageTextEdit::cursorWidth

    This property specifies the width of the cursor in pixels. The default value is 1.
*/
int PageTextEdit::cursorWidth() const
{
    Q_D(const PageTextEdit);
    return d->control->cursorWidth();
}

void PageTextEdit::setCursorWidth(int width)
{
    Q_D(PageTextEdit);
    d->control->setCursorWidth(width);
}

/*!
    \property PageTextEdit::acceptRichText
    \brief whether the text edit accepts rich text insertions by the user
    \since 4.1

    When this propery is set to false text edit will accept only
    plain text input from the user. For example through clipboard or drag and drop.

    This property's default is true.
*/

bool PageTextEdit::acceptRichText() const
{
    Q_D(const PageTextEdit);
    return d->control->acceptRichText();
}

void PageTextEdit::setAcceptRichText(bool accept)
{
    Q_D(PageTextEdit);
    d->control->setAcceptRichText(accept);
}

/*!
    \class PageTextEdit::ExtraSelection
    \since 4.2
    \inmodule QtWidgets

    \brief The PageTextEdit::ExtraSelection structure provides a way of specifying a
           character format for a given selection in a document
*/

/*!
    \variable PageTextEdit::ExtraSelection::cursor
    A cursor that contains a selection in a QTextDocument
*/

/*!
    \variable PageTextEdit::ExtraSelection::format
    A format that is used to specify a foreground or background brush/color
    for the selection.
*/

///*!
//	\since 4.2
//	This function allows temporarily marking certain regions in the document
//	with a given color, specified as \a selections. This can be useful for
//	example in a programming editor to mark a whole line of text with a given
//	background color to indicate the existence of a breakpoint.

//	\sa PageTextEdit::ExtraSelection, extraSelections()
//*/
//void PageTextEdit::setExtraSelections(const QList<ExtraSelection> &selections)
//{
//	Q_D(PageTextEdit);
//	d->control->setExtraSelections(selections);
//}

///*!
//	\since 4.2
//	Returns previously set extra selections.

//	\sa setExtraSelections()
//*/
//QList<PageTextEdit::ExtraSelection> PageTextEdit::extraSelections() const
//{
//	Q_D(const PageTextEdit);
//	return d->control->extraSelections();
//}

/*!
    This function returns a new MIME data object to represent the contents
    of the text edit's current selection. It is called when the selection needs
    to be encapsulated into a new QMimeData object; for example, when a drag
    and drop operation is started, or when data is copied to the clipboard.

    If you reimplement this function, note that the ownership of the returned
    QMimeData object is passed to the caller. The selection can be retrieved
    by using the textCursor() function.
*/
QMimeData *PageTextEdit::createMimeDataFromSelection() const
{
    Q_D(const PageTextEdit);
    return d->control->QWidgetTextControl::createMimeDataFromSelection();
}

/*!
    This function returns \c true if the contents of the MIME data object, specified
    by \a source, can be decoded and inserted into the document. It is called
    for example when during a drag operation the mouse enters this widget and it
    is necessary to determine whether it is possible to accept the drag and drop
    operation.

    Reimplement this function to enable drag and drop support for additional MIME types.
 */
bool PageTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    Q_D(const PageTextEdit);
    return d->control->QWidgetTextControl::canInsertFromMimeData(source);
}

/*!
    This function inserts the contents of the MIME data object, specified
    by \a source, into the text edit at the current cursor position. It is
    called whenever text is inserted as the result of a clipboard paste
    operation, or when the text edit accepts data from a drag and drop
    operation.

    Reimplement this function to enable drag and drop support for additional MIME types.
 */
void PageTextEdit::insertFromMimeData(const QMimeData *source)
{
    Q_D(PageTextEdit);
    d->control->QWidgetTextControl::insertFromMimeData(source);
}

/*!
    \property PageTextEdit::readOnly
    \brief whether the text edit is read-only

    In a read-only text edit the user can only navigate through the
    text and select text; modifying the text is not possible.

    This property's default is false.
*/

bool PageTextEdit::isReadOnly() const
{
    Q_D(const PageTextEdit);
    return !(d->control->textInteractionFlags() & Qt::TextEditable);
}

void PageTextEdit::setReadOnly(bool ro)
{
    Q_D(PageTextEdit);
    Qt::TextInteractionFlags flags = Qt::NoTextInteraction;
    if (ro) {
        flags = Qt::TextSelectableByMouse;
#ifndef QT_NO_TEXTBROWSER
        if (qobject_cast<QTextBrowser *>(this))
            flags |= Qt::TextBrowserInteraction;
#endif
    } else {
        flags = Qt::TextEditorInteraction;
    }
    d->control->setTextInteractionFlags(flags);
    setAttribute(Qt::WA_InputMethodEnabled, shouldEnableInputMethod(this));
    QEvent event(QEvent::ReadOnlyChange);
    QApplication::sendEvent(this, &event);
}

/*!
    \property PageTextEdit::textInteractionFlags
    \since 4.2

    Specifies how the widget should interact with user input.

    The default value depends on whether the PageTextEdit is read-only
    or editable, and whether it is a QTextBrowser or not.
*/

void PageTextEdit::setTextInteractionFlags(Qt::TextInteractionFlags flags)
{
    Q_D(PageTextEdit);
    d->control->setTextInteractionFlags(flags);
}

Qt::TextInteractionFlags PageTextEdit::textInteractionFlags() const
{
    Q_D(const PageTextEdit);
    return d->control->textInteractionFlags();
}

/*!
    Merges the properties specified in \a modifier into the current character
    format by calling QTextCursor::mergeCharFormat on the editor's cursor.
    If the editor has a selection then the properties of \a modifier are
    directly applied to the selection.

    \sa QTextCursor::mergeCharFormat()
 */
void PageTextEdit::mergeCurrentCharFormat(const QTextCharFormat &modifier)
{
    Q_D(PageTextEdit);
    d->control->mergeCurrentCharFormat(modifier);
}

/*!
    Sets the char format that is be used when inserting new text to \a
    format by calling QTextCursor::setCharFormat() on the editor's
    cursor.  If the editor has a selection then the char format is
    directly applied to the selection.
 */
void PageTextEdit::setCurrentCharFormat(const QTextCharFormat &format)
{
    Q_D(PageTextEdit);
    d->control->setCurrentCharFormat(format);
}

/*!
    Returns the char format that is used when inserting new text.
 */
QTextCharFormat PageTextEdit::currentCharFormat() const
{
    Q_D(const PageTextEdit);
    return d->control->currentCharFormat();
}

/*!
    \property PageTextEdit::autoFormatting
    \brief the enabled set of auto formatting features

    The value can be any combination of the values in the
    AutoFormattingFlag enum.  The default is AutoNone. Choose
    AutoAll to enable all automatic formatting.

    Currently, the only automatic formatting feature provided is
    AutoBulletList; future versions of Qt may offer more.
*/

PageTextEdit::AutoFormatting PageTextEdit::autoFormatting() const
{
    Q_D(const PageTextEdit);
    return d->autoFormatting;
}

void PageTextEdit::setAutoFormatting(AutoFormatting features)
{
    Q_D(PageTextEdit);
    d->autoFormatting = features;
}

/*!
    Convenience slot that inserts \a text at the current
    cursor position.

    It is equivalent to

    \snippet code/src_gui_widgets_qtextedit.cpp 1
 */
void PageTextEdit::insertPlainText(const QString &text)
{
    Q_D(PageTextEdit);
    d->control->insertPlainText(text);
}

/*!
    Convenience slot that inserts \a text which is assumed to be of
    html formatting at the current cursor position.

    It is equivalent to:

    \snippet code/src_gui_widgets_qtextedit.cpp 2

    \note When using this function with a style sheet, the style sheet will
    only apply to the current block in the document. In order to apply a style
    sheet throughout a document, use QTextDocument::setDefaultStyleSheet()
    instead.
 */
#ifndef QT_NO_TEXTHTMLPARSER
void PageTextEdit::insertHtml(const QString &text)
{
    Q_D(PageTextEdit);
    d->control->insertHtml(text);
}
#endif // QT_NO_TEXTHTMLPARSER

/*!
    Scrolls the text edit so that the anchor with the given \a name is
    visible; does nothing if the \a name is empty, or is already
    visible, or isn't found.
*/
void PageTextEdit::scrollToAnchor(const QString &name)
{
    Q_D(PageTextEdit);
    if (name.isEmpty())
        return;

    if (!isVisible()) {
        d->anchorToScrollToWhenVisible = name;
        return;
    }

    QPointF p = d->control->anchorPosition(name);
    const int newPosition = qRound(p.y());
    if ( d->vbar->maximum() < newPosition )
        d->_q_adjustScrollbars();
    d->vbar->setValue(newPosition);
}

/*!
    \fn PageTextEdit::zoomIn(int range)

    Zooms in on the text by making the base font size \a range
    points larger and recalculating all font sizes to be the new size.
    This does not change the size of any images.

    \sa zoomOut()
*/
void PageTextEdit::zoomIn(int range)
{
    zoomInF(range);
}

/*!
    \fn PageTextEdit::zoomOut(int range)

    \overload

    Zooms out on the text by making the base font size \a range points
    smaller and recalculating all font sizes to be the new size. This
    does not change the size of any images.

    \sa zoomIn()
*/
void PageTextEdit::zoomOut(int range)
{
    zoomInF(-range);
}

/*!
    \internal
*/
void PageTextEdit::zoomInF(float range)
{
    if (range == 0.f)
        return;
    QFont f = font();
    const float newSize = f.pointSizeF() + range;
    if (newSize <= 0)
        return;
    f.setPointSizeF(newSize);
    setFont(f);
}

void PageTextEdit::_q_repaintContents(const QRectF& r)
{
    Q_D(PageTextEdit);
    d->_q_repaintContents(r);
}

void PageTextEdit::_q_currentCharFormatChanged(const QTextCharFormat& cf)
{
    Q_D(PageTextEdit);
    d->_q_currentCharFormatChanged(cf);
}

void PageTextEdit::_q_adjustScrollbars()
{
    Q_D(PageTextEdit);
    d->_q_adjustScrollbars();
}

void PageTextEdit::_q_ensureVisible(const QRectF& rect)
{
    Q_D(PageTextEdit);
    d->_q_ensureVisible(rect);
}

void PageTextEdit::_q_cursorPositionChanged()
{
    Q_D(PageTextEdit);
    d->_q_cursorPositionChanged();
}

/*!
    \since 4.2
    Moves the cursor by performing the given \a operation.

    If \a mode is QTextCursor::KeepAnchor, the cursor selects the text it moves over.
    This is the same effect that the user achieves when they hold down the Shift key
    and move the cursor with the cursor keys.

    \sa QTextCursor::movePosition()
*/
void PageTextEdit::moveCursor(QTextCursor::MoveOperation operation, QTextCursor::MoveMode mode)
{
    Q_D(PageTextEdit);
    d->control->moveCursor(operation, mode);
}

/*!
    \since 4.2
    Returns whether text can be pasted from the clipboard into the textedit.
*/
bool PageTextEdit::canPaste() const
{
    Q_D(const PageTextEdit);
    return d->control->canPaste();
}

/*!
    \since 4.3
    Convenience function to print the text edit's document to the given \a printer. This
    is equivalent to calling the print method on the document directly except that this
    function also supports QPrinter::Selection as print range.

    \sa QTextDocument::print()
*/
#ifndef QT_NO_PRINTER
void PageTextEdit::print(QPagedPaintDevice *printer) const
{
    Q_D(const PageTextEdit);
    d->control->print(printer);
}
#endif

/*! \property PageTextEdit::tabChangesFocus
  \brief whether \uicontrol Tab changes focus or is accepted as input

  In some occasions text edits should not allow the user to input
  tabulators or change indentation using the \uicontrol Tab key, as this breaks
  the focus chain. The default is false.

*/

bool PageTextEdit::tabChangesFocus() const
{
    Q_D(const PageTextEdit);
    return d->tabChangesFocus;
}

void PageTextEdit::setTabChangesFocus(bool b)
{
    Q_D(PageTextEdit);
    d->tabChangesFocus = b;
}

/*!
    \property PageTextEdit::documentTitle
    \brief the title of the document parsed from the text.

    By default, for a newly-created, empty document, this property contains
    an empty string.
*/

/*!
    \property PageTextEdit::lineWrapMode
    \brief the line wrap mode

    The default mode is WidgetWidth which causes words to be
    wrapped at the right edge of the text edit. Wrapping occurs at
    whitespace, keeping whole words intact. If you want wrapping to
    occur within words use setWordWrapMode(). If you set a wrap mode of
    FixedPixelWidth or FixedColumnWidth you should also call
    setLineWrapColumnOrWidth() with the width you want.

    \sa lineWrapColumnOrWidth
*/

PageTextEdit::LineWrapMode PageTextEdit::lineWrapMode() const
{
    Q_D(const PageTextEdit);
    return d->lineWrap;
}

void PageTextEdit::setLineWrapMode(LineWrapMode wrap)
{
    Q_D(PageTextEdit);
    if (d->lineWrap == wrap)
        return;
    d->lineWrap = wrap;
    d->updateDefaultTextOption();
    d->relayoutDocument();
}

/*!
    \property PageTextEdit::lineWrapColumnOrWidth
    \brief the position (in pixels or columns depending on the wrap mode) where text will be wrapped

    If the wrap mode is FixedPixelWidth, the value is the number of
    pixels from the left edge of the text edit at which text should be
    wrapped. If the wrap mode is FixedColumnWidth, the value is the
    column number (in character columns) from the left edge of the
    text edit at which text should be wrapped.

    By default, this property contains a value of 0.

    \sa lineWrapMode
*/

int PageTextEdit::lineWrapColumnOrWidth() const
{
    Q_D(const PageTextEdit);
    return d->lineWrapColumnOrWidth;
}

void PageTextEdit::setLineWrapColumnOrWidth(int w)
{
    Q_D(PageTextEdit);
    d->lineWrapColumnOrWidth = w;
    d->relayoutDocument();
}

/*!
    \property PageTextEdit::wordWrapMode
    \brief the mode PageTextEdit will use when wrapping text by words

    By default, this property is set to QTextOption::WrapAtWordBoundaryOrAnywhere.

    \sa QTextOption::WrapMode
*/

QTextOption::WrapMode PageTextEdit::wordWrapMode() const
{
    Q_D(const PageTextEdit);
    return d->wordWrap;
}

void PageTextEdit::setWordWrapMode(QTextOption::WrapMode mode)
{
    Q_D(PageTextEdit);
    if (mode == d->wordWrap)
        return;
    d->wordWrap = mode;
    d->updateDefaultTextOption();
}

/*!
    Finds the next occurrence of the string, \a exp, using the given
    \a options. Returns \c true if \a exp was found and changes the
    cursor to select the match; otherwise returns \c false.
*/
bool PageTextEdit::find(const QString &exp, QTextDocument::FindFlags options)
{
    Q_D(PageTextEdit);
    return d->control->find(exp, options);
}

/*!
    \fn bool PageTextEdit::find(const QRegExp &exp, QTextDocument::FindFlags options)

    \since 5.3
    \overload

    Finds the next occurrence, matching the regular expression, \a exp, using the given
    \a options. The QTextDocument::FindCaseSensitively option is ignored for this overload,
    use QRegExp::caseSensitivity instead.

    Returns \c true if a match was found and changes the cursor to select the match;
    otherwise returns \c false.
*/
#ifndef QT_NO_REGEXP
bool PageTextEdit::find(const QRegExp &exp, QTextDocument::FindFlags options)
{
    Q_D(PageTextEdit);
    return d->control->find(exp, options);
}
#endif

/*!
    \fn void PageTextEdit::copyAvailable(bool yes)

    This signal is emitted when text is selected or de-selected in the
    text edit.

    When text is selected this signal will be emitted with \a yes set
    to true. If no text has been selected or if the selected text is
    de-selected this signal is emitted with \a yes set to false.

    If \a yes is true then copy() can be used to copy the selection to
    the clipboard. If \a yes is false then copy() does nothing.

    \sa selectionChanged()
*/

/*!
    \fn void PageTextEdit::currentCharFormatChanged(const QTextCharFormat &f)

    This signal is emitted if the current character format has changed, for
    example caused by a change of the cursor position.

    The new format is \a f.

    \sa setCurrentCharFormat()
*/

/*!
    \fn void PageTextEdit::selectionChanged()

    This signal is emitted whenever the selection changes.

    \sa copyAvailable()
*/

/*!
    \fn void PageTextEdit::cursorPositionChanged()

    This signal is emitted whenever the position of the
    cursor changed.
*/

/*!
    \since 4.2

    Sets the text edit's \a text. The text can be plain text or HTML
    and the text edit will try to guess the right format.

    Use setHtml() or setPlainText() directly to avoid text edit's guessing.

    \sa toPlainText(), toHtml()
*/
void PageTextEdit::setText(const QString &text)
{
    Q_D(PageTextEdit);
    Qt::TextFormat format = d->textFormat;
    if (d->textFormat == Qt::AutoText)
        format = Qt::mightBeRichText(text) ? Qt::RichText : Qt::PlainText;
#ifndef QT_NO_TEXTHTMLPARSER
    if (format == Qt::RichText)
        setHtml(text);
    else
#endif
        setPlainText(text);
}


/*!
    Appends a new paragraph with \a text to the end of the text edit.

    \note The new paragraph appended will have the same character format and
    block format as the current paragraph, determined by the position of the cursor.

    \sa currentCharFormat(), QTextCursor::blockFormat()
*/

void PageTextEdit::append(const QString &text)
{
    Q_D(PageTextEdit);
    const bool atBottom = isReadOnly() ?  d->verticalOffset() >= d->vbar->maximum() :
            d->control->textCursor().atEnd();
    d->control->append(text);
    if (atBottom)
        d->vbar->setValue(d->vbar->maximum());
}

/*!
    Ensures that the cursor is visible by scrolling the text edit if
    necessary.
*/
void PageTextEdit::ensureCursorVisible()
{
    Q_D(PageTextEdit);

    //
    // Если курсор в невидимом блоке, откручиваем его назад до тех пор пока он не войдёт в видимый блок
    //
    {
        QTextCursor cursor = textCursor();
        while (!cursor.atStart() && !cursor.block().isVisible()) {
            cursor.movePosition(QTextCursor::PreviousBlock);
            cursor.movePosition(QTextCursor::StartOfBlock);
        }
        if (cursor.position() != textCursor().position()) {
            setTextCursor(cursor);
        }
    }

    d->control->ensureCursorVisible();

    //
    // Если необходимо прокручиваем ещё немного
    //
    {
        const int DETECT_DELTA = d->vbar->singleStep() * 2;
        const int SCROLL_DELTA = d->vbar->singleStep() * 5;
        QRect cursorRect = this->cursorRect();
        if (cursorRect.y() - DETECT_DELTA <= 0) {
            d->vbar->setValue(d->vbar->value() - SCROLL_DELTA);
        }
        else if (cursorRect.height() + cursorRect.y() + DETECT_DELTA >= d->viewport->height()) {
            //
            // Если можем, прокручиваем вниз на SCROLL_DELTA
            //
            if (d->vbar->value() + SCROLL_DELTA <= d->vbar->maximum()) {
                d->vbar->setValue(d->vbar->value() + SCROLL_DELTA);
            }
            //
            // Если SCROLL_DELTA это много, но ещё можно прокрутить - прокручиваем до конца
            //
            else if (d->vbar->maximum() - d->vbar->value() > 0) {
                d->vbar->setValue(d->vbar->maximum());
            }
        }
    }
}

void PageTextEdit::ensureCursorVisible(const QTextCursor& _cursor, bool _animate)
{
    Q_D(PageTextEdit);

    const int lastVbarValue = d->vbar->value();

    setTextCursor(_cursor);
    QPoint top = cursorRect().topLeft();
    int nextVbarValue = top.y() + d->vbar->value();

    //
    // Прокручиваем ещё немного
    //
    const int SCROLL_DELTA = d->vbar->singleStep() * 5;
    nextVbarValue -= SCROLL_DELTA;

    //
    // Если нужно, анимируем
    //
    if (_animate) {
        QPropertyAnimation* animation = new QPropertyAnimation(d->vbar, "value", this);
        qreal delta = log(abs(nextVbarValue - lastVbarValue) / 300) / 2;
        if (delta < 1) {
            delta = 1;
        }
        animation->setDuration(300 * delta);
        animation->setEasingCurve(QEasingCurve::InOutCubic);
        animation->setStartValue(lastVbarValue);
        animation->setEndValue(nextVbarValue);
        animation->start();
    } else {
        d->vbar->setValue(nextVbarValue);
    }
}

/*!
    \fn void PageTextEdit::textChanged()

    This signal is emitted whenever the document's content changes; for
    example, when text is inserted or deleted, or when formatting is applied.
*/

/*!
    \fn void PageTextEdit::undoAvailable(bool available)

    This signal is emitted whenever undo operations become available
    (\a available is true) or unavailable (\a available is false).
*/

/*!
    \fn void PageTextEdit::redoAvailable(bool available)

    This signal is emitted whenever redo operations become available
    (\a available is true) or unavailable (\a available is false).
*/




//
// Реализация дополнений, необходимых для превращения простого QTextEdit в постраничный редактор
//

void PageTextEdit::setPageFormat(QPageSize::PageSizeId _pageFormat)
{
    Q_D(PageTextEdit);
    if (d->m_pageMetrics.pageFormat() != _pageFormat) {
        d->m_pageMetrics.update(_pageFormat);
        d->relayoutDocument();
    }
}

void PageTextEdit::setPageMargins(const QMarginsF& _margins)
{
    Q_D(PageTextEdit);
    if (d->m_pageMetrics.mmPageMargins() != _margins) {
        d->m_pageMetrics.update(d->m_pageMetrics.pageFormat(), _margins);
        d->relayoutDocument();
    }
}

bool PageTextEdit::usePageMode() const
{
    Q_D(const PageTextEdit);
    return d->m_usePageMode;
}

int PageTextEdit::cursorPage(const QTextCursor& _cursor)
{
    Q_D(PageTextEdit);
    return (cursorRect(_cursor).top() / d->m_pageMetrics.pxPageSize().height()) + 1;
}

void PageTextEdit::setUsePageMode(bool _use)
{
    Q_D(PageTextEdit);
    if (d->m_usePageMode != _use) {
        d->m_usePageMode = _use;
        d->relayoutDocument();
    }
}

void PageTextEdit::setAddSpaceToBottom(bool _addSpace)
{
    Q_D(PageTextEdit);
    if (d->m_addBottomSpace != _addSpace) {
        d->m_addBottomSpace = _addSpace;
        d->relayoutDocument();
    }
}

void PageTextEdit::setShowPageNumbers(bool _show)
{
    Q_D(PageTextEdit);
    if (d->m_showPageNumbers != _show) {
        d->m_showPageNumbers = _show;
        d->relayoutDocument();
    }
}

void PageTextEdit::setPageNumbersAlignment(Qt::Alignment _align)
{
    Q_D(PageTextEdit);
    if (d->m_pageNumbersAlignment != _align) {
        d->m_pageNumbersAlignment = _align;
        d->relayoutDocument();
    }
}

void PageTextEdit::setWatermark(const QString& _watermark)
{
    Q_D(PageTextEdit);
    if (d->m_watermark != _watermark) {
        d->m_watermark = _watermark;

        //
        // Размножим водяной знак
        //
        d->m_watermarkMulti.clear();
        if (!d->m_watermark.isEmpty()) {
            for (int reply = 0; reply < 5; ++reply) {
                d->m_watermarkMulti.append(d->m_watermark);
                d->m_watermarkMulti.append("     ");
            }
        }

        d->relayoutDocument();
    }
}

void PageTextEdit::clipPageDecorationRegions(QPainter* _painter)
{
    Q_D(PageTextEdit);
    d->clipPageDecorationRegions(_painter);
}

void PageTextEdit::relayoutDocument()
{
    Q_D(PageTextEdit);
    d->relayoutDocument();
}



#endif // QT_NO_TEXTEDIT

QT_END_NAMESPACE
