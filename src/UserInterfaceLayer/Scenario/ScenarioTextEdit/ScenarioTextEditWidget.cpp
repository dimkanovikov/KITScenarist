#include "ScenarioTextEditWidget.h"

#include "ScenarioTextEdit.h"

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
	m_textStyles(new QComboBox(this)),
	m_undo(new QToolButton(this)),
	m_redo(new QToolButton(this)),
	m_duration(new QLabel(this)),
	m_editor(new ScenarioTextEdit(this))
{
	initView();
	initConnections();
	initStyleSheet();
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

void ScenarioTextEditWidget::setUseSpellChecker(bool _use)
{
	m_editor->setUseSpellChecker(_use);
}

void ScenarioTextEditWidget::setTextEditBackgroundColor(const QColor& _color)
{
	m_editor->setStyleSheet(
				"QAbstractScrollArea {"
				"  background-color: " + _color.name() + ";"
				"}"
				);
}

void ScenarioTextEditWidget::setTextEditZoomRange(int _zoomRange)
{
	m_editor->setZoomRange(_zoomRange);
}

void ScenarioTextEditWidget::setCursorPosition(int _position)
{
	//
	// Устанавливаем позицию курсора
	//
	QTextCursor cursor = m_editor->textCursor();
	cursor.setPosition(_position);
	m_editor->setTextCursor(cursor);

	//
	// Сдвигаем скрол бар, чтобы не прилипало к низу редактора
	//
	m_editor->verticalScrollBar()->setValue(m_editor->verticalScrollBar()->value() + SCROLL_DELTA);
	m_editor->ensureCursorVisible();
	m_editor->setFocus();
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
		m_lastTextMd5Hash = currentTextMd5Hash;

		QWidget* topWidget = m_editor;
		while (topWidget->parentWidget() != 0) {
			topWidget = topWidget->parentWidget();
		}
		topWidget->setWindowModified(true);
	}
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

	m_durationTitle = new QLabel(tr("Chron: "), this);
	m_durationTitle->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_durationTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(m_textStyles);
	topLayout->addWidget(m_undo);
	topLayout->addWidget(m_redo);
	topLayout->addWidget(m_durationTitle);
	topLayout->addWidget(m_duration);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addLayout(topLayout);
	layout->addWidget(m_editor);

	setLayout(layout);
}

void ScenarioTextEditWidget::initConnections()
{
	connect(m_textStyles, SIGNAL(activated(int)), this, SLOT(aboutChangeTextStyle()), Qt::UniqueConnection);
	connect(m_undo, SIGNAL(clicked()), m_editor, SLOT(undo()));
	connect(m_redo, SIGNAL(clicked()), m_editor, SLOT(redo()));
	connect(m_editor, SIGNAL(undoAvailable(bool)), m_undo, SLOT(setEnabled(bool)));
	connect(m_editor, SIGNAL(redoAvailable(bool)), m_redo, SLOT(setEnabled(bool)));
	connect(m_editor, SIGNAL(currentStyleChanged()), this, SLOT(aboutUpdateTextStyle()));
	connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutUpdateTextStyle()));
	connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutCursorPositionChanged()));
	connect(m_editor, SIGNAL(textChanged()), this, SLOT(aboutTextChanged()));
	connect(m_editor, SIGNAL(zoomRangeChanged(int)), this, SIGNAL(zoomRangeChanged(int)));
}

void ScenarioTextEditWidget::initStyleSheet()
{
	m_textStyles->setProperty("inTopPanel", true);
	m_textStyles->setProperty("topPanelTopBordered", true);
	m_textStyles->setProperty("topPanelRightBordered", true);

	m_undo->setProperty("inTopPanel", true);
	m_redo->setProperty("inTopPanel", true);

	m_durationTitle->setProperty("inTopPanel", true);
	m_durationTitle->setProperty("topPanelTopBordered", true);

	m_duration->setProperty("inTopPanel", true);
	m_duration->setProperty("topPanelTopBordered", true);
	m_duration->setProperty("topPanelRightBordered", true);

	m_editor->setProperty("mainContainer", true);
}
