#include "ScenarioTextEditManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEditWidget.h>

using ManagementLayer::ScenarioTextEditManager;
using BusinessLogic::ScenarioDocument;
using UserInterface::ScenarioTextEditWidget;


ScenarioTextEditManager::ScenarioTextEditManager(QObject* _parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new ScenarioTextEditWidget(_parentWidget))
{
	initView();
	initConnections();
	reloadTextEditSettings();
}

QWidget* ScenarioTextEditManager::toolbar() const
{
	return m_view->toolbar();
}

QWidget* ScenarioTextEditManager::view() const
{
	return m_view;
}

void ScenarioTextEditManager::setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document)
{
	m_view->setScenarioDocument(_document);
	reloadTextEditSettings();
}

void ScenarioTextEditManager::setDuration(const QString& _duration)
{
	m_view->setDuration(_duration);
}

void ScenarioTextEditManager::setCursorPosition(int _position)
{
	m_view->setCursorPosition(_position);
}

void ScenarioTextEditManager::reloadTextEditSettings()
{
	m_view->setUseSpellChecker(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/spell-checking",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt());

	m_view->setTextEditColors(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/text-color",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					),
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/background-color",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					)
				);

	m_view->setTextEditZoomRange(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/zoom-range",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt());
}

int ScenarioTextEditManager::cursorPosition() const
{
    return m_view->cursorPosition();
}

void ScenarioTextEditManager::aboutUndo()
{
	m_view->aboutUndo();
}

void ScenarioTextEditManager::aboutRedo()
{
	m_view->aboutRedo();
}

void ScenarioTextEditManager::aboutTextEditZoomRangeChanged(int _zoomRange)
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"scenario-editor/zoom-range",
				QString::number(_zoomRange),
				DataStorageLayer::SettingsStorage::ApplicationSettings);
}

void ScenarioTextEditManager::initView()
{

}

void ScenarioTextEditManager::initConnections()
{
	connect(m_view, SIGNAL(cursorPositionChanged(int)), this, SIGNAL(cursorPositionChanged(int)));
	connect(m_view, SIGNAL(zoomRangeChanged(int)), this, SLOT(aboutTextEditZoomRangeChanged(int)));
}
