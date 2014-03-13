#include "ScenarioTextEditWidget.h"

#include "ScenarioTextEdit.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>

using UserInterface::ScenarioTextEditWidget;
using UserInterface::ScenarioTextEdit;
using BusinessLogic::ScenarioTextBlockStyle;

namespace {
	const int SCROLL_DELTA = 140;
}


ScenarioTextEditWidget::ScenarioTextEditWidget(QWidget* _parent) :
	QFrame(_parent),
	m_textStyles(new QComboBox(this)),
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
	QWidget* topWidget = m_editor;
	while (topWidget->parentWidget() != 0) {
		topWidget = topWidget->parentWidget();
	}
	topWidget->setWindowModified(true);
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

	m_durationTitle = new QLabel(tr("Chron: "), this);
	m_durationTitle->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_durationTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(m_textStyles);
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
	connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutUpdateTextStyle()));
    connect(m_editor, SIGNAL(cursorPositionChanged()), this, SLOT(aboutCursorPositionChanged()));
	connect(m_editor, SIGNAL(textChanged()), this, SLOT(aboutTextChanged()));
}

void ScenarioTextEditWidget::initStyleSheet()
{
	m_textStyles->setProperty("inTopPanel", true);
	m_textStyles->setProperty("topPanelTopBordered", true);
	m_textStyles->setProperty("topPanelRightBordered", true);

	m_durationTitle->setProperty("inTopPanel", true);
	m_durationTitle->setProperty("topPanelTopBordered", true);

	m_duration->setProperty("inTopPanel", true);
	m_duration->setProperty("topPanelTopBordered", true);
	m_duration->setProperty("topPanelRightBordered", true);

	m_editor->setProperty("mainContainer", true);
}
