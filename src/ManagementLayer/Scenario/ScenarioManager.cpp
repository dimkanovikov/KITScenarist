#include "ScenarioManager.h"

#include "ScenarioNavigatorManager.h"
#include "ScenarioTextEditManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QSplitter>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QTextBlock>

using ManagementLayer::ScenarioManager;
using ManagementLayer::ScenarioNavigatorManager;
using ManagementLayer::ScenarioTextEditManager;
using BusinessLogic::ScenarioDocument;
using BusinessLogic::ScenarioTextBlockStyle;


ScenarioManager::ScenarioManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new QWidget(_parentWidget)),
	m_document(new ScenarioDocument(this)),
	m_navigatorManager(new ScenarioNavigatorManager(this, m_view)),
	m_textEditManager(new ScenarioTextEditManager(this, m_view))
{
	initData();
	initView();
	initConnections();
}

QWidget* ScenarioManager::view() const
{
	return m_view;
}

void ScenarioManager::loadCurrentProject()
{
	m_navigatorManager->setNavigationModel(0);
	m_textEditManager->setScenarioDocument(0);

	Domain::Scenario* currentScenario =
			DataStorageLayer::StorageFacade::scenarioStorage()->current();
	m_document->load(currentScenario);

	m_navigatorManager->setNavigationModel(m_document->model());
	m_textEditManager->setScenarioDocument(m_document->document());
}

void ScenarioManager::saveCurrentProject()
{
	DataStorageLayer::StorageFacade::scenarioStorage()->storeScenario(m_document->save());
}

void ScenarioManager::aboutUpdateDuration(int _cursorPosition)
{
	QString durationToCursor =
			ChronometerFacade::secondsToTime(m_document->durationAtPosition(_cursorPosition));
	QString durationToEnd =
			ChronometerFacade::secondsToTime(m_document->fullDuration());
	m_textEditManager->setDuration(
				QString("%1 | %2")
				.arg(durationToCursor)
				.arg(durationToEnd)
				);
}

void ScenarioManager::initData()
{
	m_navigatorManager->setNavigationModel(0);
	m_textEditManager->setScenarioDocument(0);
}

void ScenarioManager::initView()
{
	QSplitter* splitter = new QSplitter(m_view);
	splitter->addWidget(m_navigatorManager->view());
	splitter->addWidget(m_textEditManager->view());
	splitter->setStretchFactor(1, 1);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(splitter);

	m_view->setLayout(layout);
}

void ScenarioManager::initConnections()
{
	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateDuration(int)));
}
