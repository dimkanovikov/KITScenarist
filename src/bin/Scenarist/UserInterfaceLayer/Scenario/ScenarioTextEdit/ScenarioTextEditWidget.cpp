#include "ScenarioTextEditWidget.h"

#include "ScenarioTextEdit.h"
#include "ScenarioTextEditHelpers.h"

#include <3rd_party/Widgets/SearchWidget/SearchWidget.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <QComboBox>
#include <QCryptographicHash>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QToolButton>
#include <QTextBlock>

using UserInterface::ScenarioTextEditWidget;
using UserInterface::ScenarioTextEdit;
using BusinessLogic::ScenarioTextBlockStyle;

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
}


ScenarioTextEditWidget::ScenarioTextEditWidget(QWidget* _parent) :
	QFrame(_parent),
	m_toolbar(new QWidget(this)),
	m_textStyles(new QComboBox(this)),
	m_undo(new QToolButton(this)),
	m_redo(new QToolButton(this)),
	m_search(new QToolButton(this)),
	m_fastFormat(new QToolButton(this)),
	m_durationTitle(new QLabel(this)),
	m_duration(new QLabel(this)),
	m_editor(new ScenarioTextEdit(this)),
	m_searchLine(new SearchWidget(this))
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
	m_editor->disconnect();

	m_editor->setScenarioDocument(_document);
	if (_document != 0) {
		m_lastTextMd5Hash = textMd5Hash(_document->toPlainText());
	}

	initConnections();
}

void ScenarioTextEditWidget::setDuration(const QString& _duration)
{
	m_duration->setText(_duration);
}

void ScenarioTextEditWidget::setUsePageView(bool _use)
{
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
		cursor.setPosition(_position);
		m_editor->setTextCursor(cursor);

		//
		// Сдвигаем скрол бар, чтобы не прилипало к низу редактора
		//
		m_editor->verticalScrollBar()->setValue(m_editor->verticalScrollBar()->value() + SCROLL_DELTA);
		m_editor->ensureCursorVisible();
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
	ScenarioTextBlockStyle::Type type = (ScenarioTextBlockStyle::Type)_type;
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
	if (ScenarioTextBlockStyle(type).isEmbeddableHeader()) {
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
		if (cursor.atStart()) {
			cursor.deleteChar();
		} else {
			cursor.deletePreviousChar();
		}
	}

	cursor.endEditBlock();
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

void ScenarioTextEditWidget::aboutUpdateTextStyle()
{
	ScenarioTextBlockStyle::Type currentType = m_editor->scenarioBlockType();
	if (currentType == ScenarioTextBlockStyle::TitleHeader) {
		currentType = ScenarioTextBlockStyle::Title;
	} else if (currentType == ScenarioTextBlockStyle::SceneGroupFooter) {
		currentType = ScenarioTextBlockStyle::SceneGroupHeader;
	} else if (currentType == ScenarioTextBlockStyle::FolderFooter) {
		currentType = ScenarioTextBlockStyle::FolderHeader;
	}

	for (int itemIndex = 0; itemIndex < m_textStyles->count(); ++itemIndex) {
		ScenarioTextBlockStyle::Type itemType =
				(ScenarioTextBlockStyle::Type)m_textStyles->itemData(itemIndex).toInt();
		if (itemType == currentType) {
			m_textStyles->setCurrentIndex(itemIndex);
			break;
		}
	}
}

void ScenarioTextEditWidget::aboutChangeTextStyle()
{
	ScenarioTextBlockStyle::Type type =
			(ScenarioTextBlockStyle::Type)m_textStyles->itemData(m_textStyles->currentIndex()).toInt();

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
	}
}

void ScenarioTextEditWidget::aboutStyleChanged()
{
	emit textChanged();
}

void ScenarioTextEditWidget::initView()
{
	m_textStyles->setSizePolicy(m_textStyles->sizePolicy().horizontalPolicy(), QSizePolicy::Preferred);
	m_textStyles->addItem(tr("Time and Place"), ScenarioTextBlockStyle::TimeAndPlace);
	m_textStyles->addItem(tr("Action"), ScenarioTextBlockStyle::Action);
	m_textStyles->addItem(tr("Character"), ScenarioTextBlockStyle::Character);
	m_textStyles->addItem(tr("Dialog"), ScenarioTextBlockStyle::Dialog);
	m_textStyles->addItem(tr("Parethentcial"), ScenarioTextBlockStyle::Parenthetical);
	m_textStyles->addItem(tr("Title"), ScenarioTextBlockStyle::Title);
	m_textStyles->addItem(tr("Note"), ScenarioTextBlockStyle::Note);
	m_textStyles->addItem(tr("Transition"), ScenarioTextBlockStyle::Transition);
	m_textStyles->addItem(tr("Noprintable Text"), ScenarioTextBlockStyle::NoprintableText);
	m_textStyles->addItem(tr("Scenes Group"), ScenarioTextBlockStyle::SceneGroupHeader);
	m_textStyles->addItem(tr("Folder"), ScenarioTextBlockStyle::FolderHeader);

	m_undo->setIcon(QIcon(":/Graphics/Icons/Editing/undo.png"));
	m_redo->setIcon(QIcon(":/Graphics/Icons/Editing/redo.png"));

	m_search->setIcon(QIcon(":/Graphics/Icons/Editing/search.png"));
	m_search->setCheckable(true);
	m_search->setShortcut(QKeySequence("Ctrl+F"));

	m_fastFormat->setIcon(QIcon(":/Graphics/Icons/Editing/format.png"));

	m_durationTitle->setText(tr("Chron: "));
	m_durationTitle->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_durationTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	//
	// К сожалению настройка не идеальна и пришлось подбирать в ручную
	//
	m_editor->setPageSize(60, 50);

	m_searchLine->setEditor(m_editor);
	m_searchLine->hide();

	QHBoxLayout* topLayout = new QHBoxLayout(m_toolbar);
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(m_textStyles);
	topLayout->addWidget(m_undo);
	topLayout->addWidget(m_redo);
	topLayout->addWidget(m_search);
	topLayout->addWidget(m_fastFormat);
	topLayout->addWidget(m_durationTitle);
	topLayout->addWidget(m_duration);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(m_toolbar);
	layout->addWidget(m_editor);
	layout->addWidget(m_searchLine);

	setLayout(layout);
}

void ScenarioTextEditWidget::initConnections()
{
	connect(m_textStyles, SIGNAL(activated(int)), this, SLOT(aboutChangeTextStyle()), Qt::UniqueConnection);
	connect(m_undo, SIGNAL(clicked()), this, SLOT(aboutUndo()), Qt::UniqueConnection);
	connect(m_redo, SIGNAL(clicked()), this, SLOT(aboutRedo()), Qt::UniqueConnection);
	connect(m_search, SIGNAL(toggled(bool)), this, SLOT(aboutShowSearch()));
	connect(m_editor, SIGNAL(undoAvailable(bool)), m_undo, SLOT(setEnabled(bool)), Qt::UniqueConnection);
	connect(m_editor, SIGNAL(redoAvailable(bool)), m_redo, SLOT(setEnabled(bool)), Qt::UniqueConnection);
	connect(m_editor, SIGNAL(currentStyleChanged()), this, SLOT(aboutUpdateTextStyle()), Qt::UniqueConnection);
	connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutUpdateTextStyle()), Qt::UniqueConnection);
	connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutCursorPositionChanged()), Qt::UniqueConnection);
	connect(m_editor, SIGNAL(textChanged()), this, SLOT(aboutTextChanged()), Qt::UniqueConnection);
	connect(m_editor, SIGNAL(styleChanged()), this, SLOT(aboutStyleChanged()), Qt::UniqueConnection);
	connect(m_editor, SIGNAL(zoomRangeChanged(int)), this, SIGNAL(zoomRangeChanged(int)), Qt::UniqueConnection);

	m_editor->init();
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

	m_durationTitle->setProperty("inTopPanel", true);
	m_durationTitle->setProperty("topPanelTopBordered", true);

	m_duration->setProperty("inTopPanel", true);
	m_duration->setProperty("topPanelTopBordered", true);
	m_duration->setProperty("topPanelRightBordered", true);

	m_editor->setProperty("mainContainer", true);
}
