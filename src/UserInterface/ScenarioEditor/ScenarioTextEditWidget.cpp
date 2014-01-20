#include "ScenarioTextEditWidget.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>
#include <BusinessLogic/Chronometry/ChronometerFacade.h>

#include <QComboBox>
#include <QLabel>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QVBoxLayout>


ScenarioTextEditWidget::ScenarioTextEditWidget(QWidget *parent) :
	QFrame(parent),
	m_types(0),
	m_textMode(0),
	m_chron(0),
	m_scenarioEdit(0)
{
	initView();
	initConnections();
	initStyleSheet();
}

ScenarioTextEdit* ScenarioTextEditWidget::editor()
{
	return scenarioEdit();
}

void ScenarioTextEditWidget::aboutUpdateStyle()
{
	ScenarioTextBlockStyle::Type currentType = scenarioEdit()->scenarioBlockType();
	if (currentType == ScenarioTextBlockStyle::TitleHeader) {
		currentType = ScenarioTextBlockStyle::Title;
	} else if (currentType == ScenarioTextBlockStyle::SceneGroupFooter) {
		currentType = ScenarioTextBlockStyle::SceneGroupHeader;
	} else if (currentType == ScenarioTextBlockStyle::FolderFooter) {
		currentType = ScenarioTextBlockStyle::FolderHeader;
	}

	for (int itemIndex = 0; itemIndex < types()->count(); ++itemIndex) {
		ScenarioTextBlockStyle::Type itemType =
				(ScenarioTextBlockStyle::Type)types()->itemData(itemIndex).toInt();
		if (itemType == currentType) {
			types()->setCurrentIndex(itemIndex);
			break;
		}
	}
}

void ScenarioTextEditWidget::aboutSetStyle()
{
	ScenarioTextBlockStyle::Type type =
			(ScenarioTextBlockStyle::Type)types()->itemData(types()->currentIndex()).toInt();

	//
	// Меняем стиль блока, если это возможно
	//
	if (type != scenarioEdit()->scenarioBlockType()) {
		m_scenarioEdit->changeScenarioBlockType(type);
		m_scenarioEdit->setFocus();
	}
}

void ScenarioTextEditWidget::aboutUpdateChronometry()
{
	int chronForCursor =
			ChronometerFacade::calculate(
				scenarioEdit()->document(),
				0,
				scenarioEdit()->textCursor().position());
	int chronForAll =
			ChronometerFacade::calculate(
				scenarioEdit()->document(),
				0,
				scenarioEdit()->document()->characterCount() - 1);

	chron()->setText(
				QString("%1: %2 | %3")
				.arg(tr("Chron"))
				.arg(ChronometerFacade::secondsToTime(chronForCursor))
				.arg(ChronometerFacade::secondsToTime(chronForAll))
				);
}

void ScenarioTextEditWidget::initView()
{
	//
	// Верхняя панель
	//
	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);

	topLayout->addWidget(types());
	topLayout->addWidget(spacerWidget(10, true, false));
	topLayout->addWidget(textMode());
	topLayout->addWidget(spacerWidget(0, false, false));
	topLayout->addWidget(chron());
	topLayout->addWidget(spacerWidget(10, false, false));

	//
	// Собираем всё вместе
	//
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);

	layout->addLayout(topLayout);
	layout->addWidget(scenarioEdit());

	//
	// Применяем компоновку
	//
	this->setLayout(layout);
}

void ScenarioTextEditWidget::initConnections()
{
	connect(types(), SIGNAL(currentIndexChanged(int)), this, SLOT(aboutSetStyle()));
	connect(scenarioEdit(), SIGNAL(currentStyleChanged()), this, SLOT(aboutUpdateStyle()));
	connect(scenarioEdit(), SIGNAL(currentStyleChanged()), this, SLOT(aboutUpdateChronometry()));
}

void ScenarioTextEditWidget::initStyleSheet()
{
	//
	// Установим для виджетов доп. параметры настройки стиля
	//
	setProperty("mainContainerRight", true);
	types()->setProperty("topPanelRightBordered", true);
	types()->setProperty("topPanelTopBordered", true);
	textMode()->setProperty("topPanelTopBordered", true);
	chron()->setProperty("topPanelTopBordered", true);
}

QComboBox* ScenarioTextEditWidget::types()
{
	if (m_types == 0) {
		m_types = new QComboBox(this);
		m_types->setSizePolicy(m_types->sizePolicy().horizontalPolicy(), QSizePolicy::Preferred);
		m_types->addItem(tr("Time and Place"), ScenarioTextBlockStyle::TimeAndPlace);
		m_types->addItem(tr("Action"), ScenarioTextBlockStyle::Action);
		m_types->addItem(tr("Character"), ScenarioTextBlockStyle::Character);
		m_types->addItem(tr("Dialog"), ScenarioTextBlockStyle::Dialog);
		m_types->addItem(tr("Parethentcial"), ScenarioTextBlockStyle::Parenthetical);
		m_types->addItem(tr("Title"), ScenarioTextBlockStyle::Title);
		m_types->addItem(tr("Note"), ScenarioTextBlockStyle::Note);
		m_types->addItem(tr("Transition"), ScenarioTextBlockStyle::Transition);
		m_types->addItem(tr("Noprintable Text"), ScenarioTextBlockStyle::NoprintableText);
		m_types->addItem(tr("Scenes Group"), ScenarioTextBlockStyle::SceneGroupHeader);
		m_types->addItem(tr("Folder"), ScenarioTextBlockStyle::FolderHeader);
	}
	return m_types;
}

QRadioButton* ScenarioTextEditWidget::textMode()
{
	if (m_textMode == 0) {
		m_textMode = new QRadioButton(tr("Text"), this);
		m_textMode->setSizePolicy(m_textMode->sizePolicy().horizontalPolicy(), QSizePolicy::Preferred);
		m_textMode->setChecked(true);
		m_textMode->setVisible(false);
	}
	return m_textMode;
}

QLabel* ScenarioTextEditWidget::chron()
{
	if (m_chron == 0) {
		m_chron = new QLabel(this);
		aboutUpdateChronometry();
	}
	return m_chron;
}

ScenarioTextEdit* ScenarioTextEditWidget::scenarioEdit()
{
	if (m_scenarioEdit == 0) {
		m_scenarioEdit = new ScenarioTextEdit(this);
	}
	return m_scenarioEdit;
}

QWidget* ScenarioTextEditWidget::spacerWidget(int _width, bool _leftBordered, bool _rightBordered) const
{
	QWidget* widget = new QWidget;
	if (_width > 0) {
		widget->setMinimumWidth(_width);
		widget->setMaximumWidth(_width);
		widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	} else {
		widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	}
	widget->setProperty("topPanelTopBordered", true);

	if (_leftBordered) {
		widget->setProperty("topPanelLeftBordered", true);
	}

	if (_rightBordered) {
		widget->setProperty("topPanelRightBordered", true);
	}

	return widget;
}
