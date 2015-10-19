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

void ScenarioTextEditManager::setScenarioDocument(BusinessLogic::ScenarioTextDocument* _document, bool _isDraft)
{
	if (m_view->scenarioDocument() != _document) {
		m_view->setScenarioDocument(_document, _isDraft);
		reloadTextEditSettings();
	}
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
	m_view->setShowScenesNumbers(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/show-scenes-numbers",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt());
	m_view->setHighlightCurrentLine(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/highlight-current-line",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toInt());
	m_view->setAutoReplacing(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/auto-replacing",
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

	//
	// Цветовая схема
	//
	const bool useDarkTheme =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"application/use-dark-theme",
				DataStorageLayer::SettingsStorage::ApplicationSettings)
			.toInt();
	const QString colorSuffix = useDarkTheme ? "-dark" : "";
	m_view->setTextEditColors(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/text-color" + colorSuffix,
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					),
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/background-color" + colorSuffix,
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					)
				);

	m_view->setTextEditZoomRange(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/zoom-range",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				.toDouble());

	m_view->updateStylesElements();
	m_view->updateShortcuts();
}

int ScenarioTextEditManager::cursorPosition() const
{
	return m_view->cursorPosition();
}

void ScenarioTextEditManager::setAdditionalCursors(const QMap<QString, int>& _cursors)
{
	m_view->setAdditionalCursors(_cursors);
}

void ScenarioTextEditManager::setCommentOnly(bool _isCommentOnly)
{
	m_view->setCommentOnly(_isCommentOnly);
}

void ScenarioTextEditManager::addScenarioItem(int _position, int _type, const QString& _header,
	const QColor& _color, const QString& _description)
{
	m_view->addItem(_position, _type, _header, _color, _description);
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
