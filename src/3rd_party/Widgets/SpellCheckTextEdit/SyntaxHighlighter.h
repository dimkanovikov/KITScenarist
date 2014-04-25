/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_SYNTAXHIGHLIGHTER

#include <QtCore/qobject.h>
#include <QtGui/qtextobject.h>
#include <QTextDocument>
#include <QTextCursor>
#include <QPointer>

QT_BEGIN_NAMESPACE


class QTextDocument;
class SyntaxHighlighterPrivate;
class QTextCharFormat;
class QFont;
class QColor;
class QTextBlockUserData;

class Q_GUI_EXPORT SyntaxHighlighter : public QObject
{
	Q_OBJECT

	friend class SyntaxHighlighterPrivate;

public:
	explicit SyntaxHighlighter(QObject *parent);
	explicit SyntaxHighlighter(QTextDocument *parent);
	virtual ~SyntaxHighlighter();

	void setDocument(QTextDocument *doc);
	QTextDocument *document() const;

public Q_SLOTS:
	void rehighlight();
	void rehighlightBlock(const QTextBlock &block);

protected:
	virtual void highlightBlock(const QString &text) = 0;

	void setFormat(int start, int count, const QTextCharFormat &format);
	void setFormat(int start, int count, const QColor &color);
	void setFormat(int start, int count, const QFont &font);
	QTextCharFormat format(int pos) const;

	int previousBlockState() const;
	int currentBlockState() const;
	void setCurrentBlockState(int newState);

	void setCurrentBlockUserData(QTextBlockUserData *data);
	QTextBlockUserData *currentBlockUserData() const;

	QTextBlock currentBlock() const;

private:
	Q_DISABLE_COPY(SyntaxHighlighter)
//	Q_PRIVATE_SLOT(d, void _q_reformatBlocks(int from, int charsRemoved, int charsAdded))
//	Q_PRIVATE_SLOT(d, void _q_delayedRehighlight())

	SyntaxHighlighterPrivate* d;
};


class SyntaxHighlighterPrivate : public QObject
{
	Q_OBJECT

	friend class SyntaxHighlighter;

public:
	inline SyntaxHighlighterPrivate(QObject* _parent, SyntaxHighlighter* _q)
		: QObject(_parent), q(_q), rehighlightPending(false), inReformatBlocks(false)
	{}

	SyntaxHighlighter* q;

	QPointer<QTextDocument> doc;
	void reformatBlocks(int from, int charsRemoved, int charsAdded);
	void reformatBlock(const QTextBlock &block);

	inline void rehighlight(QTextCursor &cursor, QTextCursor::MoveOperation operation) {
		inReformatBlocks = true;
		cursor.beginEditBlock();
		int from = cursor.position();
		cursor.movePosition(operation);
		reformatBlocks(from, 0, cursor.position() - from);
		cursor.endEditBlock();
		inReformatBlocks = false;
	}

	void applyFormatChanges();
	QVector<QTextCharFormat> formatChanges;
	QTextBlock currentBlock;
	bool rehighlightPending;
	bool inReformatBlocks;

public slots:
	void _q_reformatBlocks(int from, int charsRemoved, int charsAdded);

	inline void _q_delayedRehighlight() {
		if (!rehighlightPending)
			return;
		rehighlightPending = false;
		q->rehighlight();
	}
};

QT_END_NAMESPACE

#endif // QT_NO_SYNTAXHIGHLIGHTER

#endif // SYNTAXHIGHLIGHTER_H
