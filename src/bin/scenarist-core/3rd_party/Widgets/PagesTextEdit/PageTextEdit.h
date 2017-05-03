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

#ifndef PAGETEXTEDIT_H
#define PAGETEXTEDIT_H

#include <QtWidgets/qabstractscrollarea.h>
#include <QtGui/qtextdocument.h>
#include <QtGui/qtextoption.h>
#include <QtGui/qtextcursor.h>
#include <QtGui/qtextformat.h>
#include <QtGui/qpagesize.h>

#ifndef QT_NO_TEXTEDIT


QT_BEGIN_NAMESPACE


class QStyleSheet;
class QTextDocument;
class QMenu;
class PageTextEditPrivate;
class QMimeData;
class QPagedPaintDevice;

class PageTextEdit : public QAbstractScrollArea
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(PageTextEdit)
	Q_FLAGS(AutoFormatting)
	Q_PROPERTY(AutoFormatting autoFormatting READ autoFormatting WRITE setAutoFormatting)
	Q_PROPERTY(bool tabChangesFocus READ tabChangesFocus WRITE setTabChangesFocus)
	Q_PROPERTY(QString documentTitle READ documentTitle WRITE setDocumentTitle)
	Q_PROPERTY(bool undoRedoEnabled READ isUndoRedoEnabled WRITE setUndoRedoEnabled)
	Q_PROPERTY(LineWrapMode lineWrapMode READ lineWrapMode WRITE setLineWrapMode)
	QDOC_PROPERTY(QTextOption::WrapMode wordWrapMode READ wordWrapMode WRITE setWordWrapMode)
	Q_PROPERTY(int lineWrapColumnOrWidth READ lineWrapColumnOrWidth WRITE setLineWrapColumnOrWidth)
	Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
#ifndef QT_NO_TEXTHTMLPARSER
	Q_PROPERTY(QString html READ toHtml WRITE setHtml NOTIFY textChanged USER true)
#endif
	Q_PROPERTY(QString plainText READ toPlainText WRITE setPlainText DESIGNABLE false)
	Q_PROPERTY(bool overwriteMode READ overwriteMode WRITE setOverwriteMode)
	Q_PROPERTY(int tabStopWidth READ tabStopWidth WRITE setTabStopWidth)
	Q_PROPERTY(bool acceptRichText READ acceptRichText WRITE setAcceptRichText)
	Q_PROPERTY(int cursorWidth READ cursorWidth WRITE setCursorWidth)
	Q_PROPERTY(Qt::TextInteractionFlags textInteractionFlags READ textInteractionFlags WRITE setTextInteractionFlags)
	Q_PROPERTY(QTextDocument *document READ document WRITE setDocument DESIGNABLE false)
	Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
public:
	enum LineWrapMode {
		NoWrap,
		WidgetWidth,
		FixedPixelWidth,
		FixedColumnWidth
	};
	Q_ENUM(LineWrapMode)

	enum AutoFormattingFlag {
		AutoNone = 0,
		AutoBulletList = 0x00000001,
		AutoAll = 0xffffffff
	};

	Q_DECLARE_FLAGS(AutoFormatting, AutoFormattingFlag)

	explicit PageTextEdit(QWidget *parent = 0);
	explicit PageTextEdit(const QString &text, QWidget *parent = 0);
	virtual ~PageTextEdit();

	void setDocument(QTextDocument *document);
	QTextDocument *document() const;

	void setPlaceholderText(const QString &placeholderText);
	QString placeholderText() const;

	void setTextCursor(const QTextCursor &cursor);
	QTextCursor textCursor() const;

	bool isReadOnly() const;
	void setReadOnly(bool ro);

	void setTextInteractionFlags(Qt::TextInteractionFlags flags);
	Qt::TextInteractionFlags textInteractionFlags() const;

	qreal fontPointSize() const;
	QString fontFamily() const;
	int fontWeight() const;
	bool fontUnderline() const;
	bool fontItalic() const;
	QColor textColor() const;
	QColor textBackgroundColor() const;
	QFont currentFont() const;
	Qt::Alignment alignment() const;

	void mergeCurrentCharFormat(const QTextCharFormat &modifier);

	void setCurrentCharFormat(const QTextCharFormat &format);
	QTextCharFormat currentCharFormat() const;

	AutoFormatting autoFormatting() const;
	void setAutoFormatting(AutoFormatting features);

	bool tabChangesFocus() const;
	void setTabChangesFocus(bool b);

	inline void setDocumentTitle(const QString &title)
	{ document()->setMetaInformation(QTextDocument::DocumentTitle, title); }
	inline QString documentTitle() const
	{ return document()->metaInformation(QTextDocument::DocumentTitle); }

	inline bool isUndoRedoEnabled() const
	{ return document()->isUndoRedoEnabled(); }
	inline void setUndoRedoEnabled(bool enable)
	{ document()->setUndoRedoEnabled(enable); }

	LineWrapMode lineWrapMode() const;
	void setLineWrapMode(LineWrapMode mode);

	int lineWrapColumnOrWidth() const;
	void setLineWrapColumnOrWidth(int w);

	QTextOption::WrapMode wordWrapMode() const;
	void setWordWrapMode(QTextOption::WrapMode policy);

	bool find(const QString &exp, QTextDocument::FindFlags options = 0);
#ifndef QT_NO_REGEXP
	bool find(const QRegExp &exp, QTextDocument::FindFlags options = 0);
#endif

	QString toPlainText() const;
#ifndef QT_NO_TEXTHTMLPARSER
	QString toHtml() const;
#endif

	/**
	 * @brief Своя реализация проверки виден ли курсор на экране
	 * @param Значение true докручивает, как сверху, так и снизу, а false только снизу
	 */
	void ensureCursorVisible();
	void ensureCursorVisible(const QTextCursor& _cursor, bool _animate = true);

	Q_INVOKABLE virtual QVariant loadResource(int type, const QUrl &name);
#ifndef QT_NO_CONTEXTMENU
	QMenu *createStandardContextMenu(QWidget* _parent = 0);
	QMenu *createStandardContextMenu(const QPoint &position, QWidget* _parent = 0);
#endif

	QTextCursor cursorForPosition(const QPoint &pos) const;
	QRect cursorRect(const QTextCursor &cursor) const;
	QRect cursorRect() const;

	QString anchorAt(const QPoint& pos) const;

	bool overwriteMode() const;
	void setOverwriteMode(bool overwrite);

	int tabStopWidth() const;
	void setTabStopWidth(int width);

	int cursorWidth() const;
	void setCursorWidth(int width);

	bool acceptRichText() const;
	void setAcceptRichText(bool accept);

	struct ExtraSelection
	{
		QTextCursor cursor;
		QTextCharFormat format;
	};
//	void setExtraSelections(const QList<ExtraSelection> &selections);
//	QList<ExtraSelection> extraSelections() const;

	void moveCursor(QTextCursor::MoveOperation operation, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);

	bool canPaste() const;

	void print(QPagedPaintDevice *printer) const;

	QVariant inputMethodQuery(Qt::InputMethodQuery property) const Q_DECL_OVERRIDE;
	Q_INVOKABLE QVariant inputMethodQuery(Qt::InputMethodQuery query, QVariant argument) const;

public Q_SLOTS:
	void setFontPointSize(qreal s);
	void setFontFamily(const QString &fontFamily);
	void setFontWeight(int w);
	void setFontUnderline(bool b);
	void setFontItalic(bool b);
	void setTextColor(const QColor &c);
	void setTextBackgroundColor(const QColor &c);
	void setCurrentFont(const QFont &f);
	void setAlignment(Qt::Alignment a);

	void setPlainText(const QString &text);
#ifndef QT_NO_TEXTHTMLPARSER
	void setHtml(const QString &text);
#endif
	void setText(const QString &text);

#ifndef QT_NO_CLIPBOARD
	void cut();
	void copy();
	void paste();
#endif

	void undo();
	void redo();

	void clear();
	void selectAll();

	void insertPlainText(const QString &text);
#ifndef QT_NO_TEXTHTMLPARSER
	void insertHtml(const QString &text);
#endif // QT_NO_TEXTHTMLPARSER

	void append(const QString &text);

	void scrollToAnchor(const QString &name);

	void zoomIn(int range = 1);
	void zoomOut(int range = 1);

Q_SIGNALS:
	void textChanged();
	void undoAvailable(bool b);
	void redoAvailable(bool b);
	void currentCharFormatChanged(const QTextCharFormat &format);
	void copyAvailable(bool b);
	void selectionChanged();
	void cursorPositionChanged();

protected:
	virtual bool event(QEvent *e) Q_DECL_OVERRIDE;
	virtual void timerEvent(QTimerEvent *e) Q_DECL_OVERRIDE;
	virtual void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
	virtual void keyReleaseEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
	virtual void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
	virtual void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
	virtual void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	virtual void mouseDoubleClickEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	virtual bool focusNextPrevChild(bool next) Q_DECL_OVERRIDE;
#ifndef QT_NO_CONTEXTMENU
	virtual void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;
#endif
#ifndef QT_NO_DRAGANDDROP
	virtual void dragEnterEvent(QDragEnterEvent *e) Q_DECL_OVERRIDE;
	virtual void dragLeaveEvent(QDragLeaveEvent *e) Q_DECL_OVERRIDE;
	virtual void dragMoveEvent(QDragMoveEvent *e) Q_DECL_OVERRIDE;
	virtual void dropEvent(QDropEvent *e) Q_DECL_OVERRIDE;
#endif
	virtual void focusInEvent(QFocusEvent *e) Q_DECL_OVERRIDE;
	virtual void focusOutEvent(QFocusEvent *e) Q_DECL_OVERRIDE;
	virtual void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
	virtual void changeEvent(QEvent *e) Q_DECL_OVERRIDE;
#ifndef QT_NO_WHEELEVENT
	virtual void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;
#endif

	virtual QMimeData *createMimeDataFromSelection() const;
	virtual bool canInsertFromMimeData(const QMimeData *source) const;
	virtual void insertFromMimeData(const QMimeData *source);

	virtual void inputMethodEvent(QInputMethodEvent *) Q_DECL_OVERRIDE;

	PageTextEdit(PageTextEditPrivate &dd, QWidget *parent);

	virtual void scrollContentsBy(int dx, int dy) Q_DECL_OVERRIDE;
	virtual void doSetTextCursor(const QTextCursor &cursor);

	void zoomInF(float range);

private:
	Q_DISABLE_COPY(PageTextEdit)
	Q_SLOT void _q_repaintContents(const QRectF &r);
	Q_SLOT void _q_currentCharFormatChanged(const QTextCharFormat &cf);
	Q_SLOT void _q_adjustScrollbars();
	Q_SLOT void _q_ensureVisible(const QRectF &);
	Q_SLOT void _q_cursorPositionChanged();
	friend class PageTextEditControl;
	friend class QTextDocument;
	friend class QWidgetTextControl;

	//
	// Дополнения, необходимые для того, чтобы превратить простой QTextEdit в постраничный редактор
	//

public:
	/**
	 * @brief Установить формат страницы
	 */
	void setPageFormat(QPageSize::PageSizeId _pageFormat);

	/**
	 * @brief Настроить поля страницы
	 */
	void setPageMargins(const QMarginsF& _margins);

	/**
	 * @brief Получить режим отображения текста
	 */
	bool usePageMode() const;

	/**
	 * @brief Получить номер страницы курсора
	 */
	int cursorPage(const QTextCursor& _cursor);

public Q_SLOTS:
	/**
	 * @brief Установить режим отображения текста
	 */
	void setUsePageMode(bool _use);

	/**
	 * @brief Установить значение необходимости добавления дополнительной прокрутки снизу
	 */
	void setAddSpaceToBottom(bool _addSpace);

	/**
	 * @brief Установить значение необходимости отображения номеров страниц
	 */
	void setShowPageNumbers(bool _show);

	/**
	 * @brief Установить место отображения номеров страниц
	 */
	void setPageNumbersAlignment(Qt::Alignment _align);

	/**
	 * @brief Установить водяной знак
	 */
	void setWatermark(const QString& _watermark);

    /**
     * @brief Установить область обрезки так, чтобы вырезалось всё, что выходит на поля страницы
     */
    void clipPageDecorationRegions(QPainter* _painter);

	/**
	 * @brief Перестроить документ
	 */
	void relayoutDocument();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PageTextEdit::AutoFormatting)

QT_END_NAMESPACE

#endif // QT_NO_TEXTEDIT

#endif // PAGETEXTEDIT_H
