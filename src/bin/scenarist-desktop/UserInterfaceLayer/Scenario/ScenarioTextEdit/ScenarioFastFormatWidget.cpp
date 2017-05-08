#include "ScenarioFastFormatWidget.h"

#include <UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEdit.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <QCheckBox>
#include <QPainter>
#include <QPushButton>
#include <QShortcut>
#include <QTextBlock>
#include <QVBoxLayout>


using UserInterface::ScenarioFastFormatWidget;
using UserInterface::ScenarioTextEdit;
using BusinessLogic::ScenarioBlockStyle;
using BusinessLogic::ScenarioTemplate;
using BusinessLogic::ScenarioTemplateFacade;

namespace {
	/**
	 * @brief Свойство виджета, в котором сохраняется стиль блока
	 */
	const char* STYLE_PROPERTY_KEY = "block_style";

	/**
	 * @brief Создать кнопку применения стиля
	 */
	QPushButton* createStyleButton(ScenarioFastFormatWidget* _parent, Qt::Key _key) {
		QPushButton* styleButton = new QPushButton(_parent);
		styleButton->setCheckable(true);
		styleButton->setProperty("leftAlignedText", true);

		_parent->connect(styleButton, SIGNAL(clicked()), _parent, SLOT(aboutChangeStyle()));
		QShortcut* shortcut1 = new QShortcut(_key, _parent);
		QShortcut* shortcut2 = new QShortcut(_key + Qt::KeypadModifier, _parent);
		_parent->connect(shortcut1, SIGNAL(activated()), styleButton, SLOT(click()));
		_parent->connect(shortcut2, SIGNAL(activated()), styleButton, SLOT(click()));

		return styleButton;
	}
}


ScenarioFastFormatWidget::ScenarioFastFormatWidget(QWidget *parent) :
	QFrame(parent),
	m_editor(0),
	m_grabFocus(new QCheckBox(this))
{
	setFrameShape(QFrame::Box);
	setProperty("fastFormatWidget", true);


	QPushButton* goToPrevBlock = new QPushButton(this);
	goToPrevBlock->setCheckable(false);
	goToPrevBlock->setText(tr("↑ Prev"));
	connect(goToPrevBlock, SIGNAL(clicked()), this, SLOT(aboutGoToPrevBlock()));
	QShortcut* goToPrevShortcut = new QShortcut(Qt::Key_Up, this);
	connect(goToPrevShortcut, SIGNAL(activated()), goToPrevBlock, SLOT(click()));

	m_buttons << ::createStyleButton(this, Qt::Key_0);
	m_buttons << ::createStyleButton(this, Qt::Key_1);
	m_buttons << ::createStyleButton(this, Qt::Key_2);
	m_buttons << ::createStyleButton(this, Qt::Key_3);
	m_buttons << ::createStyleButton(this, Qt::Key_4);
	m_buttons << ::createStyleButton(this, Qt::Key_5);
	m_buttons << ::createStyleButton(this, Qt::Key_6);
	m_buttons << ::createStyleButton(this, Qt::Key_7);
	m_buttons << ::createStyleButton(this, Qt::Key_8);
	m_buttons << ::createStyleButton(this, Qt::Key_9);
	reinitBlockStyles();

	QPushButton* goToNextBlock = new QPushButton(this);
	goToNextBlock->setCheckable(false);
	goToNextBlock->setText(tr("↓ Next"));
	connect(goToNextBlock, SIGNAL(clicked()), this, SLOT(aboutGoToNextBlock()));
	QShortcut* goToNextShortcut = new QShortcut(Qt::Key_Down, this);
	connect(goToNextShortcut, SIGNAL(activated()), goToNextBlock, SLOT(click()));

	m_grabFocus->setText(tr("Catch focus"));
	connect(m_grabFocus, SIGNAL(toggled(bool)), this, SLOT(aboutGrabCursorChanged(bool)));

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(goToPrevBlock);
	layout->addSpacing(6);
	foreach (QPushButton* button, m_buttons) {
		layout->addWidget(button);
	}
	layout->addSpacing(6);
	layout->addWidget(goToNextBlock);
	layout->addStretch();
	layout->addWidget(m_grabFocus);

	setLayout(layout);
}

void ScenarioFastFormatWidget::setEditor(ScenarioTextEdit* _editor)
{
	if (m_editor != _editor) {
		m_editor = _editor;

		if (m_editor != 0) {
			connect(m_editor, SIGNAL(currentStyleChanged()), this, SLOT(aboutCurrentStyleChanged()));
		}
	}
}

void ScenarioFastFormatWidget::selectCurrentBlock()
{
    if (m_editor != 0
        && m_grabFocus->isChecked()) {
		QTextCursor cursor = m_editor->textCursor();
		cursor.movePosition(QTextCursor::StartOfBlock);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		m_editor->setTextCursor(cursor);
	}
}

void ScenarioFastFormatWidget::reinitBlockStyles()
{
	ScenarioTemplate style = ScenarioTemplateFacade::getTemplate();
	const bool BEAUTIFY_NAME = true;

	//
	// Настраиваем в зависимости от доступности стиля
	//
	int itemIndex = 0;

	QList<ScenarioBlockStyle::Type> types;
	types << ScenarioBlockStyle::SceneHeading
		  << ScenarioBlockStyle::SceneCharacters
		  << ScenarioBlockStyle::Action
		  << ScenarioBlockStyle::Character
		  << ScenarioBlockStyle::Dialogue
		  << ScenarioBlockStyle::Parenthetical
		  << ScenarioBlockStyle::Title
		  << ScenarioBlockStyle::Note
		  << ScenarioBlockStyle::Transition
		  << ScenarioBlockStyle::NoprintableText;

	foreach (ScenarioBlockStyle::Type type, types) {
		if (style.blockStyle(type).isActive()) {
			m_buttons.at(itemIndex)->setVisible(true);
			m_buttons.at(itemIndex)->setText(
				tr("%1 %2").arg(itemIndex).arg(ScenarioBlockStyle::typeName(type, BEAUTIFY_NAME)));
			m_buttons.at(itemIndex)->setProperty(STYLE_PROPERTY_KEY, type);
			++itemIndex;
		}
	}

	for (; itemIndex < m_buttons.count(); ++itemIndex) {
		m_buttons.at(itemIndex)->setVisible(false);
	}
}

void ScenarioFastFormatWidget::aboutGoToNextBlock()
{
	if (m_editor != 0) {
		QTextCursor cursor = m_editor->textCursor();
		do {
			cursor.movePosition(QTextCursor::NextBlock);
			cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		} while (!cursor.atEnd()
				 && !cursor.block().isVisible());
		m_editor->setTextCursor(cursor);
		catchFocusIfNeeded();
	}
}

void ScenarioFastFormatWidget::aboutGoToPrevBlock()
{
	if (m_editor != 0) {
		QTextCursor cursor = m_editor->textCursor();
		do {
			cursor.movePosition(QTextCursor::PreviousBlock);
			cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		} while (cursor.selectionStart() != 0
				 && !cursor.block().isVisible());
		m_editor->setTextCursor(cursor);
		catchFocusIfNeeded();
	}
}

void ScenarioFastFormatWidget::aboutChangeStyle()
{
	if (QPushButton* button = qobject_cast<QPushButton*>(sender())) {
		button->setChecked(true);
		ScenarioBlockStyle::Type type =
				(ScenarioBlockStyle::Type)button->property(STYLE_PROPERTY_KEY).toInt();
		if (m_editor != 0) {
			m_editor->changeScenarioBlockType(type);
			selectCurrentBlock();
			catchFocusIfNeeded();
		}
	}
}

void ScenarioFastFormatWidget::aboutCurrentStyleChanged()
{
	ScenarioBlockStyle::Type currentType = m_editor->scenarioBlockType();
	foreach (QPushButton* button, m_buttons) {
		button->setChecked(
			(ScenarioBlockStyle::Type)button->property(STYLE_PROPERTY_KEY).toInt() == currentType);
	}
}

void ScenarioFastFormatWidget::aboutGrabCursorChanged(bool _catch)
{
	if (_catch) {
		selectCurrentBlock();
	}
	catchFocusIfNeeded();
}

void ScenarioFastFormatWidget::catchFocusIfNeeded()
{
	//
	// Если необходимо захватим фокус
	//
	if (m_grabFocus->isChecked()) {
		this->setFocus();
	}
	//
	// Или снимем выделение, для продолжения редактирования текста
	//
	else {
        if (m_editor != 0) {
			QTextCursor cursor = m_editor->textCursor();
			cursor.setPosition(qMax(cursor.selectionStart(), cursor.selectionEnd()));
            m_editor->setTextCursor(cursor);
            emit focusMovedToEditor();
		}
	}
}


