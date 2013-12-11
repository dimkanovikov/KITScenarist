#include "widget.h"

#include <QWidget>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h"
#include <QTextCodec>

namespace {
	const char* TYPE_PROPERTY = "style_property";
}

Widget::Widget(QWidget *parent) :
	QWidget(parent)
{

	QRadioButton* cbSceneHeading = new QRadioButton("Scene Heading");
	cbSceneHeading->setProperty(TYPE_PROPERTY, ScenarioTextBlockStyle::TimeAndPlace);
	connect(cbSceneHeading,SIGNAL(clicked()), this, SLOT(setStyle()));

	QRadioButton* cbAction = new QRadioButton("Action");
	cbAction->setProperty(TYPE_PROPERTY, ScenarioTextBlockStyle::Action);
	connect(cbAction,SIGNAL(clicked()), this, SLOT(setStyle()));

	QRadioButton* cbCharacter = new QRadioButton("Character");
	cbCharacter->setProperty(TYPE_PROPERTY, ScenarioTextBlockStyle::Character);
	connect(cbCharacter,SIGNAL(clicked()), this, SLOT(setStyle()));

	QRadioButton* cbDialog = new QRadioButton("Dialog");
	cbDialog->setProperty(TYPE_PROPERTY, ScenarioTextBlockStyle::Dialog);
	connect(cbDialog,SIGNAL(clicked()), this, SLOT(setStyle()));

	QRadioButton* cbParethentcial = new QRadioButton("Parethentcial");
	cbParethentcial->setProperty(TYPE_PROPERTY, ScenarioTextBlockStyle::Parenthetical);
	connect(cbParethentcial,SIGNAL(clicked()), this, SLOT(setStyle()));

	QRadioButton* cbTitle = new QRadioButton("Title");
	cbTitle->setProperty(TYPE_PROPERTY, ScenarioTextBlockStyle::Title);
	connect(cbTitle,SIGNAL(clicked()), this, SLOT(setStyle()));

	QRadioButton* cbNote = new QRadioButton("Note");
	cbNote->setProperty(TYPE_PROPERTY, ScenarioTextBlockStyle::Note);
	connect(cbNote,SIGNAL(clicked()), this, SLOT(setStyle()));

	QRadioButton* cbTransition = new QRadioButton("Transition");
	cbTransition->setProperty(TYPE_PROPERTY, ScenarioTextBlockStyle::Transition);
	connect(cbTransition,SIGNAL(clicked()), this, SLOT(setStyle()));

	QRadioButton* cbSimpleText = new QRadioButton("Simple Text");
	cbSimpleText->setProperty(TYPE_PROPERTY, ScenarioTextBlockStyle::SimpleText);
	connect(cbSimpleText,SIGNAL(clicked()), this, SLOT(setStyle()));

	// ****

	QPushButton* testBtn = new QPushButton("Test");

	QVBoxLayout* stylesLayout = new QVBoxLayout;
	stylesLayout->addWidget(cbSceneHeading);
	stylesLayout->addWidget(cbAction);
	stylesLayout->addWidget(cbCharacter);
	stylesLayout->addWidget(cbDialog);
	stylesLayout->addWidget(cbParethentcial);
	stylesLayout->addWidget(cbTitle);
	stylesLayout->addWidget(cbNote);
	stylesLayout->addWidget(cbTransition);
	stylesLayout->addWidget(cbSimpleText);
	stylesLayout->addWidget(testBtn);
	stylesLayout->addStretch();

	m_screenEdit = new ScenarioTextEdit;
	connect(m_screenEdit, SIGNAL(currentStyleChanged()), this, SLOT(styleChanged()));
	connect(testBtn, SIGNAL(clicked()), m_screenEdit, SLOT(test()));

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addLayout(stylesLayout);
	layout->addWidget(m_screenEdit);


	setLayout(layout);
}

void Widget::exec()
{
	resize(600,400);

	m_screenEdit->setScenarioBlockType(ScenarioTextBlockStyle::Action);
	m_screenEdit->setScenarioBlockType(ScenarioTextBlockStyle::TimeAndPlace);
	m_screenEdit->setFocus();

	show();
}

void Widget::setStyle()
{
	if (QRadioButton* rb = qobject_cast<QRadioButton*>(sender())) {
		if (rb->isChecked()) {
			ScenarioTextBlockStyle::Type type =
					(ScenarioTextBlockStyle::Type)rb->property(TYPE_PROPERTY).toInt();
			m_screenEdit->setScenarioBlockType(type);
			m_screenEdit->setFocus();
		}
	}
}

void Widget::styleChanged()
{
	ScenarioTextBlockStyle::Type currentType = m_screenEdit->scenarioBlockType();
	if (currentType == ScenarioTextBlockStyle::TitleHeader) {
		currentType = ScenarioTextBlockStyle::Title;
	}

	foreach (QObject* child, children()) {
		if (QRadioButton* rb = qobject_cast<QRadioButton*>(child)) {
			ScenarioTextBlockStyle::Type rbType =
					(ScenarioTextBlockStyle::Type)rb->property(TYPE_PROPERTY).toInt();
			if (rbType == currentType) {
				rb->setChecked(true);
				break;
			}
		}
	}
}
