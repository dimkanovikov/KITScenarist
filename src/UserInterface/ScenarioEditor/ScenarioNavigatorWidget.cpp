#include "ScenarioNavigatorWidget.h"

#include <BusinessLogic/ScenarioTextEdit/ScenarioTextEdit.h>
#include <BusinessLogic/ScenarioNavigator/ScenarioNavigator.h>

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>


ScenarioNavigatorWidget::ScenarioNavigatorWidget(QWidget *parent, ScenarioTextEdit* _editor) :
	QFrame(parent),
	m_editor(_editor),
	m_title(0),
	m_scenesCounter(0),
	m_navigator(0)
{
	Q_ASSERT(_editor);

	initView();
	initConnections();
	initStyleSheet();
}

void ScenarioNavigatorWidget::aboutUpdateScenesCount()
{
	scenesCounter()->setText(
				QString("%1: %2")
				.arg(tr("Scenes"))
				.arg(navigator()->scenesCount())
				);
}

void ScenarioNavigatorWidget::initView()
{
	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);

	topLayout->addWidget(title());
	topLayout->addWidget(spacerWidget(0, false, false));
	topLayout->addWidget(scenesCounter());
	topLayout->addWidget(spacerWidget(4, false, true));

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);

	layout->addLayout(topLayout);
	layout->addWidget(navigator());

	this->setLayout(layout);
}

void ScenarioNavigatorWidget::initConnections()
{
	connect(navigator(), SIGNAL(structureChanged()), this, SLOT(aboutUpdateScenesCount()));
}

void ScenarioNavigatorWidget::initStyleSheet()
{
	//
	// Установим для виджетов доп. параметры настройки стиля
	//
	setProperty("mainContainerLeft", true);
	title()->setProperty("topPanelTopBordered", true);
	scenesCounter()->setProperty("topPanelTopBordered", true);
}

ScenarioTextEdit* ScenarioNavigatorWidget::editor()
{
	return m_editor;
}

QLabel* ScenarioNavigatorWidget::title()
{
	if (m_title == 0) {
		m_title = new QLabel(tr("Navigator"), this);
	}
	return m_title;
}

QLabel* ScenarioNavigatorWidget::scenesCounter()
{
	if (m_scenesCounter == 0) {
		m_scenesCounter = new QLabel(this);
		aboutUpdateScenesCount();
	}
	return m_scenesCounter;
}

ScenarioNavigator* ScenarioNavigatorWidget::navigator()
{
	if (m_navigator == 0) {
		m_navigator = new ScenarioNavigator(this, editor());
	}
	return m_navigator;
}

QWidget* ScenarioNavigatorWidget::spacerWidget(int _width, bool _leftBordered, bool _rightBordered) const
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
