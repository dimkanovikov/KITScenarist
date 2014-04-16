#include "SimpleTextEditor.h"

#include <QAction>
#include <QTextCharFormat>
#include <QTextEdit>
#include <QToolBar>
#include <QVBoxLayout>
#include <QMenu>
#include <QContextMenuEvent>


SimpleTextEditor::SimpleTextEditor(QWidget *parent) :
	QTextEdit(parent)
{
	setupMenu();

	connect(this, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
			this, SLOT(currentCharFormatChanged(QTextCharFormat)));
}

void SimpleTextEditor::contextMenuEvent(QContextMenuEvent* _event)
{
	//
	// Сформируем стандартное контекстное меню
	//
	QMenu* menu = new QMenu(this);

	//
	// Добавим действия настройки стиля
	//
	menu->insertAction(0, actionTextBold);
	menu->insertAction(0, actionTextItalic);
	menu->insertAction(0, actionTextUnderline);

	//
	// Покажем меню, а после очистим от него память
	//
	menu->exec(_event->globalPos());
	delete menu;
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
	actionTextBold = new QAction(tr("Bold"), this);
	actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
	actionTextBold->setPriority(QAction::LowPriority);
	QFont bold;
	bold.setBold(true);
	actionTextBold->setFont(bold);
	connect(actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
	actionTextBold->setCheckable(true);

	actionTextItalic = new QAction(tr("Italic"), this);
	actionTextItalic->setPriority(QAction::LowPriority);
	actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
	QFont italic;
	italic.setItalic(true);
	actionTextItalic->setFont(italic);
	connect(actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
	actionTextItalic->setCheckable(true);

	actionTextUnderline = new QAction(tr("Underline"), this);
	actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
	actionTextUnderline->setPriority(QAction::LowPriority);
	QFont underline;
	underline.setUnderline(true);
	actionTextUnderline->setFont(underline);
	connect(actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
	actionTextUnderline->setCheckable(true);
}

void SimpleTextEditor::mergeFormatOnWordOrSelection(const QTextCharFormat& format)
{
	QTextCursor cursor = textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
	mergeCurrentCharFormat(format);
}
