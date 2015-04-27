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
#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <DataLayer/Database/Database.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>
#include <DataLayer/DataStorageLayer/ScenarioChangeStorage.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/LocationStorage.h>

#include <3rd_party/Helpers/DiffMatchPatch.h>
#include <3rd_party/Helpers/ShortcutHelper.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/TabBar/TabBar.h>

#include <QComboBox>
#include <QCryptographicHash>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSplitter>
#include <QTextCursor>
#include <QTextBlock>
#include <QTimer>
#include <QSet>
#include <QStackedWidget>
#include <QWidget>

using ManagementLayer::ScenarioManager;
using ManagementLayer::ScenarioNavigatorManager;
using ManagementLayer::ScenarioSceneSynopsisManager;
using ManagementLayer::ScenarioDataEditManager;
using ManagementLayer::ScenarioTextEditManager;
using BusinessLogic::ScenarioDocument;
using BusinessLogic::ScenarioBlockStyle;

namespace {

	/**
	 * @brief Ключ для хранения атрибута последнего размера сплитера
	 */
	const char* SPLITTER_LAST_SIZES = "last_sizes";

	/**
	 * @brief Ключ для доступа к черновику сценария
	 */
	const bool IS_DRAFT = true;

	/**
	 * @brief Обновить текст сценария для нового имени персонажа
	 */
	static void updateScenarioForNewCharacterName(ScenarioDocument* _scenario,
		const QString _oldName, const QString& _newName) {

		QTextCursor cursor(_scenario->document());
		while (!cursor.isNull() && !cursor.atEnd()) {
			cursor = _scenario->document()->find(_oldName, cursor);

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
						//
						// Убедимся, что выделено именно имя, а не часть другого имени
						//
						QTextCursor checkCursor(cursor);
						// ... всё ли в порядке слева
						bool atLeftAllOk = false;
						checkCursor.setPosition(cursor.selectionStart());
						if (checkCursor.atBlockStart()) {
							atLeftAllOk = true;
						} else {
							checkCursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
							if (checkCursor.selectedText() == " "
								|| checkCursor.selectedText() == ",") {
								atLeftAllOk = true;
							} else {
								atLeftAllOk = false;
							}
						}
						// ... всё ли в порядке справа
						bool atRightAllOk = false;
						checkCursor.setPosition(cursor.selectionEnd());
						if (checkCursor.atBlockEnd()) {
							atRightAllOk = true;
						} else {
							checkCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
							if (checkCursor.selectedText() == " "
								|| checkCursor.selectedText() == ",") {
								atRightAllOk = true;
							} else {
								atRightAllOk = false;
							}
						}
						// ... если со всех сторон всё в порядке - заменяем
						if (atLeftAllOk && atRightAllOk) {
							replaceSelection = true;
						}
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

	static void updateScenarioForNewLocationName(ScenarioDocument* _scenario,
		const QString& _oldName, const QString& _newName) {

		QTextCursor cursor(_scenario->document());
		while (!cursor.isNull() && !cursor.atEnd()) {
			cursor = _scenario->document()->find(_oldName, cursor);

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

	/**
	 * @brief Получить хэш текста
	 */
	static QByteArray textMd5Hash(const QString& _text) {
		QCryptographicHash hash(QCryptographicHash::Md5);
		hash.addData(_text.toUtf8());
		return hash.result();
	}
}


ScenarioManager::ScenarioManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_view(new QWidget(_parentWidget)),
	m_mainViewSplitter(new QSplitter(m_view)),
	m_draftViewSplitter(new QSplitter(m_view)),
	m_noteViewSplitter(new QSplitter(m_view)),
	m_scenario(new ScenarioDocument(this)),
	m_scenarioDraft(new ScenarioDocument(this)),
	m_navigatorManager(new ScenarioNavigatorManager(this, m_view)),
	m_draftNavigatorManager(new ScenarioNavigatorManager(this, m_view, IS_DRAFT)),
	m_sceneSynopsisManager(new ScenarioSceneSynopsisManager(this, m_view)),
	m_dataEditManager(new ScenarioDataEditManager(this, m_view)),
	m_textEditManager(new ScenarioTextEditManager(this, m_view)),
	m_workModeIsDraft(false)
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

BusinessLogic::ScenarioDocument*ScenarioManager::scenarioDraft() const
{
	return m_scenarioDraft;
}

QString ScenarioManager::scenarioName() const
{
	return m_dataEditManager->scenarioName();
}

int ScenarioManager::cursorPosition() const
{
	return m_textEditManager->cursorPosition();
}

void ScenarioManager::loadCurrentProject()
{
	//
	// Очистим от предыдущих данных
	//
	m_navigatorManager->setNavigationModel(0);
	m_draftNavigatorManager->setNavigationModel(0);
	m_dataEditManager->clear();
	m_textEditManager->setScenarioDocument(0);

	//
	// Очистим сценарий
	//
	m_scenario->clear();
	m_scenarioDraft->clear();

	//
	// Загрузим сценарий
	//
	// ... чистовик
	//
	Domain::Scenario* currentScenario =
			DataStorageLayer::StorageFacade::scenarioStorage()->current();
	m_scenario->load(currentScenario);
	m_lastScenarioXml = currentScenario->text();
	m_lastScenarioXmlHash = ::textMd5Hash(m_lastScenarioXml);
	//
	// ... черновик
	//
	Domain::Scenario* currentScenarioDraft =
			DataStorageLayer::StorageFacade::scenarioStorage()->current(IS_DRAFT);
	m_scenarioDraft->load(currentScenarioDraft);
	m_lastScenarioDraftXml = currentScenarioDraft->text();
	m_lastScenarioDraftXmlHash = ::textMd5Hash(m_lastScenarioDraftXml);

	//
	// Установим данные для менеджеров
	//
	m_navigatorManager->setNavigationModel(m_scenario->model());
	m_draftNavigatorManager->setNavigationModel(m_scenarioDraft->model());
	if (currentScenario != 0) {
		m_dataEditManager->setScenarioName(currentScenario->name());
		m_dataEditManager->setScenarioSynopsis(currentScenario->synopsis());
	}
	m_textEditManager->setScenarioDocument(m_scenarioDraft->document(), IS_DRAFT);
	m_textEditManager->setScenarioDocument(m_scenario->document());

	//
	// Возврат всех окон в исходное положение - навигатор на верх, данные сценария вперёд
	//
	m_textEditManager->setCursorPosition(0);

	//
	// Обновим счётчики, когда данные полностью загрузятся
	//
	QTimer::singleShot(100, this, SLOT(aboutUpdateCounters()));

	//
	// Запускаем таймер сохранения изменений
	//
	const int SAVE_CHANGES_INTERVAL = 5000;
	m_saveChangesTimer.start(SAVE_CHANGES_INTERVAL);
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
	//
	// Сохраняем сценарий
	//
	m_scenario->scenario()->setName(m_dataEditManager->scenarioName());
	m_scenario->scenario()->setSynopsis(m_dataEditManager->scenarioSynopsis());
	m_scenario->scenario()->setText(m_scenario->save());
	DataStorageLayer::StorageFacade::scenarioStorage()->storeScenario(m_scenario->scenario());

	//
	// Сохраняем черновик
	//
	m_scenarioDraft->scenario()->setText(m_scenarioDraft->save());
	DataStorageLayer::StorageFacade::scenarioStorage()->storeScenario(m_scenarioDraft->scenario());

	//
	// Сохраняем изменения
	//
	aboutSaveChanges();
	DataStorageLayer::StorageFacade::scenarioChangeStorage()->store();
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
	m_draftViewSplitter->restoreGeometry(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/scenario/draft/geometry",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
	m_draftViewSplitter->restoreState(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/scenario/draft/state",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
	const bool draftInvisible = m_draftViewSplitter->sizes().last() == 0;
	m_navigatorManager->setDraftVisible(!draftInvisible);

	m_noteViewSplitter->restoreGeometry(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/scenario/left/geometry",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
	m_noteViewSplitter->restoreState(
				QByteArray::fromHex(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
					"application/scenario/left/state",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
					.toUtf8()
					)
				);
	const bool noteInvisible = m_noteViewSplitter->sizes().last() == 0;
	m_navigatorManager->setNoteVisible(!noteInvisible);

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

	if (m_mainViewSplitter->opaqueResize()) {
		m_mainViewSplitter->setOpaqueResize(false);
	}
}

void ScenarioManager::saveViewState()
{
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/scenario/draft/geometry", m_draftViewSplitter->saveGeometry().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/scenario/draft/state", m_draftViewSplitter->saveState().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);

	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/scenario/left/geometry", m_noteViewSplitter->saveGeometry().toHex(),
				DataStorageLayer::SettingsStorage::ApplicationSettings
				);
	DataStorageLayer::StorageFacade::settingsStorage()->setValue(
				"application/scenario/left/state", m_noteViewSplitter->saveState().toHex(),
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
	m_scenarioDraft->refresh();
	m_textEditManager->reloadTextEditSettings();
}

void ScenarioManager::aboutNavigatorSettingsUpdated()
{
	m_navigatorManager->reloadNavigatorSettings();
	m_draftNavigatorManager->reloadNavigatorSettings();
}

void ScenarioManager::aboutChronometrySettingsUpdated()
{
	m_scenario->refresh();
	m_scenarioDraft->refresh();
	aboutUpdateDuration(m_textEditManager->cursorPosition());
}

void ScenarioManager::aboutCountersSettingsUpdated()
{
	m_scenario->refresh();
	m_scenarioDraft->refresh();
	aboutUpdateCounters();
}

void ScenarioManager::aboutCharacterNameChanged(const QString& _oldName, const QString& _newName)
{
	//
	// Обновить тексты всех сценариев
	//
	::updateScenarioForNewCharacterName(m_scenario, _oldName, _newName);
	::updateScenarioForNewCharacterName(m_scenarioDraft, _oldName, _newName);
}

void ScenarioManager::aboutRefreshCharacters()
{
	//
	// Найти персонажей во всём тексте
	//
	QSet<QString> characters = QSet<QString>::fromList(m_scenario->findCharacters());
	characters.unite(QSet<QString>::fromList(m_scenarioDraft->findCharacters()));

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
		DatabaseLayer::Database::transaction();
		foreach (const QString& character, charactersToDelete) {
			DataStorageLayer::StorageFacade::characterStorage()->removeCharacter(character);
		}
		DatabaseLayer::Database::commit();

		//
		// Добавить новых
		//
		DatabaseLayer::Database::transaction();
		foreach (const QString& character, characters) {
			if (!DataStorageLayer::StorageFacade::characterStorage()->hasCharacter(character)) {
				DataStorageLayer::StorageFacade::characterStorage()->storeCharacter(character);
			}
		}
		DatabaseLayer::Database::commit();
	}
}

void ScenarioManager::aboutLocationNameChanged(const QString& _oldName, const QString& _newName)
{
	//
	// Обновить тексты всех сценариев
	//
	::updateScenarioForNewLocationName(m_scenario, _oldName, _newName);
	::updateScenarioForNewLocationName(m_scenarioDraft, _oldName, _newName);
}

void ScenarioManager::aboutRefreshLocations()
{
	//
	// Найти локации во всём тексте
	//
	QSet<QString> locations = QSet<QString>::fromList(m_scenario->findLocations());
	locations.unite(QSet<QString>::fromList(m_scenarioDraft->findLocations()));

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
		DatabaseLayer::Database::transaction();
		foreach (const QString& location, locationsToDelete) {
			DataStorageLayer::StorageFacade::locationStorage()->removeLocation(location);
		}
		DatabaseLayer::Database::commit();

		//
		// Добавить новых
		//
		DatabaseLayer::Database::transaction();
		foreach (const QString& location, locations) {
			if (!DataStorageLayer::StorageFacade::locationStorage()->hasLocation(location)) {
				DataStorageLayer::StorageFacade::locationStorage()->storeLocation(location);
			}
		}
		DatabaseLayer::Database::commit();
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
				BusinessLogic::ChronometerFacade::secondsToTime(workingScenario()->durationAtPosition(_cursorPosition));
		QString durationToEnd =
				BusinessLogic::ChronometerFacade::secondsToTime(workingScenario()->fullDuration());
		duration = QString("%1: <b>%2 | %3</b>").arg(tr("Chron.")).arg(durationToCursor).arg(durationToEnd);
	}

	m_textEditManager->setDuration(duration);
}

void ScenarioManager::aboutUpdateCounters()
{
	m_textEditManager->setCountersInfo(workingScenario()->countersInfo());
}

void ScenarioManager::aboutUpdateCurrentSynopsis(int _cursorPosition)
{
	QString itemHeader = workingScenario()->itemHeaderAtPosition(_cursorPosition);
	m_sceneSynopsisManager->setHeader(itemHeader);

	QString synopsis = workingScenario()->itemSynopsisAtPosition(_cursorPosition);
	m_sceneSynopsisManager->setSynopsis(synopsis);
}

void ScenarioManager::aboutUpdateCurrentSceneSynopsis(const QString& _synopsis)
{
	workingScenario()->setItemSynopsisAtPosition(m_textEditManager->cursorPosition(), _synopsis);
}

void ScenarioManager::aboutBuildSynopsisFromScenes()
{
	QString synopsis = m_scenario->builSynopsisFromScenes();
	m_dataEditManager->setScenarioSynopsisFromScenes(synopsis);
}

void ScenarioManager::aboutSelectItemInNavigator(int _cursorPosition)
{
	QModelIndex index = workingScenario()->itemIndexAtPosition(_cursorPosition);

	if (!m_workModeIsDraft) {
		m_navigatorManager->setCurrentIndex(index);
	} else {
		m_draftNavigatorManager->setCurrentIndex(index);
	}
}

void ScenarioManager::aboutMoveCursorToItem(const QModelIndex& _index)
{
	setWorkingMode(sender());

	const int position = workingScenario()->itemStartPosition(_index);
	m_textEditManager->setCursorPosition(position);
}

void ScenarioManager::aboutMoveCursorToItem(int _itemPosition)
{
	setWorkingMode(sender());

	m_textEditManager->setCursorPosition(_itemPosition);
}

void ScenarioManager::aboutAddItem(const QModelIndex& _afterItemIndex, const QString& _itemHeader, int _itemType)
{
	setWorkingMode(sender());

	const int position = workingScenario()->itemEndPosition(_afterItemIndex);
	m_textEditManager->addScenarioItem(position, _itemHeader, _itemType);
}

void ScenarioManager::aboutRemoveItems(const QModelIndexList& _indexes)
{
	setWorkingMode(sender());

	const int from = workingScenario()->itemStartPosition(_indexes.first());
	const int to = workingScenario()->itemEndPosition(_indexes.last());
	m_textEditManager->removeScenarioText(from, to);
}

void ScenarioManager::aboutShowHideDraft()
{
	const bool draftInvisible = m_draftViewSplitter->sizes().last() == 0;

	//
	// Показать примечания, если скрыты
	//
	if (draftInvisible) {
		if (m_draftViewSplitter->property(SPLITTER_LAST_SIZES).isNull()) {
			int splitterHeight = m_draftViewSplitter->height();
			m_draftViewSplitter->setSizes(QList<int>() << splitterHeight * 2/3 << splitterHeight * 1/3);
		} else {
			m_draftViewSplitter->setSizes(m_draftViewSplitter->property(SPLITTER_LAST_SIZES).value<QList<int> >());
		}
	}
	//
	// Скрыть примечания
	//
	else {
		m_draftViewSplitter->setProperty(SPLITTER_LAST_SIZES, QVariant::fromValue<QList<int> >(m_draftViewSplitter->sizes()));
		m_draftViewSplitter->setSizes(QList<int>() << 1 << 0);
	}

	m_navigatorManager->setDraftVisible(draftInvisible);
}

void ScenarioManager::aboutShowHideNote()
{
	const bool noteInvisible = m_noteViewSplitter->sizes().last() == 0;

	//
	// Показать примечания, если скрыты
	//
	if (noteInvisible) {
		if (m_noteViewSplitter->property(SPLITTER_LAST_SIZES).isNull()) {
			int splitterHeight = m_noteViewSplitter->height();
			m_noteViewSplitter->setSizes(QList<int>() << splitterHeight * 2/3 << splitterHeight * 1/3);
		} else {
			m_noteViewSplitter->setSizes(m_noteViewSplitter->property(SPLITTER_LAST_SIZES).value<QList<int> >());
		}
	}
	//
	// Скрыть примечания
	//
	else {
		m_noteViewSplitter->setProperty(SPLITTER_LAST_SIZES, QVariant::fromValue<QList<int> >(m_noteViewSplitter->sizes()));
		m_noteViewSplitter->setSizes(QList<int>() << 1 << 0);
	}

	m_navigatorManager->setNoteVisible(noteInvisible);
}

void ScenarioManager::aboutSaveChanges()
{
	saveScenarioChanges(true);
	saveScenarioChanges(false);
}

void ScenarioManager::initData()
{
	m_navigatorManager->setNavigationModel(m_scenario->model());
	m_draftNavigatorManager->setNavigationModel(m_scenarioDraft->model());
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

	m_showFullscreen = new FlatButton(m_view);
	m_showFullscreen->setIcons(QIcon(":/Graphics/Icons/Editing/fullscreen.png"), QIcon(),
		QIcon(":/Graphics/Icons/Editing/fullscreen_active.png"));
	m_showFullscreen->setToolTip(ShortcutHelper::makeToolTip(tr("On/off Fullscreen Mode"), "F5"));
	m_showFullscreen->setShortcut(QKeySequence("F5"));
	m_showFullscreen->setCheckable(true);

	QWidget* rightWidget = new QWidget(m_view);

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setContentsMargins(QMargins());
	topLayout->setSpacing(0);
	topLayout->addWidget(m_viewEditorsToolbars);
	topLayout->addWidget(m_viewEditorsTabs);
	topLayout->addWidget(m_showFullscreen);
	QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
	rightLayout->setContentsMargins(QMargins());
	rightLayout->setSpacing(0);
	rightLayout->addLayout(topLayout);
	rightLayout->addWidget(m_viewEditors, 1);

	m_draftViewSplitter->setHandleWidth(1);
	m_draftViewSplitter->addWidget(m_navigatorManager->view());
	m_draftViewSplitter->addWidget(m_draftNavigatorManager->view());
	m_draftViewSplitter->setOrientation(Qt::Vertical);

	m_noteViewSplitter->setHandleWidth(1);
	m_noteViewSplitter->addWidget(m_draftViewSplitter);
	m_noteViewSplitter->addWidget(m_sceneSynopsisManager->view());
	m_noteViewSplitter->setOrientation(Qt::Vertical);

	m_mainViewSplitter->setHandleWidth(1);
	m_mainViewSplitter->addWidget(m_noteViewSplitter);
	m_mainViewSplitter->addWidget(rightWidget);
	m_mainViewSplitter->setStretchFactor(1, 1);
	m_mainViewSplitter->setOpaqueResize(false);

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

	connect(m_showFullscreen, SIGNAL(clicked()), this, SIGNAL(showFullscreen()));

	connect(m_navigatorManager, SIGNAL(addItem(QModelIndex,QString,int)), this, SLOT(aboutAddItem(QModelIndex,QString,int)));
	connect(m_navigatorManager, SIGNAL(removeItems(QModelIndexList)), this, SLOT(aboutRemoveItems(QModelIndexList)));
	connect(m_navigatorManager, SIGNAL(showHideDraft()), this, SLOT(aboutShowHideDraft()));
	connect(m_navigatorManager, SIGNAL(showHideNote()), this, SLOT(aboutShowHideNote()));
	connect(m_navigatorManager, SIGNAL(sceneChoosed(QModelIndex)), this, SLOT(aboutMoveCursorToItem(QModelIndex)));
	connect(m_navigatorManager, SIGNAL(sceneChoosed(int)), this, SLOT(aboutMoveCursorToItem(int)));
	connect(m_navigatorManager, SIGNAL(undoPressed()), m_textEditManager, SLOT(aboutUndo()));
	connect(m_navigatorManager, SIGNAL(redoPressed()), m_textEditManager, SLOT(aboutRedo()));

	connect(m_draftNavigatorManager, SIGNAL(addItem(QModelIndex,QString,int)), this, SLOT(aboutAddItem(QModelIndex,QString,int)));
	connect(m_draftNavigatorManager, SIGNAL(removeItems(QModelIndexList)), this, SLOT(aboutRemoveItems(QModelIndexList)));
	connect(m_draftNavigatorManager, SIGNAL(sceneChoosed(QModelIndex)), this, SLOT(aboutMoveCursorToItem(QModelIndex)));
	connect(m_draftNavigatorManager, SIGNAL(sceneChoosed(int)), this, SLOT(aboutMoveCursorToItem(int)));
	connect(m_draftNavigatorManager, SIGNAL(undoPressed()), m_textEditManager, SLOT(aboutUndo()));
	connect(m_draftNavigatorManager, SIGNAL(redoPressed()), m_textEditManager, SLOT(aboutRedo()));

	connect(m_sceneSynopsisManager, SIGNAL(synopsisChanged(QString)), this, SLOT(aboutUpdateCurrentSceneSynopsis(QString)));

	connect(m_dataEditManager, SIGNAL(buildSynopsisFromScenes()), this, SLOT(aboutBuildSynopsisFromScenes()));

	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateDuration(int)));
	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateCurrentSynopsis(int)));
	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutSelectItemInNavigator(int)), Qt::QueuedConnection);
	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateCounters()));

	connect(&m_saveChangesTimer, SIGNAL(timeout()), this, SLOT(aboutSaveChanges()));

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

	m_showFullscreen->setProperty("inTopPanel", true);
	m_showFullscreen->setProperty("topPanelTopBordered", true);
	m_showFullscreen->setProperty("topPanelRightBordered", true);
}

void ScenarioManager::setWorkingMode(QObject* _sender)
{
	if (ScenarioNavigatorManager* manager = qobject_cast<ScenarioNavigatorManager*>(_sender)) {
		const bool workingModeIsDraft = manager == m_draftNavigatorManager;

		if (m_workModeIsDraft != workingModeIsDraft) {
			m_workModeIsDraft = workingModeIsDraft;

			if (!m_workModeIsDraft) {
				m_textEditManager->setScenarioDocument(m_scenario->document());
				m_draftNavigatorManager->clearSelection();
			} else {
				m_textEditManager->setScenarioDocument(m_scenarioDraft->document(), IS_DRAFT);
				m_navigatorManager->clearSelection();
			}

			emit scenarioChanged();
		}
	}
}

BusinessLogic::ScenarioDocument* ScenarioManager::workingScenario() const
{
	return m_workModeIsDraft ? m_scenarioDraft : m_scenario;
}

void ScenarioManager::saveScenarioChanges(bool _isDraft)
{
	const QString newScenarioXml = _isDraft ? m_scenarioDraft->save() : m_scenario->save();
	const QByteArray newScenarioXmlHash = ::textMd5Hash(newScenarioXml);
	QString& lastScenarioXml = _isDraft ? m_lastScenarioDraftXml : m_lastScenarioXml;
	QByteArray& lastScenarioXmlHash = _isDraft ? m_lastScenarioDraftXmlHash : m_lastScenarioXmlHash;

	//
	// Если текущий текст сценария отличается от последнего сохранённого
	//
	if (lastScenarioXmlHash != newScenarioXmlHash) {
		//
		// Сформируем изменения
		//
		diff_match_patch<std::wstring> dmp;
		const std::wstring lastXml = lastScenarioXml.toStdWString();
		const std::wstring newXml = newScenarioXml.toStdWString();
		const std::wstring undoPatch = dmp.patch_toText(dmp.patch_make(newXml, lastXml));
		const std::wstring redoPatch = dmp.patch_toText(dmp.patch_make(lastXml, newXml));

		//
		// Сохраним изменения
		//
		DataStorageLayer::StorageFacade::scenarioChangeStorage()->append("user",
			QString::fromStdWString(undoPatch), QString::fromStdWString(redoPatch), _isDraft);

		//
		// Запомним новый текст
		// NOTE: т.к. использовались ссылки, то обновятся нужные значения
		//
		lastScenarioXml = newScenarioXml;
		lastScenarioXmlHash = newScenarioXmlHash;
	}
}
