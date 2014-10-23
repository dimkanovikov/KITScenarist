#include "ScenarioTextEditWidget.h"

#include "ScenarioTextEdit.h"
#include "ScenarioTextEditHelpers.h"
#include "ScenarioFastFormatWidget.h"

#include <3rd_party/Widgets/SearchWidget/SearchWidget.h>

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <QApplication>
#include <QComboBox>
#include <QCryptographicHash>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QToolButton>
#include <QTextBlock>
#include <QTreeView>
#include <QHeaderView>
#include <QStandardItemModel>

using UserInterface::ScenarioTextEditWidget;
using UserInterface::ScenarioTextEdit;
using BusinessLogic::ScenarioStyleFacade;
using BusinessLogic::ScenarioStyle;
using BusinessLogic::ScenarioBlockStyle;

namespace {
	const int SCROLL_DELTA = 140;


	/**
	 * @brief Получить хэш текста
	 */
	static QByteArray textMd5Hash(const QString& _text) {
		QCryptographicHash hash(QCryptographicHash::Md5);
		hash.addData(_text.toUtf8());
		return hash.result();
	}

	/**
	 * @brief Сформировать платформозависимый шорткат
	 */
	static QString makeShortcut(const QString& _shortcut) {
		return QKeySequence(_shortcut).toString(QKeySequence::NativeText);
	}

	/**
	 * @brief Сформиовать платформозависимую подсказку
	 */
	static QString makeToolTip(const QString& _text, const QString& _shortcut) {
		return QString("%1 (%2)").arg(_text).arg(makeShortcut(_shortcut));
	}
}


ScenarioTextEditWidget::ScenarioTextEditWidget(QWidget* _parent) :
	QFrame(_parent),
	m_toolbar(new QWidget(this)),
	m_textStyles(new QComboBox(this)),
	m_undo(new QToolButton(this)),
	m_redo(new QToolButton(this)),
	m_search(new QToolButton(this)),
	m_fastFormat(new QToolButton(this)),
	m_duration(new QLabel(this)),
	m_countersInfo(new QLabel(this)),
	m_editor(new ScenarioTextEdit(this)),
	m_searchLine(new SearchWidget(this)),
	m_fastFormatWidget(new ScenarioFastFormatWidget(this))
{
	initView();
	initConnections();
	initStyleSheet();
}

QWidget* ScenarioTextEditWidget::toolbar() const
{
	return m_toolbar;
}

void ScenarioTextEditWidget::setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document)
{
	removeEditorConnections();

	m_editor->setScenarioDocument(_document);
	if (_document != 0) {
		m_lastTextMd5Hash = textMd5Hash(_document->toPlainText());
	}

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

void ScenarioTextEditWidget::setUsePageView(bool _use)
{
	QMarginsF pageMargins(5, 5, 5, 5);
	if (_use) {
		pageMargins = ScenarioStyleFacade::style().pageMargins();
	}

	m_editor->setPageMargins(pageMargins);
	m_editor->setUsePageMode(_use);
}

void ScenarioTextEditWidget::setUseSpellChecker(bool _use)
{
	m_editor->setUseSpellChecker(_use);
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

void ScenarioTextEditWidget::setTextEditZoomRange(int _zoomRange)
{
	m_editor->setZoomRange(_zoomRange);
}

int ScenarioTextEditWidget::cursorPosition() const
{
	return m_editor->textCursor().position();
}

void ScenarioTextEditWidget::setCursorPosition(int _position)
{
	//
	// Устанавливаем позицию курсора
	//
	QTextCursor cursor = m_editor->textCursor();

	//
	// Если это новая позиция
	//
	if (cursor.position() != _position) {
		//
		// Прокручиваем вниз, чтобы курсор стоял в верху экрана
		//
		cursor.movePosition(QTextCursor::End);
		m_editor->verticalScrollBar()->setValue(m_editor->verticalScrollBar()->maximum());

		//
		// Устанавливаем реальную позицию
		//
		cursor.setPosition(_position);
		m_editor->setTextCursor(cursor);
		m_editor->ensureCursorVisibleReimpl();
		m_editor->setFocus();
	}
	//
	// Если нужно обновить в текущей позиции курсора просто имитируем отправку сигнала
	//
	else {
		emit m_editor->cursorPositionChanged();
	}
}

void ScenarioTextEditWidget::addItem(int _position, const QString& _text, int _type)
{
	QTextCursor cursor = m_editor->textCursor();
	cursor.beginEditBlock();

	cursor.setPosition(_position);
	m_editor->setTextCursor(cursor);
	ScenarioBlockStyle::Type type = (ScenarioBlockStyle::Type)_type;
	//
	// Если в позиции пустой блок, изменим его
	//
	if (cursor.block().text().isEmpty()) {
		m_editor->changeScenarioBlockType(type);
	}
	//
	// В противном случае добавим новый
	//
	else {
		m_editor->addScenarioBlock(type);
	}

	//
	// Устанавливаем текст в блок
	//
	m_editor->insertPlainText(_text);
	//
	// Если это группирующий блок, то вставим и закрывающий текст
	//
	if (ScenarioStyleFacade::style().blockStyle(type).isEmbeddableHeader()) {
		cursor = m_editor->textCursor();
		cursor.movePosition(QTextCursor::NextBlock);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		cursor.insertText(Helpers::footerText(_text));
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
		//
		// Стирать необходимо через имитацию удаления редактором,
		// для корректного обновления модели сцен
		//
		QKeyEvent* event = 0;
		if (cursor.atStart()) {
			event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier);
		} else {
			event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
		}
		QApplication::sendEvent(m_editor, event);
	}

	cursor.endEditBlock();
}

void ScenarioTextEditWidget::updateStylesElements()
{
	//
	// Обновить выпадающий список стилей сценария
	//
	m_textStyles->clear();
	initStylesCombo();

	//
	// Обновить виджет быстрого форматирования
	//
	m_fastFormatWidget->reinitBlockStyles();
}

void ScenarioTextEditWidget::aboutUndo()
{
	m_editor->undo();
}

void ScenarioTextEditWidget::aboutRedo()
{
	m_editor->redo();
}

void ScenarioTextEditWidget::aboutShowSearch()
{
	m_searchLine->setVisible(m_search->isChecked());
	if (m_searchLine->isVisible()) {
		m_searchLine->setFocus();
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

void ScenarioTextEditWidget::aboutUpdateTextStyle()
{
	ScenarioBlockStyle::Type currentType = m_editor->scenarioBlockType();
	if (currentType == ScenarioBlockStyle::TitleHeader) {
		currentType = ScenarioBlockStyle::Title;
	} else if (currentType == ScenarioBlockStyle::SceneGroupFooter) {
		currentType = ScenarioBlockStyle::SceneGroupHeader;
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
	m_editor->setFocus();
}

void ScenarioTextEditWidget::aboutCursorPositionChanged()
{
	emit cursorPositionChanged(m_editor->textCursor().position());
}

void ScenarioTextEditWidget::aboutTextChanged()
{
	QByteArray currentTextMd5Hash = textMd5Hash(m_editor->document()->toPlainText());
	if (m_lastTextMd5Hash != currentTextMd5Hash) {
		emit textChanged();

		m_lastTextMd5Hash = currentTextMd5Hash;
	}
}

void ScenarioTextEditWidget::aboutStyleChanged()
{
	emit textChanged();
}

void ScenarioTextEditWidget::initView()
{
	m_textStyles->setToolTip(tr("Current Text Block Style"));
	m_textStyles->setSizePolicy(m_textStyles->sizePolicy().horizontalPolicy(), QSizePolicy::Preferred);
	m_textStyles->setMaximumHeight(26);

	initStylesCombo();

	m_undo->setIcon(QIcon(":/Graphics/Icons/Editing/undo.png"));
	m_undo->setToolTip(::makeToolTip(tr("Undo last action"), "Ctrl+Z"));

	m_redo->setIcon(QIcon(":/Graphics/Icons/Editing/redo.png"));
	m_redo->setToolTip(::makeToolTip(tr("Redo last action"), "Shift+Ctrl+Z"));

	m_search->setIcon(QIcon(":/Graphics/Icons/Editing/search.png"));
	m_search->setToolTip(::makeToolTip(tr("Search text"), "Ctrl+F"));
	m_search->setCheckable(true);
	m_search->setShortcut(QKeySequence("Ctrl+F"));

	m_fastFormat->setIcon(QIcon(":/Graphics/Icons/Editing/format.png"));
	m_fastFormat->setToolTip(tr("Text Fast Format"));
	m_fastFormat->setCheckable(true);

	m_duration->setToolTip(tr("Duration from Start to Cursor Position | Full Duration"));
	m_duration->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_duration->setAlignment(Qt::AlignRight);

	m_editor->setObjectName("scenarioEditor");
	m_editor->setPageFormat(ScenarioStyleFacade::style().pageSizeId());

	m_searchLine->setEditor(m_editor);
	m_searchLine->hide();

	m_fastFormatWidget->setEditor(m_editor);
	m_fastFormatWidget->hide();

	QHBoxLayout* topLayout = new QHBoxLayout(m_toolbar);
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(m_textStyles);
	topLayout->addWidget(m_undo);
	topLayout->addWidget(m_redo);
	topLayout->addWidget(m_search);
	topLayout->addWidget(m_fastFormat);
	topLayout->addWidget(m_duration);
	topLayout->addWidget(m_countersInfo);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_toolbar);
	mainLayout->addWidget(m_editor);
	mainLayout->addWidget(m_searchLine);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addLayout(mainLayout);
	layout->addWidget(m_fastFormatWidget);

	setLayout(layout);
}

void ScenarioTextEditWidget::initStylesCombo()
{
	ScenarioStyle style = ScenarioStyleFacade::style();
	int itemIndex = 0;

	if (style.blockStyle(ScenarioBlockStyle::TimeAndPlace).isActive()) {
		m_textStyles->addItem(tr("Time and Place"), ScenarioBlockStyle::TimeAndPlace);
		m_textStyles->setItemData(itemIndex++, ::makeShortcut("Ctrl+Return"), Qt::ToolTipRole);
	}

	if (style.blockStyle(ScenarioBlockStyle::SceneCharacters).isActive()) {
		m_textStyles->addItem(tr("Scene Characters"), ScenarioBlockStyle::SceneCharacters);
		m_textStyles->setItemData(itemIndex++, ::makeShortcut("Ctrl+E"), Qt::ToolTipRole);
	}

	if (style.blockStyle(ScenarioBlockStyle::Action).isActive()) {
		m_textStyles->addItem(tr("Action"), ScenarioBlockStyle::Action);
		m_textStyles->setItemData(itemIndex++, ::makeShortcut("Ctrl+J"), Qt::ToolTipRole);
	}

	if (style.blockStyle(ScenarioBlockStyle::Character).isActive()) {
		m_textStyles->addItem(tr("Character"), ScenarioBlockStyle::Character);
		m_textStyles->setItemData(itemIndex++, ::makeShortcut("Ctrl+U"), Qt::ToolTipRole);
	}

	if (style.blockStyle(ScenarioBlockStyle::Dialog).isActive()) {
		m_textStyles->addItem(tr("Dialog"), ScenarioBlockStyle::Dialog);
		m_textStyles->setItemData(itemIndex++, ::makeShortcut("Ctrl+L"), Qt::ToolTipRole);
	}

	if (style.blockStyle(ScenarioBlockStyle::Parenthetical).isActive()) {
		m_textStyles->addItem(tr("Parethentcial"), ScenarioBlockStyle::Parenthetical);
		m_textStyles->setItemData(itemIndex++, ::makeShortcut("Ctrl+H"), Qt::ToolTipRole);
	}

	if (style.blockStyle(ScenarioBlockStyle::Title).isActive()) {
		m_textStyles->addItem(tr("Title"), ScenarioBlockStyle::Title);
		m_textStyles->setItemData(itemIndex++, ::makeShortcut("Ctrl+N"), Qt::ToolTipRole);
	}

	if (style.blockStyle(ScenarioBlockStyle::Note).isActive()) {
		m_textStyles->addItem(tr("Note"), ScenarioBlockStyle::Note);
		m_textStyles->setItemData(itemIndex++, ::makeShortcut("Ctrl+P"), Qt::ToolTipRole);
	}

	if (style.blockStyle(ScenarioBlockStyle::Transition).isActive()) {
		m_textStyles->addItem(tr("Transition"), ScenarioBlockStyle::Transition);
		m_textStyles->setItemData(itemIndex++, ::makeShortcut("Ctrl+G"), Qt::ToolTipRole);
	}

	if (style.blockStyle(ScenarioBlockStyle::NoprintableText).isActive()) {
		m_textStyles->addItem(tr("Noprintable Text"), ScenarioBlockStyle::NoprintableText);
		m_textStyles->setItemData(itemIndex++, ::makeShortcut("Ctrl+Y"), Qt::ToolTipRole);
	}

	if (style.blockStyle(ScenarioBlockStyle::SceneGroupHeader).isActive()) {
		m_textStyles->addItem(tr("Scenes Group"), ScenarioBlockStyle::SceneGroupHeader);
		m_textStyles->setItemData(itemIndex++, ::makeShortcut("Ctrl+D"), Qt::ToolTipRole);
	}

	if (style.blockStyle(ScenarioBlockStyle::FolderHeader).isActive()) {
		m_textStyles->addItem(tr("Folder"), ScenarioBlockStyle::FolderHeader);
		m_textStyles->setItemData(itemIndex++, ::makeShortcut("Ctrl+Space"), Qt::ToolTipRole);
	}
}

void ScenarioTextEditWidget::initConnections()
{
	connect(m_textStyles, SIGNAL(activated(int)), this, SLOT(aboutChangeTextStyle()));
	connect(m_undo, SIGNAL(clicked()), this, SLOT(aboutUndo()));
	connect(m_redo, SIGNAL(clicked()), this, SLOT(aboutRedo()));
	connect(m_search, SIGNAL(toggled(bool)), this, SLOT(aboutShowSearch()));
	connect(m_fastFormat, SIGNAL(toggled(bool)), this, SLOT(aboutShowFastFormat()));

	initEditorConnections();
}

void ScenarioTextEditWidget::initEditorConnections()
{
	connect(m_editor, SIGNAL(undoAvailable(bool)), m_undo, SLOT(setEnabled(bool)));
	connect(m_editor, SIGNAL(redoAvailable(bool)), m_redo, SLOT(setEnabled(bool)));
	connect(m_editor, SIGNAL(currentStyleChanged()), this, SLOT(aboutUpdateTextStyle()));
	connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutUpdateTextStyle()));
	connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutCursorPositionChanged()));
	connect(m_editor, SIGNAL(textChanged()), this, SLOT(aboutTextChanged()));
	connect(m_editor, SIGNAL(styleChanged()), this, SLOT(aboutStyleChanged()));
	connect(m_editor, SIGNAL(zoomRangeChanged(int)), this, SIGNAL(zoomRangeChanged(int)));
}

void ScenarioTextEditWidget::removeEditorConnections()
{
	disconnect(m_editor, SIGNAL(undoAvailable(bool)), m_undo, SLOT(setEnabled(bool)));
	disconnect(m_editor, SIGNAL(redoAvailable(bool)), m_redo, SLOT(setEnabled(bool)));
	disconnect(m_editor, SIGNAL(currentStyleChanged()), this, SLOT(aboutUpdateTextStyle()));
	disconnect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutUpdateTextStyle()));
	disconnect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutCursorPositionChanged()));
	disconnect(m_editor, SIGNAL(textChanged()), this, SLOT(aboutTextChanged()));
	disconnect(m_editor, SIGNAL(styleChanged()), this, SLOT(aboutStyleChanged()));
	disconnect(m_editor, SIGNAL(zoomRangeChanged(int)), this, SIGNAL(zoomRangeChanged(int)));
}

void ScenarioTextEditWidget::initStyleSheet()
{
	m_textStyles->setProperty("inTopPanel", true);
	m_textStyles->setProperty("topPanelTopBordered", true);
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

	m_editor->setProperty("mainContainer", true);
}
