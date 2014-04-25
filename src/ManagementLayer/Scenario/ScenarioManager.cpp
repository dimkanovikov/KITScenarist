#include "ScenarioManager.h"

#include "ScenarioNavigatorManager.h"
#include "ScenarioSceneSynopsisManager.h"
#include "ScenarioDataEditManager.h"
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

#include <3rd_party/Widgets/TabBar/TabBar.h>

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QSplitter>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QTextBlock>
#include <QSet>
#include <QStackedWidget>

using ManagementLayer::ScenarioManager;
using ManagementLayer::ScenarioNavigatorManager;
using ManagementLayer::ScenarioDataEditManager;
using ManagementLayer::ScenarioTextEditManager;
using BusinessLogic::ScenarioDocument;
using BusinessLogic::ScenarioTextBlockStyle;


ScenarioManager::ScenarioManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new QWidget(_parentWidget)),
	m_mainViewSplitter(new QSplitter(m_view)),
	m_leftViewSplitter(new QSplitter(m_view)),
	m_scenario(new ScenarioDocument(this)),
	m_navigatorManager(new ScenarioNavigatorManager(this, m_view)),
	m_sceneSynopsisManager(new ScenarioSceneSynopsisManager(this, m_view)),
	m_dataEditManager(new ScenarioDataEditManager(this, m_view)),
	m_textEditManager(new ScenarioTextEditManager(this, m_view))
{
	initData();
	initView();
	initConnections();
	initStyleSheet();
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
	m_dataEditManager->clear();
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
	if (currentScenario != 0) {
		m_dataEditManager->setScenarioName(currentScenario->name());
		m_dataEditManager->setScenarioSynopsis(currentScenario->synopsis());
	}
	m_textEditManager->setScenarioDocument(m_scenario->document());

	//
	// FIXME: Обновление всех счётчиков программы - хронометраж, кол-во сцен
	//        Возврат всех окон в исходное положение - навигатор на верх, данные сценария вперёд
	//
	m_textEditManager->setCursorPosition(0);
}

void ScenarioManager::saveCurrentProject()
{
	QString scenarioName = m_dataEditManager->scenarioName();
	QString scenarioSynopsis = m_dataEditManager->scenarioSynopsis();
	QString scenarioText = m_scenario->save();

	DataStorageLayer::StorageFacade::scenarioStorage()->storeScenario(
				scenarioName,
				scenarioSynopsis,
				scenarioText);
}

void ScenarioManager::loadViewState()
{
	m_leftViewSplitter->restoreGeometry(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/scenario/left/geometry",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
	m_leftViewSplitter->restoreState(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/scenario/left/state",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);

	m_mainViewSplitter->restoreGeometry(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/scenario/geometry",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
	m_mainViewSplitter->restoreState(
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
				"application/scenario/left/geometry", m_leftViewSplitter->saveGeometry().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/scenario/left/state", m_leftViewSplitter->saveState().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);

	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/scenario/geometry", m_mainViewSplitter->saveGeometry().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/scenario/state", m_mainViewSplitter->saveState().toHex(),
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

void ScenarioManager::aboutScenarioChanged()
{
	//
	// Проходим по иерархии виджетов до самого первого
	//
	QWidget* topWidget = m_view;
	while (topWidget->parentWidget() != 0) {
		topWidget = topWidget->parentWidget();
	}
	//
	// и устанавливаем ему флаг об изменении документа
	//
	topWidget->setWindowModified(true);
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

void ScenarioManager::aboutUpdateCurrentSynopsis(int _cursorPosition)
{
	QString itemHeader = m_scenario->itemHeaderAtPosition(_cursorPosition);
	m_sceneSynopsisManager->setHeader(itemHeader);

	QString synopsis = m_scenario->itemSynopsisAtPosition(_cursorPosition);
	m_sceneSynopsisManager->setSynopsis(synopsis);
}

void ScenarioManager::aboutUpdateCurrentSceneSynopsis(const QString& _synopsis)
{
	m_scenario->setItemSynopsisAtPosition(m_textEditManager->cursorPosition(), _synopsis);
}

void ScenarioManager::aboutBuildSynopsisFromScenes()
{
	QString synopsis = m_scenario->builSynopsisFromScenes();
	m_dataEditManager->setScenarioSynopsisFromScenes(synopsis);
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
    m_viewEditorsTabs = new TabBar(m_view);
	m_viewEditorsTabs->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	m_viewEditorsTabs->addTab(tr("Data"));
	m_viewEditorsTabs->addTab(tr("Text"));
	m_viewEditorsTabs->setMinimumSize(100,0);

	m_viewEditorsToolbars = new QStackedWidget(m_view);
	m_viewEditorsTabs->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	m_viewEditorsToolbars->addWidget(m_dataEditManager->toolbar());
	m_viewEditorsToolbars->addWidget(m_textEditManager->toolbar());

	m_viewEditors = new QStackedWidget(m_view);
	m_viewEditors->addWidget(m_dataEditManager->view());
	m_viewEditors->addWidget(m_textEditManager->view());

	QWidget* rightWidget = new QWidget(m_view);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(m_viewEditorsToolbars);
	topLayout->addWidget(m_viewEditorsTabs);
	QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
	rightLayout->setContentsMargins(QMargins());
	rightLayout->setSpacing(0);
	rightLayout->addLayout(topLayout);
	rightLayout->addWidget(m_viewEditors, 1);

	m_leftViewSplitter->setHandleWidth(1);
	m_leftViewSplitter->addWidget(m_navigatorManager->view());
	m_leftViewSplitter->addWidget(m_sceneSynopsisManager->view());
	m_leftViewSplitter->setOrientation(Qt::Vertical);

	m_mainViewSplitter->setHandleWidth(1);
	m_mainViewSplitter->addWidget(m_leftViewSplitter);
	m_mainViewSplitter->addWidget(rightWidget);
	m_mainViewSplitter->setStretchFactor(1, 1);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	layout->addWidget(m_mainViewSplitter);

	m_view->setLayout(layout);
}

void ScenarioManager::initConnections()
{
	connect(m_viewEditorsTabs, SIGNAL(currentChanged(int)), m_viewEditorsToolbars, SLOT(setCurrentIndex(int)));
	connect(m_viewEditorsTabs, SIGNAL(currentChanged(int)), m_viewEditors, SLOT(setCurrentIndex(int)));

    connect(m_navigatorManager, SIGNAL(sceneChoosed(QModelIndex)), this, SLOT(aboutMoveCursorToItem(QModelIndex)));
	connect(m_navigatorManager, SIGNAL(undoPressed()), m_textEditManager, SLOT(aboutUndo()));
	connect(m_navigatorManager, SIGNAL(redoPressed()), m_textEditManager, SLOT(aboutRedo()));

	connect(m_sceneSynopsisManager, SIGNAL(synopsisChanged(QString)), this, SLOT(aboutUpdateCurrentSceneSynopsis(QString)));

	connect(m_dataEditManager, SIGNAL(buildSynopsisFromScenes()), this, SLOT(aboutBuildSynopsisFromScenes()));

	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateDuration(int)));
	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateCurrentSynopsis(int)));
	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutSelectItemInNavigator(int)), Qt::QueuedConnection);

	//
	// Настраиваем отслеживание изменений документа
	//
	connect(m_sceneSynopsisManager, SIGNAL(synopsisChanged(QString)), this, SLOT(aboutScenarioChanged()));
	connect(m_dataEditManager, SIGNAL(scenarioNameChanged()), this, SLOT(aboutScenarioChanged()));
	connect(m_dataEditManager, SIGNAL(scenarioSynopsisChanged()), this, SLOT(aboutScenarioChanged()));
	connect(m_textEditManager, SIGNAL(textChanged()), this, SLOT(aboutScenarioChanged()));
}

void ScenarioManager::initStyleSheet()
{
	m_viewEditorsTabs->setProperty("inTopPanel", true);
}
