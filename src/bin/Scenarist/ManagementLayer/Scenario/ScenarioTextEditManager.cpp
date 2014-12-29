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

void ScenarioTextEditManager::setCountersInfo(const QString& _counters)
{
	m_view->setCountersInfo(_counters);
}

void ScenarioTextEditManager::setCursorPosition(int _position)
{
	m_view->setCursorPosition(_position);
}

void ScenarioTextEditManager::reloadTextEditSettings()
{
	m_view->setUsePageView(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/page-view",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt());

	m_view->setUseSpellChecker(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/spell-checking",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt());
	m_view->setSpellCheckLanguage(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/spell-checking-language",
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
				.toDouble());

	m_view->updateStylesElements();
}

int ScenarioTextEditManager::cursorPosition() const
{
	return m_view->cursorPosition();
}

void ScenarioTextEditManager::addScenarioItem(int _position, const QString& _text, int _type)
{
	m_view->addItem(_position, _text, _type);
}

void ScenarioTextEditManager::removeScenarioText(int _from, int _to)
{
	m_view->removeText(_from, _to);
}

void ScenarioTextEditManager::aboutUndo()
{
	m_view->aboutUndo();
}

void ScenarioTextEditManager::aboutRedo()
{
	m_view->aboutRedo();
}

void ScenarioTextEditManager::aboutTextEditZoomRangeChanged(qreal _zoomRange)
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
	connect(m_view, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
	connect(m_view, SIGNAL(cursorPositionChanged(int)), this, SIGNAL(cursorPositionChanged(int)));
	connect(m_view, SIGNAL(zoomRangeChanged(qreal)), this, SLOT(aboutTextEditZoomRangeChanged(qreal)));
}
