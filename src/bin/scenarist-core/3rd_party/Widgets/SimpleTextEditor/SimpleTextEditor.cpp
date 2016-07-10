#include "SimpleTextEditor.h"

#include <QApplication>
#include <QTextCharFormat>
#include <QTextEdit>
#include <QToolBar>
#include <QVBoxLayout>
#include <QMenu>
#include <QMimeData>
#include <QShortcut>

namespace {
	/**
	 * @brief Майм тип данных для своего текста
	 */
	const QString MIME_TYPE = "text/simple.html";
}


SimpleTextEditor::SimpleTextEditor(QWidget *parent) :
	SpellCheckTextEdit(parent)
{
	setAddSpaceToBottom(false);
	setTabChangesFocus(true);
	setUsePageMode(false);
	setPageMargins(QMarginsF(2, 2, 2, 2));
}

void SimpleTextEditor::setTextBold(bool _bold)
{
	QTextCharFormat fmt;
	fmt.setFontWeight(_bold ? QFont::Bold : QFont::Normal);
	mergeFormatOnWordOrSelection(fmt);
}

void SimpleTextEditor::setTextUnderline(bool _underline)
{
	QTextCharFormat fmt;
	fmt.setFontUnderline(_underline);
	mergeFormatOnWordOrSelection(fmt);
}

void SimpleTextEditor::setTextItalic(bool _italic)
{
	QTextCharFormat fmt;
	fmt.setFontItalic(_italic);
	mergeFormatOnWordOrSelection(fmt);
}

void SimpleTextEditor::setTextColor(const QColor& _color)
{
	QTextCharFormat fmt;
	fmt.setForeground(_color);
	mergeFormatOnWordOrSelection(fmt);
}

void SimpleTextEditor::setTextBackgroundColor(const QColor& _color)
{
	QTextCharFormat fmt;
	fmt.setBackground(_color);
	mergeFormatOnWordOrSelection(fmt);
}

void SimpleTextEditor::setTextFont(const QFont& _font)
{
	QTextCharFormat fmt;
	fmt.setFont(_font);
	mergeFormatOnParagraphOrSelection(fmt);
}

QMimeData* SimpleTextEditor::createMimeDataFromSelection() const
{
	QMimeData* mime = PageTextEdit::createMimeDataFromSelection();
	mime->setData(MIME_TYPE, mime->html().toUtf8());
	return mime;
}


void SimpleTextEditor::insertFromMimeData(const QMimeData* _source)
{
	if (_source->hasFormat(MIME_TYPE)) {
		textCursor().insertHtml(_source->data(MIME_TYPE));
	} else if (_source->hasText()) {
		textCursor().insertText(_source->text());
	}
}

void SimpleTextEditor::mergeFormatOnWordOrSelection(const QTextCharFormat& format)
{
	QTextCursor cursor = textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
	mergeCurrentCharFormat(format);
}

void SimpleTextEditor::mergeFormatOnParagraphOrSelection(const QTextCharFormat& format)
{
	QTextCursor cursor = textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::BlockUnderCursor);
	cursor.mergeCharFormat(format);
	mergeCurrentCharFormat(format);
}
