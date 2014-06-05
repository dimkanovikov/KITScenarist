#include "ScenarioFastFormatWidget.h"

#include "ScenarioTextEdit.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h>

#include <QPushButton>
#include <QShortcut>
#include <QVBoxLayout>

using UserInterface::ScenarioFastFormatWidget;
using UserInterface::ScenarioTextEdit;
using BusinessLogic::ScenarioTextBlockStyle;

namespace {
	const char* STYLE_PROPERTY_KEY = "block_style";
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

	QPushButton* goToPrevBlock = new QPushButton(tr("↑ Prev"), this);
	connect(goToPrevBlock, SIGNAL(clicked()), this, SLOT(goToPrevBlock()));
	QShortcut* goToPrevShortcut = new QShortcut(Qt::Key_Up, this);
	connect (goToPrevShortcut, SIGNAL(activated()), goToPrevBlock, SLOT(click()));

	QPushButton* timeAndPlaceStyle = new QPushButton(tr("0 Time and Place"), this);
	timeAndPlaceStyle->setProperty(STYLE_PROPERTY_KEY, ScenarioTextBlockStyle::TimeAndPlace);
	connect(timeAndPlaceStyle, SIGNAL(clicked()), this, SLOT(changeStyle()));
	QShortcut* timeAndPlaceShortcut1 = new QShortcut(Qt::Key_0, this);
	QShortcut* timeAndPlaceShortcut2 = new QShortcut(Qt::Key_0 + Qt::KeypadModifier, this);
	connect(timeAndPlaceShortcut1, SIGNAL(activated()), timeAndPlaceStyle, SLOT(click()));
	connect(timeAndPlaceShortcut2, SIGNAL(activated()), timeAndPlaceStyle, SLOT(click()));

	QPushButton* actionStyle = new QPushButton(tr("1 Action"), this);
	actionStyle->setProperty(STYLE_PROPERTY_KEY, ScenarioTextBlockStyle::Action);
	connect(actionStyle, SIGNAL(clicked()), this, SLOT(changeStyle()));
	QShortcut* actionShortcut1 = new QShortcut(Qt::Key_1, this);
	QShortcut* actionShortcut2 = new QShortcut(Qt::Key_1 + Qt::KeypadModifier, this);
	connect(actionShortcut1, SIGNAL(activated()), actionStyle, SLOT(click()));
	connect(actionShortcut2, SIGNAL(activated()), actionStyle, SLOT(click()));

	QPushButton* characterStyle = new QPushButton(tr("2 Character"), this);
	characterStyle->setProperty(STYLE_PROPERTY_KEY, ScenarioTextBlockStyle::Character);
	connect(characterStyle, SIGNAL(clicked()), this, SLOT(changeStyle()));
	QShortcut* characterShortcut1 = new QShortcut(Qt::Key_2, this);
	QShortcut* characterShortcut2 = new QShortcut(Qt::Key_2 + Qt::KeypadModifier, this);
	connect(characterShortcut1, SIGNAL(activated()), characterStyle, SLOT(click()));
	connect(characterShortcut2, SIGNAL(activated()), characterStyle, SLOT(click()));

	QPushButton* dialogStyle = new QPushButton(tr("3 Dialog"), this);
	dialogStyle->setProperty(STYLE_PROPERTY_KEY, ScenarioTextBlockStyle::Dialog);
	connect(dialogStyle, SIGNAL(clicked()), this, SLOT(changeStyle()));
	QShortcut* dialogShortcut1 = new QShortcut(Qt::Key_3, this);
	QShortcut* dialogShortcut2 = new QShortcut(Qt::Key_3 + Qt::KeypadModifier, this);
	connect(dialogShortcut1, SIGNAL(activated()), dialogStyle, SLOT(click()));
	connect(dialogShortcut2, SIGNAL(activated()), dialogStyle, SLOT(click()));

	QPushButton* parentheticalStyle = new QPushButton(tr("4 Parenthetical"), this);
	parentheticalStyle->setProperty(STYLE_PROPERTY_KEY, ScenarioTextBlockStyle::Parenthetical);
	connect(parentheticalStyle, SIGNAL(clicked()), this, SLOT(changeStyle()));
	QShortcut* parentheticalShortcut1 = new QShortcut(Qt::Key_4, this);
	QShortcut* parentheticalShortcut2 = new QShortcut(Qt::Key_4 + Qt::KeypadModifier, this);
	connect(parentheticalShortcut1, SIGNAL(activated()), parentheticalStyle, SLOT(click()));
	connect(parentheticalShortcut2, SIGNAL(activated()), parentheticalStyle, SLOT(click()));

	QPushButton* transitionStyle = new QPushButton(tr("5 Transition"), this);
	transitionStyle->setProperty(STYLE_PROPERTY_KEY, ScenarioTextBlockStyle::Transition);
	connect(transitionStyle, SIGNAL(clicked()), this, SLOT(changeStyle()));
	QShortcut* transitionShortcut1 = new QShortcut(Qt::Key_5, this);
	QShortcut* transitionShortcut2 = new QShortcut(Qt::Key_5 + Qt::KeypadModifier, this);
	connect(transitionShortcut1, SIGNAL(activated()), transitionStyle, SLOT(click()));
	connect(transitionShortcut2, SIGNAL(activated()), transitionStyle, SLOT(click()));

	QPushButton* noteStyle = new QPushButton(tr("6 Note"), this);
	noteStyle->setProperty(STYLE_PROPERTY_KEY, ScenarioTextBlockStyle::Note);
	connect(noteStyle, SIGNAL(clicked()), this, SLOT(changeStyle()));
	QShortcut* noteShortcut1 = new QShortcut(Qt::Key_6, this);
	QShortcut* noteShortcut2 = new QShortcut(Qt::Key_6 + Qt::KeypadModifier, this);
	connect(noteShortcut1, SIGNAL(activated()), noteStyle, SLOT(click()));
	connect(noteShortcut2, SIGNAL(activated()), noteStyle, SLOT(click()));

	QPushButton* titleStyle = new QPushButton(tr("7 Title"), this);
	titleStyle->setProperty(STYLE_PROPERTY_KEY, ScenarioTextBlockStyle::Title);
	connect(titleStyle, SIGNAL(clicked()), this, SLOT(changeStyle()));
	QShortcut* titleShortcut1 = new QShortcut(Qt::Key_7, this);
	QShortcut* titleShortcut2 = new QShortcut(Qt::Key_7 + Qt::KeypadModifier, this);
	connect(titleShortcut1, SIGNAL(activated()), titleStyle, SLOT(click()));
	connect(titleShortcut2, SIGNAL(activated()), titleStyle, SLOT(click()));

	QPushButton* nonprintableTextStyle = new QPushButton(tr("8 Noprintable Text"), this);
	nonprintableTextStyle->setProperty(STYLE_PROPERTY_KEY, ScenarioTextBlockStyle::NoprintableText);
	connect(nonprintableTextStyle, SIGNAL(clicked()), this, SLOT(changeStyle()));
	QShortcut* nonprintableTextShortcut1 = new QShortcut(Qt::Key_8, this);
	QShortcut* nonprintableTextShortcut2 = new QShortcut(Qt::Key_8 + Qt::KeypadModifier, this);
	connect(nonprintableTextShortcut1, SIGNAL(activated()), nonprintableTextStyle, SLOT(click()));
	connect(nonprintableTextShortcut2, SIGNAL(activated()), nonprintableTextStyle, SLOT(click()));

	QPushButton* goToNextBlock = new QPushButton(tr("↓ Next"), this);
	connect(goToNextBlock, SIGNAL(clicked()), this, SLOT(goToNextBlock()));
	QShortcut* goToNextShortcut = new QShortcut(Qt::Key_Down, this);
	connect (goToNextShortcut, SIGNAL(activated()), goToNextBlock, SLOT(click()));

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(goToPrevBlock);
	layout->addSpacing(6);
	layout->addWidget(timeAndPlaceStyle);
	layout->addWidget(actionStyle);
	layout->addWidget(characterStyle);
	layout->addWidget(parentheticalStyle);
	layout->addWidget(dialogStyle);
	layout->addWidget(transitionStyle);
	layout->addWidget(noteStyle);
	layout->addWidget(titleStyle);
	layout->addWidget(nonprintableTextStyle);
	layout->addSpacing(6);
	layout->addWidget(goToNextBlock);
	layout->addStretch();

	setLayout(layout);
}

void ScenarioFastFormatWidget::setEditor(ScenarioTextEdit* _editor)
{
	if (m_editor != _editor) {
		m_editor = _editor;
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

void ScenarioFastFormatWidget::goToNextBlock()
{
	if (m_editor != 0) {
		QTextCursor cursor = m_editor->textCursor();
		cursor.movePosition(QTextCursor::NextBlock);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		m_editor->setTextCursor(cursor);
	}
}

void ScenarioFastFormatWidget::goToPrevBlock()
{
	if (m_editor != 0) {
		QTextCursor cursor = m_editor->textCursor();
		cursor.movePosition(QTextCursor::PreviousBlock);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		m_editor->setTextCursor(cursor);
	}
}

void ScenarioFastFormatWidget::changeStyle()
{
	if (QWidget* button = qobject_cast<QWidget*>(sender())) {
		ScenarioTextBlockStyle::Type type =
				(ScenarioTextBlockStyle::Type)button->property(STYLE_PROPERTY_KEY).toInt();
		if (m_editor != 0) {
			m_editor->changeScenarioBlockType(type);
			selectCurrentBlock();
		}
	}
}
