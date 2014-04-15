#include "SimpleTextEditor.h"

#include <QAction>
#include <QTextCharFormat>
#include <QTextEdit>
#include <QToolBar>
#include <QVBoxLayout>


SimpleTextEditor::SimpleTextEditor(QWidget *parent) :
	QWidget(parent)
{
	setupMenu();

	textEdit = new QTextEdit(this);
	connect(textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
			this, SLOT(currentCharFormatChanged(QTextCharFormat)));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(tb);
	layout->addWidget(textEdit);
	layout->setSpacing(0);
}

void SimpleTextEditor::textBold()
{
	QTextCharFormat fmt;
	fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
	mergeFormatOnWordOrSelection(fmt);
}

void SimpleTextEditor::textUnderline()
{
	QTextCharFormat fmt;
	fmt.setFontUnderline(actionTextUnderline->isChecked());
	mergeFormatOnWordOrSelection(fmt);
}

void SimpleTextEditor::textItalic()
{
	QTextCharFormat fmt;
	fmt.setFontItalic(actionTextItalic->isChecked());
	mergeFormatOnWordOrSelection(fmt);
}

void SimpleTextEditor::currentCharFormatChanged(const QTextCharFormat& format)
{
	QFont formatFont = format.font();
	actionTextBold->setChecked(formatFont.bold());
	actionTextItalic->setChecked(formatFont.italic());
	actionTextUnderline->setChecked(formatFont.underline());
}

void SimpleTextEditor::setupMenu()
{
	tb = new QToolBar(this);
	tb->setWindowTitle(tr("Text Formatting"));

	actionTextBold = new QAction(QIcon::fromTheme("format-text-bold", QIcon("/textbold.png")),
								 tr("&Bold"), this);
	actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
	actionTextBold->setPriority(QAction::LowPriority);
	QFont bold;
	bold.setBold(true);
	actionTextBold->setFont(bold);
	connect(actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
	tb->addAction(actionTextBold);
	actionTextBold->setCheckable(true);

	actionTextItalic = new QAction(QIcon::fromTheme("format-text-italic", QIcon("/textitalic.png")),
								   tr("&Italic"), this);
	actionTextItalic->setPriority(QAction::LowPriority);
	actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
	QFont italic;
	italic.setItalic(true);
	actionTextItalic->setFont(italic);
	connect(actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
	tb->addAction(actionTextItalic);
	actionTextItalic->setCheckable(true);

	actionTextUnderline = new QAction(QIcon::fromTheme("format-text-underline", QIcon("/textunder.png")),
									  tr("&Underline"), this);
	actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
	actionTextUnderline->setPriority(QAction::LowPriority);
	QFont underline;
	underline.setUnderline(true);
	actionTextUnderline->setFont(underline);
	connect(actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
	tb->addAction(actionTextUnderline);
	actionTextUnderline->setCheckable(true);
}

void SimpleTextEditor::mergeFormatOnWordOrSelection(const QTextCharFormat& format)
{
	QTextCursor cursor = textEdit->textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
	textEdit->mergeCurrentCharFormat(format);
}
