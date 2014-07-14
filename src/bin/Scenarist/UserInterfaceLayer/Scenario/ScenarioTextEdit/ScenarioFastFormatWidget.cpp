#include "ScenarioFastFormatWidget.h"

#include "ScenarioTextEdit.h"

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>

#include <QShortcut>
#include <QToolButton>
#include <QVBoxLayout>

using UserInterface::ScenarioFastFormatWidget;
using UserInterface::ScenarioTextEdit;
using BusinessLogic::ScenarioBlockStyle;

namespace {
	/**
	 * @brief Свойство виджета, в котором сохраняется стиль блока
	 */
	const char* STYLE_PROPERTY_KEY = "block_style";

	/**
	 * @brief Создать кнопку применения стиля
	 */
	QToolButton* createStyleButton(ScenarioFastFormatWidget* _parent, const QString& _text, Qt::Key _key,
		ScenarioBlockStyle::Type _type) {
		QToolButton* styleButton = new QToolButton(_parent);
		styleButton->setText(_text);
		styleButton->setCheckable(true);
		styleButton->setProperty(STYLE_PROPERTY_KEY, _type);
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
	m_editor(0)
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


	QToolButton* goToPrevBlock = new QToolButton(this);
	goToPrevBlock->setText(tr("↑ Prev"));
	connect(goToPrevBlock, SIGNAL(clicked()), this, SLOT(aboutGoToPrevBlock()));
	QShortcut* goToPrevShortcut = new QShortcut(Qt::Key_Up, this);
	connect(goToPrevShortcut, SIGNAL(activated()), goToPrevBlock, SLOT(click()));

	m_buttons << ::createStyleButton(this, tr("0 Time and Place"), Qt::Key_0, ScenarioBlockStyle::TimeAndPlace);
	m_buttons << ::createStyleButton(this, tr("1 Scene Characters"), Qt::Key_1, ScenarioBlockStyle::SceneCharacters);
	m_buttons << ::createStyleButton(this, tr("2 Action"), Qt::Key_2, ScenarioBlockStyle::Action);
	m_buttons << ::createStyleButton(this, tr("3 Character"), Qt::Key_3, ScenarioBlockStyle::Character);
	m_buttons << ::createStyleButton(this, tr("4 Dialog"), Qt::Key_4, ScenarioBlockStyle::Dialog);
	m_buttons << ::createStyleButton(this, tr("5 Parenthetical"), Qt::Key_5, ScenarioBlockStyle::Parenthetical);
	m_buttons << ::createStyleButton(this, tr("6 Transition"), Qt::Key_6, ScenarioBlockStyle::Transition);
	m_buttons << ::createStyleButton(this, tr("7 Note"), Qt::Key_7, ScenarioBlockStyle::Note);
	m_buttons << ::createStyleButton(this, tr("8 Title"), Qt::Key_8, ScenarioBlockStyle::Title);
	m_buttons << ::createStyleButton(this, tr("9 Noprintable Text"), Qt::Key_9, ScenarioBlockStyle::NoprintableText);

	QToolButton* goToNextBlock = new QToolButton(this);
	goToNextBlock->setText(tr("↓ Next"));
	connect(goToNextBlock, SIGNAL(clicked()), this, SLOT(aboutGoToNextBlock()));
	QShortcut* goToNextShortcut = new QShortcut(Qt::Key_Down, this);
	connect(goToNextShortcut, SIGNAL(activated()), goToNextBlock, SLOT(click()));


	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(goToPrevBlock);
	layout->addSpacing(6);
	foreach (QToolButton* button, m_buttons) {
		layout->addWidget(button);
	}
	layout->addSpacing(6);
	layout->addWidget(goToNextBlock);
	layout->addStretch();

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

void ScenarioFastFormatWidget::aboutGoToNextBlock()
{
	if (m_editor != 0) {
		QTextCursor cursor = m_editor->textCursor();
		cursor.movePosition(QTextCursor::NextBlock);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		m_editor->setTextCursor(cursor);
	}
}

void ScenarioFastFormatWidget::aboutGoToPrevBlock()
{
	if (m_editor != 0) {
		QTextCursor cursor = m_editor->textCursor();
		cursor.movePosition(QTextCursor::PreviousBlock);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		m_editor->setTextCursor(cursor);
	}
}

void ScenarioFastFormatWidget::aboutChangeStyle()
{
	if (QToolButton* button = qobject_cast<QToolButton*>(sender())) {
		ScenarioBlockStyle::Type type =
				(ScenarioBlockStyle::Type)button->property(STYLE_PROPERTY_KEY).toInt();
		if (m_editor != 0) {
			m_editor->changeScenarioBlockType(type);
			selectCurrentBlock();
		}
	}
}

void ScenarioFastFormatWidget::aboutCurrentStyleChanged()
{
	ScenarioBlockStyle::Type currentType = m_editor->scenarioBlockType();
	foreach (QToolButton* button, m_buttons) {
		button->setChecked(
			(ScenarioBlockStyle::Type)button->property(STYLE_PROPERTY_KEY).toInt() == currentType);
	}
}
