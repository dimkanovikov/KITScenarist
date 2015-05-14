#include "ScenarioTextEditPrivate.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/ShortcutHelper.h>

#include <QShortcut>
#include <QSignalMapper>

using UserInterface::ShortcutsManager;
using BusinessLogic::ScenarioBlockStyle;


ShortcutsManager::ShortcutsManager(UserInterface::ScenarioTextEdit* _editor) :
	QObject(_editor),
	m_editor(_editor)
{
	Q_ASSERT(_editor);

	//
	// Создаём шорткаты
	//
	m_shortcuts.insert(ScenarioBlockStyle::TimeAndPlace, new QShortcut(_editor));
	m_shortcuts.insert(ScenarioBlockStyle::SceneCharacters, new QShortcut(_editor));
	m_shortcuts.insert(ScenarioBlockStyle::Action, new QShortcut(_editor));
	m_shortcuts.insert(ScenarioBlockStyle::Character, new QShortcut(_editor));
	m_shortcuts.insert(ScenarioBlockStyle::Parenthetical, new QShortcut(_editor));
	m_shortcuts.insert(ScenarioBlockStyle::Dialog, new QShortcut(_editor));
	m_shortcuts.insert(ScenarioBlockStyle::Transition, new QShortcut(_editor));
	m_shortcuts.insert(ScenarioBlockStyle::Note, new QShortcut(_editor));
	m_shortcuts.insert(ScenarioBlockStyle::Title, new QShortcut(_editor));
	m_shortcuts.insert(ScenarioBlockStyle::SceneGroupHeader, new QShortcut(_editor));
	m_shortcuts.insert(ScenarioBlockStyle::FolderHeader, new QShortcut(_editor));

	//
	// Настраиваем их
	//
	update();
	QSignalMapper* mapper = new QSignalMapper(this);
	foreach (int type, m_shortcuts.keys()) {
		QShortcut* shortcut = m_shortcuts.value(type);
		connect(shortcut, SIGNAL(activated()), mapper, SLOT(map()));
		mapper->setMapping(shortcut, type);
	}
	connect(mapper, SIGNAL(mapped(int)), this, SLOT(changeTextBlock(int)));
}

void ShortcutsManager::update()
{
	//
	// Обновим сочетания клавиш для всех блоков
	//
	foreach (int type, m_shortcuts.keys()) {
		ScenarioBlockStyle::Type blockType = (ScenarioBlockStyle::Type)type;
		const QString typeShortName = ScenarioBlockStyle::typeName(blockType);
		const QString keySequence =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					QString("scenario-editor/shortcuts/%1").arg(typeShortName),
					DataStorageLayer::SettingsStorage::ApplicationSettings
					);

		QShortcut* shortcut = m_shortcuts.value(type);
		shortcut->setKey(QKeySequence(keySequence));
	}
}

QString ShortcutsManager::shortcut(int _forBlockType) const
{
	QString result;
	if (m_shortcuts.contains(_forBlockType)) {
		result = ShortcutHelper::makeShortcut(m_shortcuts.value(_forBlockType)->key().toString());
	}
	return result;
}

void ShortcutsManager::changeTextBlock(int _blockType) const
{
	if (!m_editor->isReadOnly()) {
		ScenarioBlockStyle::Type blockType = (ScenarioBlockStyle::Type)_blockType;
		m_editor->changeScenarioBlockType(blockType);
	}
}
