#include "SimpleTextEditor.h"

#include <QAction>
#include <QApplication>
#include <QTextCharFormat>
#include <QTextEdit>
#include <QToolBar>
#include <QVBoxLayout>
#include <QMenu>
#include <QContextMenuEvent>
#include <QSettings>


SimpleTextEditor::SimpleTextEditor(QWidget *parent) :
	QTextEdit(parent),
	m_zoomRange(0)
{
	setTabChangesFocus(true);

	setupMenu();

	connect(this, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
			this, SLOT(currentCharFormatChanged(QTextCharFormat)));

	//
	// Подготовить редактор к синхронизации
	//
	s_editors.append(this);
	//
	// Обновить масштаб
	//
	QSettings settings;
	setZoomRange(settings.value("simple-editor/zoom-range", 0).toInt());
}

void SimpleTextEditor::setZoomRange(int _zoomRange)
{
	if (m_zoomRange != _zoomRange) {
		//
		// Отменить предыдущее масштабирование
		//
		zoomOut(m_zoomRange);

		//
		// Применить новое масштабирование
		//
		m_zoomRange = _zoomRange;
		zoomIn(m_zoomRange);

		//
		// Для каждого редактора применить коэффициент
		//
		foreach (SimpleTextEditor* editor, s_editors) {
			editor->setZoomRange(m_zoomRange);
		}

		//
		// Сохранить значение масштаба
		//
		QSettings settings;
		settings.setValue("simple-editor/zoom-range", m_zoomRange);
	}
}

void SimpleTextEditor::contextMenuEvent(QContextMenuEvent* _event)
{
	//
	// Сформируем  контекстное меню
	//
	QMenu* menu = createStandardContextMenu();

	//
	// Добавим действия настройки стиля
	//
	QAction* actionInsertBefore = 0;
	if (menu->actions().count() > 0) {
		actionInsertBefore = menu->actions().first();
	}
	menu->insertAction(actionInsertBefore, actionTextBold);
	menu->insertAction(actionInsertBefore, actionTextItalic);
	menu->insertAction(actionInsertBefore, actionTextUnderline);
	menu->insertSeparator(actionInsertBefore);

	//
	// Покажем меню, а после очистим от него память
	//
	menu->exec(_event->globalPos());
	delete menu;
}

void SimpleTextEditor::wheelEvent(QWheelEvent* _event)
{
	if (_event->modifiers() & Qt::ControlModifier) {
		if (_event->orientation() == Qt::Vertical) {
			//
			// zoomRange > 0 - Текст увеличивается
			// zoomRange < 0 - Текст уменьшается
			//
			int zoomRange = m_zoomRange + (_event->angleDelta().y() / 120);
			setZoomRange(zoomRange);

			_event->accept();
		}
	} else {
		QTextEdit::wheelEvent(_event);
	}
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
	actionTextBold->setShortcutContext(Qt::WidgetShortcut);
	actionTextBold->setPriority(QAction::LowPriority);
	QFont bold;
	bold.setBold(true);
	actionTextBold->setFont(bold);
	connect(actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
	actionTextBold->setCheckable(true);

	actionTextItalic = new QAction(tr("Italic"), this);
	actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
	actionTextItalic->setShortcutContext(Qt::WidgetShortcut);
	actionTextItalic->setPriority(QAction::LowPriority);
	QFont italic;
	italic.setItalic(true);
	actionTextItalic->setFont(italic);
	connect(actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
	actionTextItalic->setCheckable(true);

	actionTextUnderline = new QAction(tr("Underline"), this);
	actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
	actionTextUnderline->setShortcutContext(Qt::WidgetShortcut);
	actionTextUnderline->setPriority(QAction::LowPriority);
	QFont underline;
	underline.setUnderline(true);
	actionTextUnderline->setFont(underline);
	connect(actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
	actionTextUnderline->setCheckable(true);

	//
	// Добавим действия виджету
	//
	addAction(actionTextBold);
	addAction(actionTextItalic);
	addAction(actionTextUnderline);
}

void SimpleTextEditor::mergeFormatOnWordOrSelection(const QTextCharFormat& format)
{
	QTextCursor cursor = textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
	mergeCurrentCharFormat(format);
}

QList<SimpleTextEditor*> SimpleTextEditor::s_editors;
