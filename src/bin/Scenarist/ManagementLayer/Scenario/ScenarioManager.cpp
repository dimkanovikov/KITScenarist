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
#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/LocationStorage.h>

#include <3rd_party/Widgets/TabBar/TabBar.h>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSplitter>
#include <QTextCursor>
#include <QTextBlock>
#include <QSet>
#include <QStackedWidget>
#include <QWidget>k

using ManagementLayer::ScenarioManager;
using ManagementLayer::ScenarioNavigatorManager;
using ManagementLayer::ScenarioDataEditManager;
using ManagementLayer::ScenarioTextEditManager;
using BusinessLogic::ScenarioDocument;
using BusinessLogic::ScenarioBlockStyle;


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

QString ScenarioManager::scenarioName() const
{
	return m_dataEditManager->scenarioName();
}

void ScenarioManager::loadCurrentProject()
{
	//
	// TODO: Loading progress
	//

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
	m_scenario->load(currentScenario);

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
	// Возврат всех окон в исходное положение - навигатор на верх, данные сценария вперёд
	//
	m_textEditManager->setCursorPosition(0);
}

void ScenarioManager::loadCurrentProjectSettings(const QString& _projectPath)
{
	//
	// Загрузим позицию курсора
	//
	const int lastCursorPosition =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				QString("projects/%1/last-cursor-position").arg(_projectPath),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				).toInt();
	m_textEditManager->setCursorPosition(lastCursorPosition);
}

void ScenarioManager::saveCurrentProject()
{
	QString scenarioName = m_dataEditManager->scenarioName();
	QString scenarioSynopsis = m_dataEditManager->scenarioSynopsis();
	QString scenarioText = m_scenario->save();

	DataStorageLayer::StorageFacade::scenarioStorage()->storeScenario(scenarioName,
		scenarioSynopsis, scenarioText);
}

void ScenarioManager::saveCurrentProjectSettings(const QString& _projectPath)
{
	//
	// Сохраним позицию курсора
	//
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				QString("projects/%1/last-cursor-position").arg(_projectPath),
				QString::number(m_textEditManager->cursorPosition()),
				DataStorageLayer::SettingsStorage::ApplicationSettings);
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
	aboutUpdateDuration(m_textEditManager->cursorPosition());
}

void ScenarioManager::aboutCountersSettingsUpdated()
{
	m_scenario->refresh();
	aboutUpdateCounters();
}

void ScenarioManager::aboutCharacterNameChanged(const QString& _oldName, const QString& _newName)
{
	//
	// Обновить тексты всех сценариев
	//
	QTextCursor cursor(m_scenario->document());
	while (!cursor.isNull() && !cursor.atEnd()) {
		cursor = m_scenario->document()->find(_oldName, cursor);

		if (!cursor.isNull()) {
			//
			// Выделенным должно быть именно имя, а не составная часть другого имени
			//
			bool replaceSelection = false;

			//
			// Если мы в блоке персонажа
			//
			if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::Character) {
				const QString name = BusinessLogic::CharacterParser::name(cursor.block().text());
				if (name == cursor.selectedText()) {
					replaceSelection = true;
				}
			}
			//
			// Если в блоке участники сцены
			//
			else if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneCharacters) {
				const QStringList names = BusinessLogic::SceneCharactersParser::characters(cursor.block().text());
				if (names.contains(cursor.selectedText())) {
					replaceSelection = true;
				}
			}

			//
			// Если выделено имя для замены, меняем его
			//
			if (replaceSelection) {
				cursor.insertText(_newName);
			}
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
		if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::Character) {
			cursor.select(QTextCursor::BlockUnderCursor);
			QString character =
					BusinessLogic::CharacterParser::name(cursor.selectedText().toUpper().trimmed());
			characters.insert(character);
		} else if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneCharacters) {
			cursor.select(QTextCursor::BlockUnderCursor);
			QStringList blockCharacters = cursor.selectedText().split(",", QString::SkipEmptyParts);
			foreach (const QString& characterName, blockCharacters) {
				QString character = BusinessLogic::CharacterParser::name(characterName.toUpper().trimmed());
				characters.insert(character);
			}
		}
		cursor.movePosition(QTextCursor::NextBlock);
	}

	//
	// Определить персонажи, которых нет в тексте
	//
	QSet<QString> charactersToDelete;
	foreach (DomainObject* domainObject,
			 DataStorageLayer::StorageFacade::characterStorage()->all()->toList()) {
		Character* character = dynamic_cast<Character*>(domainObject);
		if (!characters.contains(character->name())) {
			charactersToDelete.insert(character->name());
		}
	}

	//
	// Спросить пользователя, хочет ли он выполнить это действие
	//
	const QStringList deleteList = charactersToDelete.toList();
	const QStringList saveList = characters.toList();
	QString message;
	if (!deleteList.isEmpty()) {
		message.append(QString("<b>%1:</b> %2.").arg(tr("Characters to delete")).arg(deleteList.join(", ")));
	}
	if (!saveList.isEmpty()) {
		if (!message.isEmpty()) {
			message.append("<br/><br/>");
		}
		message.append(QString("<b>%1:</b> %2.").arg(tr("Characters to save")).arg(saveList.join(", ")));
	}
	if (QMessageBox::question(m_view, tr("Apply refreshing"), message) == QMessageBox::Yes) {
		//
		// Удалить тех, кого нет
		//
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
}

void ScenarioManager::aboutLocationNameChanged(const QString& _oldName, const QString& _newName)
{
	//
	// Обновить тексты всех сценариев
	//
	QTextCursor cursor(m_scenario->document());
	while (!cursor.isNull() && !cursor.atEnd()) {
		cursor = m_scenario->document()->find(_oldName, cursor);

		if (!cursor.isNull()) {
			//
			// Выделенным должно быть именно локация, а не составная часть другой локации
			//
			bool replaceSelection = false;

			//
			// Если мы в блоке персонажа
			//
			if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::TimeAndPlace) {
				const QString location = BusinessLogic::TimeAndPlaceParser::locationName(cursor.block().text());
				if (location == cursor.selectedText()) {
					replaceSelection = true;
				}
			}

			//
			// Если выделено имя для замены, меняем его
			//
			if (replaceSelection) {
				cursor.insertText(_newName);
			}
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
		if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::TimeAndPlace) {
			cursor.select(QTextCursor::BlockUnderCursor);
			QString location =
					BusinessLogic::TimeAndPlaceParser::locationName(cursor.selectedText().toUpper().trimmed());
			locations.insert(location);
		}
		cursor.movePosition(QTextCursor::NextBlock);
	}

	//
	// Определить локации, которых нет в тексте
	//
	QSet<QString> locationsToDelete;
	foreach (DomainObject* domainObject,
			 DataStorageLayer::StorageFacade::locationStorage()->all()->toList()) {
		Location* location = dynamic_cast<Location*>(domainObject);
		if (!locations.contains(location->name())) {
			locationsToDelete.insert(location->name());
		}
	}

	//
	// Спросить пользователя, хочет ли он выполнить это действие
	//
	const QStringList deleteList = locationsToDelete.toList();
	const QStringList saveList = locations.toList();
	QString message;
	if (!deleteList.isEmpty()) {
		message.append(QString("<b>%1:</b> %2.").arg(tr("Locations to delete")).arg(deleteList.join(", ")));
	}
	if (!saveList.isEmpty()) {
		if (!message.isEmpty()) {
			message.append("<br/><br/>");
		}
		message.append(QString("<b>%1:</b> %2.").arg(tr("Locations to save")).arg(saveList.join(", ")));
	}

	if (QMessageBox::question(m_view, tr("Apply refreshing"), message) == QMessageBox::Yes) {
		//
		// Удалить те, которых нет
		//
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
}

void ScenarioManager::aboutScenarioNameChanged(const QString& _name)
{
	m_dataEditManager->setScenarioName(_name);
}

void ScenarioManager::aboutUpdateDuration(int _cursorPosition)
{
	QString duration;
	if (BusinessLogic::ChronometerFacade::chronometryUsed()) {
		QString durationToCursor =
				BusinessLogic::ChronometerFacade::secondsToTime(m_scenario->durationAtPosition(_cursorPosition));
		QString durationToEnd =
				BusinessLogic::ChronometerFacade::secondsToTime(m_scenario->fullDuration());
		duration = QString("%1: <b>%2 | %3</b>").arg(tr("Chron.")).arg(durationToCursor).arg(durationToEnd);
	}

	m_textEditManager->setDuration(duration);
}

void ScenarioManager::aboutUpdateCounters()
{
	m_textEditManager->setCountersInfo(m_scenario->countersInfo());
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
	int position = m_scenario->itemStartPosition(_index);
	m_textEditManager->setCursorPosition(position);
}

void ScenarioManager::aboutMoveCursorToItem(int _itemPosition)
{
	m_textEditManager->setCursorPosition(_itemPosition);
}

void ScenarioManager::aboutAddItem(const QModelIndex& _afterItemIndex, const QString& _itemHeader, int _itemType)
{
	int position = m_scenario->itemEndPosition(_afterItemIndex);
	m_textEditManager->addScenarioItem(position, _itemHeader, _itemType);
}

void ScenarioManager::aboutRemoveItems(const QModelIndexList& _indexes)
{
	int from = m_scenario->itemStartPosition(_indexes.first());
	int to = m_scenario->itemEndPosition(_indexes.last());
	m_textEditManager->removeScenarioText(from, to);
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

	int tabIndex = -1;
	tabIndex = m_viewEditorsTabs->addTab(tr("Text"));
	m_viewEditorsTabs->setTabToolTip(tabIndex, tr("Edit Scenario Text"));

	tabIndex = m_viewEditorsTabs->addTab(tr("Data"));
	m_viewEditorsTabs->setTabToolTip(tabIndex, tr("Edit Scenario Data"));

	m_viewEditorsToolbars = new QStackedWidget(m_view);
	m_viewEditorsTabs->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	m_viewEditorsToolbars->addWidget(m_textEditManager->toolbar());
	m_viewEditorsToolbars->addWidget(m_dataEditManager->toolbar());

	m_viewEditors = new QStackedWidget(m_view);
	m_viewEditors->addWidget(m_textEditManager->view());
	m_viewEditors->addWidget(m_dataEditManager->view());

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

	connect(m_navigatorManager, SIGNAL(addItem(QModelIndex,QString,int)), this, SLOT(aboutAddItem(QModelIndex,QString,int)));
	connect(m_navigatorManager, SIGNAL(removeItems(QModelIndexList)), this, SLOT(aboutRemoveItems(QModelIndexList)));
	connect(m_navigatorManager, SIGNAL(sceneChoosed(QModelIndex)), this, SLOT(aboutMoveCursorToItem(QModelIndex)));
	connect(m_navigatorManager, SIGNAL(sceneChoosed(int)), this, SLOT(aboutMoveCursorToItem(int)));
	connect(m_navigatorManager, SIGNAL(undoPressed()), m_textEditManager, SLOT(aboutUndo()));
	connect(m_navigatorManager, SIGNAL(redoPressed()), m_textEditManager, SLOT(aboutRedo()));

	connect(m_sceneSynopsisManager, SIGNAL(synopsisChanged(QString)), this, SLOT(aboutUpdateCurrentSceneSynopsis(QString)));

	connect(m_dataEditManager, SIGNAL(buildSynopsisFromScenes()), this, SLOT(aboutBuildSynopsisFromScenes()));

	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateDuration(int)));
	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateCurrentSynopsis(int)));
	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutSelectItemInNavigator(int)), Qt::QueuedConnection);

	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateCounters()));

	//
	// Настраиваем отслеживание изменений документа
	//
	connect(m_sceneSynopsisManager, SIGNAL(synopsisChanged(QString)), this, SIGNAL(scenarioChanged()));
	connect(m_dataEditManager, SIGNAL(scenarioNameChanged()), this, SIGNAL(scenarioChanged()));
	connect(m_dataEditManager, SIGNAL(scenarioSynopsisChanged()), this, SIGNAL(scenarioChanged()));
	connect(m_textEditManager, SIGNAL(textChanged()), this, SIGNAL(scenarioChanged()));
}

void ScenarioManager::initStyleSheet()
{
	m_viewEditorsTabs->setProperty("inTopPanel", true);
}
