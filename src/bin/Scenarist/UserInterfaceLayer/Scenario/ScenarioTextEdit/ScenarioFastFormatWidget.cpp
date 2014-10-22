#include "ScenarioFastFormatWidget.h"

#include "ScenarioTextEdit.h"

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>

#include <QCheckBox>
#include <QLabel>
#include <QShortcut>
#include <QVBoxLayout>

//********
// ToolButton

ToolButton::ToolButton(QWidget* _parent) :
	QToolButton(_parent),
	m_label(new QLabel(this))
{
	QToolButton::setText(QString());
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	setCheckable(true);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(8, 4, 8, 4);
	layout->addWidget(m_label);
	setLayout(layout);
}

void ToolButton::setText(const QString& _text)
{
	m_label->setText(_text);
}

QSize ToolButton::sizeHint() const
{
	return layout()->sizeHint();
}

//********
// ScenarioFastFormatWidget

using UserInterface::ScenarioFastFormatWidget;
using UserInterface::ScenarioTextEdit;
using BusinessLogic::ScenarioBlockStyle;
using BusinessLogic::ScenarioStyle;
using BusinessLogic::ScenarioStyleFacade;

namespace {
	/**
	 * @brief Свойство виджета, в котором сохраняется стиль блока
	 */
	const char* STYLE_PROPERTY_KEY = "block_style";

	/**
	 * @brief Создать кнопку применения стиля
	 */
	ToolButton* createStyleButton(ScenarioFastFormatWidget* _parent, Qt::Key _key) {
		ToolButton* styleButton = new ToolButton(_parent);
		_parent->connect(styleButton, SIGNAL(clicked()), _parent, SLOT(aboutChangeStyle()));
		QShortcut* timeAndPlaceShortcut1 = new QShortcut(_key, _parent);
		QShortcut* timeAndPlaceShortcut2 = new QShortcut(_key + Qt::KeypadModifier, _parent);
		_parent->connect(timeAndPlaceShortcut1, SIGNAL(activated()), styleButton, SLOT(click()));
		_parent->connect(timeAndPlaceShortcut2, SIGNAL(activated()), styleButton, SLOT(click()));

		return styleButton;
	}
}


ScenarioFastFormatWidget::ScenarioFastFormatWidget(QWidget *parent) :
	QFrame(parent),
	m_editor(0),
	m_grabFocus(new QCheckBox(this))
{
	setFrameShape(QFrame::Box);
	setStyleSheet("*[fastFormatWidget=\"true\"] {"
				  "  border: 0px solid black; "
				  "  border-left-width: 1px; "
				  "  border-left-style: solid; "
				  "  border-left-color: palette(dark);"
				  "}"
				  "QPushButton {"
				  "padding: 3px 6px 3px 6px; text-align: left;"
				  "}"
				  );
	setProperty("fastFormatWidget", true);


	ToolButton* goToPrevBlock = new ToolButton(this);
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

	ToolButton* goToNextBlock = new ToolButton(this);
	goToNextBlock->setCheckable(false);
	goToNextBlock->setText(tr("↓ Next"));
	connect(goToNextBlock, SIGNAL(clicked()), this, SLOT(aboutGoToNextBlock()));
	QShortcut* goToNextShortcut = new QShortcut(Qt::Key_Down, this);
	connect(goToNextShortcut, SIGNAL(activated()), goToNextBlock, SLOT(click()));

	m_grabFocus->setText(tr("Catch focus"));

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(goToPrevBlock);
	layout->addSpacing(6);
	foreach (ToolButton* button, m_buttons) {
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
	if (m_editor != 0) {
		QTextCursor cursor = m_editor->textCursor();
		cursor.movePosition(QTextCursor::StartOfBlock);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		m_editor->setTextCursor(cursor);
	}
}

void ScenarioFastFormatWidget::reinitBlockStyles()
{
	ScenarioStyle style = ScenarioStyleFacade::style();

	//
	// Настраиваем в зависимости от доступности стиля
	//
	int itemIndex = 0;

	if (style.blockStyle(ScenarioBlockStyle::TimeAndPlace).isActive()) {
		m_buttons.at(itemIndex)->setVisible(true);
		m_buttons.at(itemIndex)->setText(tr("%1 Time and Place").arg(itemIndex));
		m_buttons.at(itemIndex)->setProperty(STYLE_PROPERTY_KEY, ScenarioBlockStyle::TimeAndPlace);
		++itemIndex;
	}

	if (style.blockStyle(ScenarioBlockStyle::SceneCharacters).isActive()) {
		m_buttons.at(itemIndex)->setVisible(true);
		m_buttons.at(itemIndex)->setText(tr("%1 Scene Characters").arg(itemIndex));
		m_buttons.at(itemIndex)->setProperty(STYLE_PROPERTY_KEY, ScenarioBlockStyle::SceneCharacters);
		++itemIndex;
	}

	if (style.blockStyle(ScenarioBlockStyle::Action).isActive()) {
		m_buttons.at(itemIndex)->setVisible(true);
		m_buttons.at(itemIndex)->setText(tr("%1 Action").arg(itemIndex));
		m_buttons.at(itemIndex)->setProperty(STYLE_PROPERTY_KEY, ScenarioBlockStyle::Action);
		++itemIndex;
	}

	if (style.blockStyle(ScenarioBlockStyle::Character).isActive()) {
		m_buttons.at(itemIndex)->setVisible(true);
		m_buttons.at(itemIndex)->setText(tr("%1 Character").arg(itemIndex));
		m_buttons.at(itemIndex)->setProperty(STYLE_PROPERTY_KEY, ScenarioBlockStyle::Character);
		++itemIndex;
	}

	if (style.blockStyle(ScenarioBlockStyle::Dialog).isActive()) {
		m_buttons.at(itemIndex)->setVisible(true);
		m_buttons.at(itemIndex)->setText(tr("%1 Dialog").arg(itemIndex));
		m_buttons.at(itemIndex)->setProperty(STYLE_PROPERTY_KEY, ScenarioBlockStyle::Dialog);
		++itemIndex;
	}

	if (style.blockStyle(ScenarioBlockStyle::Parenthetical).isActive()) {
		m_buttons.at(itemIndex)->setVisible(true);
		m_buttons.at(itemIndex)->setText(tr("%1 Parenthetical").arg(itemIndex));
		m_buttons.at(itemIndex)->setProperty(STYLE_PROPERTY_KEY, ScenarioBlockStyle::Parenthetical);
		++itemIndex;
	}

	if (style.blockStyle(ScenarioBlockStyle::Title).isActive()) {
		m_buttons.at(itemIndex)->setVisible(true);
		m_buttons.at(itemIndex)->setText(tr("%1 Title").arg(itemIndex));
		m_buttons.at(itemIndex)->setProperty(STYLE_PROPERTY_KEY, ScenarioBlockStyle::Title);
		++itemIndex;
	}

	if (style.blockStyle(ScenarioBlockStyle::Note).isActive()) {
		m_buttons.at(itemIndex)->setVisible(true);
		m_buttons.at(itemIndex)->setText(tr("%1 Note").arg(itemIndex));
		m_buttons.at(itemIndex)->setProperty(STYLE_PROPERTY_KEY, ScenarioBlockStyle::Note);
		++itemIndex;
	}

	if (style.blockStyle(ScenarioBlockStyle::Transition).isActive()) {
		m_buttons.at(itemIndex)->setVisible(true);
		m_buttons.at(itemIndex)->setText(tr("%1 Transition").arg(itemIndex));
		m_buttons.at(itemIndex)->setProperty(STYLE_PROPERTY_KEY, ScenarioBlockStyle::Transition);
		++itemIndex;
	}

	if (style.blockStyle(ScenarioBlockStyle::NoprintableText).isActive()) {
		m_buttons.at(itemIndex)->setVisible(true);
		m_buttons.at(itemIndex)->setText(tr("%1 NoprintableText").arg(itemIndex));
		m_buttons.at(itemIndex)->setProperty(STYLE_PROPERTY_KEY, ScenarioBlockStyle::NoprintableText);
		++itemIndex;
	}

	for (; itemIndex < m_buttons.count(); ++itemIndex) {
		m_buttons.at(itemIndex)->setVisible(false);
	}
}

void ScenarioFastFormatWidget::aboutGoToNextBlock()
{
	if (m_editor != 0) {
		QTextCursor cursor = m_editor->textCursor();
		cursor.movePosition(QTextCursor::NextBlock);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		m_editor->setTextCursor(cursor);
		catchFocusIfNeeded();
	}
}

void ScenarioFastFormatWidget::aboutGoToPrevBlock()
{
	if (m_editor != 0) {
		QTextCursor cursor = m_editor->textCursor();
		cursor.movePosition(QTextCursor::PreviousBlock);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		m_editor->setTextCursor(cursor);
		catchFocusIfNeeded();
	}
}

void ScenarioFastFormatWidget::aboutChangeStyle()
{
	if (ToolButton* button = qobject_cast<ToolButton*>(sender())) {
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
	foreach (ToolButton* button, m_buttons) {
		button->setChecked(
			(ScenarioBlockStyle::Type)button->property(STYLE_PROPERTY_KEY).toInt() == currentType);
	}
}

void ScenarioFastFormatWidget::catchFocusIfNeeded()
{
	//
	// Если необходимо захватим фокус
	//
	if (m_grabFocus->isChecked()) {
		this->setFocus();
	}
}

