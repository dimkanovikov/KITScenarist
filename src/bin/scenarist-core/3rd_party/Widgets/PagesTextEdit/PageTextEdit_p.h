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

#ifndef PAGETEXTEDIT_P_H
#define PAGETEXTEDIT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qabstractscrollarea_p.h"
#include "QtGui/qtextdocumentfragment.h"
#include "QtWidgets/qscrollbar.h"
#include "QtGui/qtextcursor.h"
#include "QtGui/qtextformat.h"
#include "QtWidgets/qmenu.h"
#include "QtGui/qabstracttextdocumentlayout.h"
#include "QtCore/qbasictimer.h"
#include "QtCore/qurl.h"
#include "private/qwidgettextcontrol_p.h"

#include "PageTextEdit.h"
#include "PageMetrics.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_TEXTEDIT

class QMimeData;
class PageTextEditPrivate : public QAbstractScrollAreaPrivate
{
	Q_DECLARE_PUBLIC(PageTextEdit)
public:
	PageTextEditPrivate();

	void init(const QString &html = QString());
	void paint(QPainter *p, QPaintEvent *e);
    void _q_repaintContents(const QRectF &contentsRect);

	inline QPoint mapToContents(const QPoint &point) const
	{ return QPoint(point.x() + horizontalOffset(), point.y() + verticalOffset()); }

	void _q_adjustScrollbars();
	void _q_ensureVisible(const QRectF &rect);
	void relayoutDocument();

	void createAutoBulletList();
	void pageUpDown(QTextCursor::MoveOperation op, QTextCursor::MoveMode moveMode);

	inline int horizontalOffset() const
	{ return q_func()->isRightToLeft() ? (hbar->maximum() - hbar->value()) : hbar->value(); }
	inline int verticalOffset() const
	{ return vbar->value(); }

	inline void sendControlEvent(QEvent *e)
	{ control->processEvent(e, QPointF(horizontalOffset(), verticalOffset()), viewport); }

	void _q_currentCharFormatChanged(const QTextCharFormat &format);
	void _q_cursorPositionChanged();

	void updateDefaultTextOption();

	// re-implemented by QTextBrowser, called by QTextDocument::loadResource
	virtual QUrl resolveUrl(const QUrl &url) const
	{ return url; }

	QWidgetTextControl *control;

	PageTextEdit::AutoFormatting autoFormatting;
	bool tabChangesFocus;

	QBasicTimer autoScrollTimer;
	QPoint autoScrollDragPos;

	PageTextEdit::LineWrapMode lineWrap;
	int lineWrapColumnOrWidth;
	QTextOption::WrapMode wordWrap;

	uint ignoreAutomaticScrollbarAdjustment : 1;
	uint preferRichText : 1;
	uint showCursorOnInitialShow : 1;
	uint inDrag : 1;
	uint clickCausedFocus : 1;

	// Qt3 COMPAT only, for setText
	Qt::TextFormat textFormat;

	QString anchorToScrollToWhenVisible;

	QString placeholderText;

#ifdef QT_KEYPAD_NAVIGATION
	QBasicTimer deleteAllTimer;
#endif

	//
	// Дополнения, необходимые для того, чтобы превратить простой QTextEdit в постраничный редактор
	//

	/**
	 * @brief Обновить отступы вьювпорта
	 */
	void updateViewportMargins();

	/**
	 * @brief Обновить геометрию документа
	 * @note Стандартная реализация QTextEdit такова, что она всё время сбрасывает установленный
	 *		 размер документа, что приводит к нежелательным последствиям
	 */
	void updateDocumentGeometry();

	/**
	 * @brief Нарисовать оформление страниц документа
	 */
	void paintPagesView(QPainter* _painter);

	/**
	 * @brief Нарисовать номера страниц
	 */
	void paintPageNumbers(QPainter* _painter);

	/**
	 * @brief Нарисовать номер страницы с заданными парамтерами расположения
	 */
	void paintPageNumber(QPainter* _painter, const QRectF& _rect, bool _isHeader, int _number);

	/**
	 * @brief Нарисовать водяной знак
	 */
	void paintWatermark(QPainter* _painter);

    /**
     * @brief Установить область обрезки так, чтобы вырезалось всё, что выходит на поля страницы
     */
    void clipPageDecorationRegions(QPainter* _painter);

	/**
	 * @brief Режим отображения текста
	 *
	 * true - постраничный
	 * false - сплошной
	 */
	bool m_usePageMode;

	/**
	 * @brief Необходимо ли добавлять пространство снизу в обычном режиме
	 */
	bool m_addBottomSpace;

	/**
	 * @brief Необходимо ли показывать номера страниц
	 */
	bool m_showPageNumbers;

	/**
	 * @brief Где показывать номера страниц
	 */
	Qt::Alignment m_pageNumbersAlignment;

	/**
	 * @brief Метрика страницы редактора
	 */
	PageMetrics m_pageMetrics;

	/**
	 * @brief Водяной знак
	 */
	QString m_watermark;
	QString m_watermarkMulti;

//
// Дополнения для корректной работы с мышью при наличии невидимых текстовых блоков в документе
//
public:
	/**
	 * @brief Отправить скорректированное событие о взаимодействии мышью
	 */
	/** @{ */
	void sendControlMouseEvent(QMouseEvent *e);
	void sendControlContextMenuEvent(QContextMenuEvent *e);
	/** @} */

private:
	/**
	 * @brief Скорректировать позицию мыши
	 * @note Используется в событиях связанных с мышью из-за того, что при наличии невидимых блоков
	 *		 в документе, стандартная реализация иногда скачет сильно вниз
	 */
	QPoint correctMousePosition(const QPoint& _eventPos);
};
#endif // QT_NO_TEXTEDIT


QT_END_NAMESPACE

#endif // PAGETEXTEDIT_P_H
