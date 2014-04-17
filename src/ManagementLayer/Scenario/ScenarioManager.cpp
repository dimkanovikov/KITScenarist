#include "ScenarioManager.h"

#include "ScenarioNavigatorManager.h"
#include "ScenarioTextEditManager.h"

#include <Domain/Scenario.h>
#include <Domain/Character.h>
#include <Domain/Location.h>

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/LocationStorage.h>

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QSplitter>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QTextBlock>
#include <QSet>

using ManagementLayer::ScenarioManager;
using ManagementLayer::ScenarioNavigatorManager;
using ManagementLayer::ScenarioTextEditManager;
using BusinessLogic::ScenarioDocument;
using BusinessLogic::ScenarioTextBlockStyle;


ScenarioManager::ScenarioManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new QWidget(_parentWidget)),
	m_scenario(new ScenarioDocument(this)),
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

BusinessLogic::ScenarioDocument* ScenarioManager::scenario() const
{
	return m_scenario;
}

void ScenarioManager::loadCurrentProject()
{
	//
	// Очистим от предыдущих данных
	//
	m_navigatorManager->setNavigationModel(0);
	m_textEditManager->setScenarioDocument(0);

	//
	// Загрузим сценарий
	//
	Domain::Scenario* currentScenario =
			DataStorageLayer::StorageFacade::scenarioStorage()->current();
	QString scenarioToLoad;
	if (currentScenario != 0) {
		scenarioToLoad = currentScenario->text();
	}
	m_scenario->load(scenarioToLoad);

	//
	// Установим данные для менеджеров
	//
	m_navigatorManager->setNavigationModel(m_scenario->model());
	m_textEditManager->setScenarioDocument(m_scenario->document());

	//
	// Обновим хронометраж
	//
	aboutUpdateDuration(0);
}

void ScenarioManager::saveCurrentProject()
{
	DataStorageLayer::StorageFacade::scenarioStorage()->storeScenario(m_scenario->save());
}

void ScenarioManager::loadViewState()
{
	m_viewSplitter->restoreGeometry(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/scenario/geometry",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
	m_viewSplitter->restoreState(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/scenario/state",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
}

void ScenarioManager::saveViewState()
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/scenario/geometry", m_viewSplitter->saveGeometry().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/scenario/state", m_viewSplitter->saveState().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
}

void ScenarioManager::aboutTextEditSettingsUpdated()
{
	m_scenario->refresh();
    m_textEditManager->reloadTextEditSettings();
}

void ScenarioManager::aboutNavigatorSettingsUpdated()
{
	m_navigatorManager->reloadNavigatorSettings();
}

void ScenarioManager::aboutChronometrySettingsUpdated()
{
	m_scenario->refresh();
	aboutUpdateDuration(0);
}

void ScenarioManager::aboutCharacterNameChanged(const QString& _oldName, const QString& _newName)
{
	//
	// Обновить тексты всех сценариев
	//
	QTextCursor cursor(m_scenario->document());
	while (!cursor.isNull() && !cursor.atEnd()) {
		cursor = m_scenario->document()->find(_oldName, cursor);

		if (!cursor.isNull()
			&& ScenarioTextBlockStyle::forBlock(cursor.block()) == ScenarioTextBlockStyle::Character) {
			cursor.insertText(_newName);
		}
	}
}

void ScenarioManager::aboutRefreshCharacters()
{
	//
	// Найти персонажей во всём тексте
	//
	QSet<QString> characters;
	QTextCursor cursor(m_scenario->document());
	while (!cursor.atEnd()) {
		cursor.movePosition(QTextCursor::EndOfBlock);
		if (ScenarioTextBlockStyle::forBlock(cursor.block()) == ScenarioTextBlockStyle::Character) {
			cursor.select(QTextCursor::BlockUnderCursor);
			QString character =
					BusinessLogic::CharacterParser::name(cursor.selectedText().toUpper().trimmed());
			characters.insert(character);
		}
		cursor.movePosition(QTextCursor::NextBlock);
	}

	//
	// Удалить те, которых нет в тексте
	//
	QSet<QString> charactersToDelete;
	foreach (DomainObject* domainObject,
			 DataStorageLayer::StorageFacade::characterStorage()->all()->toList()) {
		Character* character = dynamic_cast<Character*>(domainObject);
		if (!characters.contains(character->name())) {
			charactersToDelete.insert(character->name());
		}
	}
	foreach (const QString& character, charactersToDelete) {
		DataStorageLayer::StorageFacade::characterStorage()->removeCharacter(character);
	}

	//
	// Добавить новых
	//
	foreach (const QString& character, characters) {
		if (!DataStorageLayer::StorageFacade::characterStorage()->hasCharacter(character)) {
			DataStorageLayer::StorageFacade::characterStorage()->storeCharacter(character);
		}
	}
}

void ScenarioManager::aboutLocationNameChanged(const QString& _oldName, const QString& _newName)
{
	//
	// Обновить тексты всех сценариев
	//
	QTextCursor cursor(m_scenario->document());
	while (!cursor.isNull() && !cursor.atEnd()) {
		cursor = m_scenario->document()->find(_oldName, cursor);

		if (!cursor.isNull()
			&& ScenarioTextBlockStyle::forBlock(cursor.block()) == ScenarioTextBlockStyle::TimeAndPlace) {
			cursor.insertText(_newName);
		}
	}
}

void ScenarioManager::aboutRefreshLocations()
{
	//
	// Найти локации во всём тексте
	//
	QSet<QString> locations;
	QTextCursor cursor(m_scenario->document());
	while (!cursor.atEnd()) {
		cursor.movePosition(QTextCursor::EndOfBlock);
		if (ScenarioTextBlockStyle::forBlock(cursor.block()) == ScenarioTextBlockStyle::TimeAndPlace) {
			cursor.select(QTextCursor::BlockUnderCursor);
			QString location =
					BusinessLogic::TimeAndPlaceParser::locationName(cursor.selectedText().toUpper().trimmed());
			locations.insert(location);
		}
		cursor.movePosition(QTextCursor::NextBlock);
	}

	//
	// Удалить те, которых нет в тексте
	//
	QSet<QString> locationsToDelete;
	foreach (DomainObject* domainObject,
			 DataStorageLayer::StorageFacade::locationStorage()->all()->toList()) {
		Location* location = dynamic_cast<Location*>(domainObject);
		if (!locations.contains(location->name())) {
			locationsToDelete.insert(location->name());
		}
	}
	foreach (const QString& location, locationsToDelete) {
		DataStorageLayer::StorageFacade::locationStorage()->removeLocation(location);
	}

	//
	// Добавить новых
	//
	foreach (const QString& location, locations) {
		if (!DataStorageLayer::StorageFacade::locationStorage()->hasLocation(location)) {
			DataStorageLayer::StorageFacade::locationStorage()->storeLocation(location);
		}
	}
}

void ScenarioManager::aboutUpdateDuration(int _cursorPosition)
{
	QString durationToCursor =
			BusinessLogic::ChronometerFacade::secondsToTime(m_scenario->durationAtPosition(_cursorPosition));
	QString durationToEnd =
			BusinessLogic::ChronometerFacade::secondsToTime(m_scenario->fullDuration());
	m_textEditManager->setDuration(
				QString("%1 | %2")
				.arg(durationToCursor)
				.arg(durationToEnd)
				);
}

void ScenarioManager::aboutSelectItemInNavigator(int _cursorPosition)
{
	QModelIndex index = m_scenario->itemIndexAtPosition(_cursorPosition);
	m_navigatorManager->setCurrentIndex(index);
}

void ScenarioManager::aboutMoveCursorToItem(const QModelIndex& _index)
{
	int position = m_scenario->itemPositionAtIndex(_index);
	m_textEditManager->setCursorPosition(position);
}

void ScenarioManager::initData()
{
	m_navigatorManager->setNavigationModel(m_scenario->model());
	m_textEditManager->setScenarioDocument(m_scenario->document());
}

void ScenarioManager::initView()
{
	m_viewSplitter = new QSplitter(m_view);
	m_viewSplitter->setHandleWidth(1);
	m_viewSplitter->addWidget(m_navigatorManager->view());
	m_viewSplitter->addWidget(m_textEditManager->view());
	m_viewSplitter->setStretchFactor(1, 1);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(m_viewSplitter);

	m_view->setLayout(layout);
}

void ScenarioManager::initConnections()
{
    connect(m_navigatorManager, SIGNAL(sceneChoosed(QModelIndex)), this, SLOT(aboutMoveCursorToItem(QModelIndex)));
	connect(m_navigatorManager, SIGNAL(undoPressed()), m_textEditManager, SLOT(aboutUndo()));
	connect(m_navigatorManager, SIGNAL(redoPressed()), m_textEditManager, SLOT(aboutRedo()));

	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateDuration(int)));
	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutSelectItemInNavigator(int)), Qt::QueuedConnection);
}
