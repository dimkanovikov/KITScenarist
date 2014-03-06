#include "ScenarioManager.h"

#include "ScenarioNavigatorManager.h"
#include "ScenarioTextEditManager.h"

#include <Domain/Character.h>

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>

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
	initView();
	initConnections();
}

QWidget* ScenarioManager::view() const
{
	return m_view;
}

BusinessLogic::ScenarioDocument*ScenarioManager::scenario() const
{
	return m_scenario;
}

void ScenarioManager::loadCurrentProject()
{
	m_navigatorManager->setNavigationModel(0);
	m_textEditManager->setScenarioDocument(0);

	Domain::Scenario* currentScenario =
			DataStorageLayer::StorageFacade::scenarioStorage()->current();
	m_scenario->load(currentScenario);

	m_navigatorManager->setNavigationModel(m_scenario->model());
	m_textEditManager->setScenarioDocument(m_scenario->document());
}

void ScenarioManager::saveCurrentProject()
{
	DataStorageLayer::StorageFacade::scenarioStorage()->storeScenario(m_scenario->save());
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
			cursor.insertText(_newName);
		}
	}
}

void ScenarioManager::refreshCharacters()
{
	//
	// Найти персонажей во всём тексте
	//
	QSet<QString> characters;
	QTextCursor cursor(m_scenario->document());
	while (!cursor.atEnd()) {
		cursor.movePosition(QTextCursor::NextBlock);
		if (ScenarioTextBlockStyle::forBlock(cursor.block()) == ScenarioTextBlockStyle::Character) {
			cursor.select(QTextCursor::BlockUnderCursor);
			characters.insert(cursor.selectedText().toUpper().trimmed());
		}
		cursor.movePosition(QTextCursor::EndOfBlock);
	}

	//
	// Удалить тех, которых нет в тексте
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

void ScenarioManager::aboutUpdateDuration(int _cursorPosition)
{
	QString durationToCursor =
			ChronometerFacade::secondsToTime(m_scenario->durationAtPosition(_cursorPosition));
	QString durationToEnd =
			ChronometerFacade::secondsToTime(m_scenario->fullDuration());
	m_textEditManager->setDuration(
				QString("%1 | %2")
				.arg(durationToCursor)
				.arg(durationToEnd)
				);
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
