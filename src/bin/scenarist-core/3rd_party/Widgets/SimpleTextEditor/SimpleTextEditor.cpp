#include "SimpleTextEditor.h"

#include <QApplication>
#include <QTextBlock>
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

    connect(document(), &QTextDocument::contentsChange, this, &SimpleTextEditor::correctLineSpacing);
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

	//
	// TODO: При изменении шрифта, не удаётся корректно наложить новое форматированние на старое,
	// поэтому приходится делать всё это вручную
	//

	QTextCursor cursor = textCursor();
	if (!cursor.hasSelection()) {
		cursor.select(QTextCursor::BlockUnderCursor);
	}
    cursor.mergeCharFormat(fmt);
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

void SimpleTextEditor::correctLineSpacing(int _from, int _removed, int _added)
{
    Q_UNUSED(_removed);

    QTextCursor cursor(document());
//    cursor.beginEditBlock();
    cursor.setPosition(_from);
    do {
        cursor.select(QTextCursor::BlockUnderCursor);
        QTextBlockFormat blockFormat = cursor.blockFormat();
        const int bottomMargin = QFontMetrics(cursor.charFormat().font()).lineSpacing();
        if (blockFormat.bottomMargin() != bottomMargin) {
            blockFormat.setBottomMargin(bottomMargin);
            cursor.setBlockFormat(blockFormat);
        }
        cursor.movePosition(QTextCursor::NextBlock);
        cursor.movePosition(QTextCursor::EndOfBlock);
    } while (cursor.position() < _from + _added
             && !cursor.atEnd());
//    cursor.endEditBlock();
}
