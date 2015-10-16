#include "ScenarioManager.h"

#include "ScenarioNavigatorManager.h"
//#include "ScenarioSceneSynopsisManager.h"
//#include "ScenarioDataEditManager.h"
#include "ScenarioTextEditManager.h"

#include <Domain/Scenario.h>
#include <Domain/ScenarioChange.h>
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
#include <DataLayer/DataStorageLayer/ScenarioDataStorage.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/LocationStorage.h>

#include <3rd_party/Helpers/DiffMatchPatchHelper.h>
#include <3rd_party/Helpers/ShortcutHelper.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

#include <QHBoxLayout>
#include <QTextCursor>
#include <QTextBlock>
#include <QTimer>
#include <QSet>
#include <QStackedWidget>

using ManagementLayer::ScenarioManager;
using ManagementLayer::ScenarioNavigatorManager;
using ManagementLayer::ScenarioSceneSynopsisManager;
using ManagementLayer::ScenarioDataEditManager;
using ManagementLayer::ScenarioTextEditManager;
using BusinessLogic::ScenarioDocument;
using BusinessLogic::ScenarioBlockStyle;

namespace {
	/**
	 * @brief Индекс редактора сценария в стэке представлений
	 */
	const int TEXT_TAB_INDEX = 0;

	/**
	 * @brief Индекс навигатора в стэке представлений
	 */
	const int NAVIGATOR_TAB_INDEX = 1;

	/**
	 * @brief Ключ для хранения атрибута последнего размера сплитера
	 */
	const char* SPLITTER_LAST_SIZES = "last_sizes";

	/**
	 * @brief Ключ для доступа к черновику сценария
	 */
	const bool IS_DRAFT = true;

	/**
	 * @brief Интервал формирования патчей для отмены/повтора последнего действия, мс
	 */
	const int SAVE_CHANGES_INTERVAL = 5000;

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
				if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneHeading) {
					const QString location = BusinessLogic::SceneHeadingParser::locationName(cursor.block().text());
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
}


ScenarioManager::ScenarioManager(QObject *_parent, QWidget* _parentWidget) :
	QObject(_parent),
	m_toolbar(new QStackedWidget(_parentWidget)),
	m_view(new QStackedWidget(_parentWidget)),
	m_scenario(new ScenarioDocument(this)),
	m_scenarioDraft(new ScenarioDocument(this)),
	m_navigatorManager(new ScenarioNavigatorManager(this, m_view)),
	m_draftNavigatorManager(new ScenarioNavigatorManager(this, m_view, IS_DRAFT)),
//	m_sceneSynopsisManager(new ScenarioSceneSynopsisManager(this, m_view)),
//	m_dataEditManager(new ScenarioDataEditManager(this, m_view)),
	m_textEditManager(new ScenarioTextEditManager(this, m_view)),
	m_workModeIsDraft(false)
{
	initData();
	initView();
	initConnections();
	initStyleSheet();
}

QWidget* ScenarioManager::toolbar() const
{
	return m_toolbar;
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
	return DataStorageLayer::StorageFacade::scenarioDataStorage()->name();
}

int ScenarioManager::cursorPosition() const
{
	return m_textEditManager->cursorPosition();
}

void ScenarioManager::loadCurrentProject()
{
	//
	// Загрузим сценарий
	//
	// ... чистовик
	//
	Domain::Scenario* currentScenario =
			DataStorageLayer::StorageFacade::scenarioStorage()->current();
	m_scenario->load(currentScenario);
	//
	// ... черновик
	//
	Domain::Scenario* currentScenarioDraft =
			DataStorageLayer::StorageFacade::scenarioStorage()->current(IS_DRAFT);
	m_scenarioDraft->load(currentScenarioDraft);

	//
	// Установим данные для менеджеров
	//
	m_navigatorManager->setScenarioName(scenarioName());
	m_navigatorManager->setNavigationModel(m_scenario->model());
	m_draftNavigatorManager->setNavigationModel(m_scenarioDraft->model());
//	if (currentScenario != 0) {
//		m_dataEditManager->setScenarioName(DataStorageLayer::StorageFacade::scenarioDataStorage()->name());
//		m_dataEditManager->setScenarioSynopsis(DataStorageLayer::StorageFacade::scenarioDataStorage()->synopsis());
//	}
	m_textEditManager->setScenarioName(scenarioName());
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
	// Показываем редактор текста сценария
	//
	m_toolbar->setCurrentIndex(TEXT_TAB_INDEX);
	m_view->setCurrentIndex(TEXT_TAB_INDEX);
}

void ScenarioManager::startChangesHandling()
{
	//
	// Запускаем таймер сохранения изменений
	//
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
	aboutSaveScenarioChanges();
	DataStorageLayer::StorageFacade::scenarioChangeStorage()->store();

//	//
//	// Сохраняем данные
//	//
//	DataStorageLayer::StorageFacade::scenarioDataStorage()->setName(m_dataEditManager->scenarioName());
//	DataStorageLayer::StorageFacade::scenarioDataStorage()->setSynopsis(m_dataEditManager->scenarioSynopsis());
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

void ScenarioManager::closeCurrentProject()
{
	//
	// Остановим таймер сохранения изменений документа
	//
	m_saveChangesTimer.stop();

	//
	// Очистим от предыдущих данных
	//
	m_navigatorManager->setNavigationModel(0);
	m_draftNavigatorManager->setNavigationModel(0);
//	m_dataEditManager->clear();
	m_textEditManager->setScenarioDocument(0);

	//
	// Очистим сценарий
	//
	m_scenario->clear();
	m_scenarioDraft->clear();
}

void ScenarioManager::setCommentOnly(bool _isCommentOnly)
{
	m_navigatorManager->setCommentOnly(_isCommentOnly);
	m_draftNavigatorManager->setCommentOnly(_isCommentOnly);
//	m_sceneSynopsisManager->setCommentOnly(_isCommentOnly);
	m_textEditManager->setCommentOnly(_isCommentOnly);
//	m_dataEditManager->setCommentOnly(_isCommentOnly);
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
//	aboutUpdateDuration(m_textEditManager->cursorPosition());
}

void ScenarioManager::aboutCountersSettingsUpdated()
{
	m_scenario->refresh();
	m_scenarioDraft->refresh();
//	aboutUpdateCounters();
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
	if (QLightBoxMessage::question(m_view, tr("Apply refreshing"), message) == QDialogButtonBox::Yes) {
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

	if (QLightBoxMessage::question(m_view, tr("Apply refreshing"), message) == QDialogButtonBox::Yes) {
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

//void ScenarioManager::aboutScenarioNameChanged(const QString& _name)
//{
//	m_dataEditManager->setScenarioName(_name);
//}

void ScenarioManager::aboutApplyPatch(const QString& _patch, bool _isDraft)
{
	if (_isDraft) {
		m_scenarioDraft->document()->applyPatch(_patch);
	} else {
		m_scenario->document()->applyPatch(_patch);
	}
}

void ScenarioManager::aboutApplyPatches(const QList<QString>& _patches, bool _isDraft)
{
	if (_isDraft) {
		m_scenarioDraft->document()->applyPatches(_patches);
	} else {
		m_scenario->document()->applyPatches(_patches);
	}
}

void ScenarioManager::aboutCursorsUpdated(const QMap<QString, int>& _cursors, bool _isDraft)
{
	//
	// Запомним курсоры
	//
	if (_isDraft) {
		if (m_draftCursors != _cursors) {
			m_draftCursors = _cursors;
		}
	} else {
		if (m_cleanCursors != _cursors) {
			m_cleanCursors = _cursors;
		}
	}

	//
	// Обновим представление
	//
	if (m_workModeIsDraft == _isDraft) {
		m_textEditManager->setAdditionalCursors(_cursors);
	}
}

//void ScenarioManager::aboutUpdateDuration(int _cursorPosition)
//{
//	QString duration;
//	if (BusinessLogic::ChronometerFacade::chronometryUsed()) {
//		QString durationToCursor =
//				BusinessLogic::ChronometerFacade::secondsToTime(workingScenario()->durationAtPosition(_cursorPosition));
//		QString durationToEnd =
//				BusinessLogic::ChronometerFacade::secondsToTime(workingScenario()->fullDuration());
//		duration = QString("%1: <b>%2 | %3</b>").arg(tr("Chron.")).arg(durationToCursor).arg(durationToEnd);
//	}

//	m_textEditManager->setDuration(duration);
//}

//void ScenarioManager::aboutUpdateCounters()
//{
//	m_textEditManager->setCountersInfo(workingScenario()->countersInfo());
//}

//void ScenarioManager::aboutUpdateCurrentSynopsis(int _cursorPosition)
//{
//	QString itemHeader = workingScenario()->itemHeaderAtPosition(_cursorPosition);
//	m_sceneSynopsisManager->setHeader(itemHeader);

//	QString synopsis = workingScenario()->itemSynopsisAtPosition(_cursorPosition);
//	m_sceneSynopsisManager->setSynopsis(synopsis);
//}

void ScenarioManager::aboutUpdateCurrentSceneSynopsis(const QString& _synopsis)
{
	workingScenario()->setItemSynopsisAtPosition(m_textEditManager->cursorPosition(), _synopsis);
}

//void ScenarioManager::aboutBuildSynopsisFromScenes()
//{
//	QString synopsis = m_scenario->builSynopsisFromScenes();
//	m_dataEditManager->setScenarioSynopsisFromScenes(synopsis);
//}

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

void ScenarioManager::aboutAddItem(const QModelIndex& _afterItemIndex, int _itemType,
	const QString& _header, const QColor& _color, const QString& _description)
{
	setWorkingMode(sender());

	const int position = workingScenario()->itemEndPosition(_afterItemIndex);
	m_textEditManager->addScenarioItem(position, _itemType, _header, _color, _description);
}

void ScenarioManager::aboutRemoveItems(const QModelIndexList& _indexes)
{
	setWorkingMode(sender());

	const int from = workingScenario()->itemStartPosition(_indexes.first());
	const int to = workingScenario()->itemEndPosition(_indexes.last());
	m_textEditManager->removeScenarioText(from, to);
}

void ScenarioManager::aboutSetItemColors(const QModelIndex& _itemIndex, const QString& _colors)
{
	setWorkingMode(sender());

	const int position = workingScenario()->itemStartPosition(_itemIndex);
	workingScenario()->setItemColorsAtPosition(position, _colors);

	emit scenarioChanged();
}

//void ScenarioManager::aboutShowHideDraft()
//{
//	const bool draftInvisible = m_draftViewSplitter->sizes().last() == 0;

//	//
//	// Показать примечания, если скрыты
//	//
//	if (draftInvisible) {
//		if (m_draftViewSplitter->property(SPLITTER_LAST_SIZES).isNull()) {
//			int splitterHeight = m_draftViewSplitter->height();
//			m_draftViewSplitter->setSizes(QList<int>() << splitterHeight * 2/3 << splitterHeight * 1/3);
//		} else {
//			m_draftViewSplitter->setSizes(m_draftViewSplitter->property(SPLITTER_LAST_SIZES).value<QList<int> >());
//		}
//	}
//	//
//	// Скрыть примечания
//	//
//	else {
//		m_draftViewSplitter->setProperty(SPLITTER_LAST_SIZES, QVariant::fromValue<QList<int> >(m_draftViewSplitter->sizes()));
//		m_draftViewSplitter->setSizes(QList<int>() << 1 << 0);
//	}

//	m_navigatorManager->setDraftVisible(draftInvisible);
//}

//void ScenarioManager::aboutShowHideNote()
//{
//	const bool noteInvisible = m_noteViewSplitter->sizes().last() == 0;

//	//
//	// Показать примечания, если скрыты
//	//
//	if (noteInvisible) {
//		if (m_noteViewSplitter->property(SPLITTER_LAST_SIZES).isNull()) {
//			int splitterHeight = m_noteViewSplitter->height();
//			m_noteViewSplitter->setSizes(QList<int>() << splitterHeight * 2/3 << splitterHeight * 1/3);
//		} else {
//			m_noteViewSplitter->setSizes(m_noteViewSplitter->property(SPLITTER_LAST_SIZES).value<QList<int> >());
//		}
//	}
//	//
//	// Скрыть примечания
//	//
//	else {
//		m_noteViewSplitter->setProperty(SPLITTER_LAST_SIZES, QVariant::fromValue<QList<int> >(m_noteViewSplitter->sizes()));
//		m_noteViewSplitter->setSizes(QList<int>() << 1 << 0);
//	}

//	m_navigatorManager->setNoteVisible(noteInvisible);
//}

void ScenarioManager::aboutSaveScenarioChanges()
{
	Domain::ScenarioChange* change = m_scenario->document()->saveChanges();
	if (change != 0) {
		change->setIsDraft(false);
	}

	Domain::ScenarioChange* changeDraft = m_scenarioDraft->document()->saveChanges();
	if (changeDraft != 0) {
		changeDraft->setIsDraft(true);
	}

	emit scenarioChangesSaved();
	emit cursorPositionUpdated(cursorPosition(), m_workModeIsDraft);
}

void ScenarioManager::initData()
{
	m_navigatorManager->setNavigationModel(m_scenario->model());
	m_draftNavigatorManager->setNavigationModel(m_scenarioDraft->model());
	m_textEditManager->setScenarioDocument(m_scenario->document());
}

void ScenarioManager::initView()
{
	m_toolbar->addWidget(m_textEditManager->toolbar());
	m_toolbar->addWidget(m_navigatorManager->toolbar());

	m_view->addWidget(m_textEditManager->view());
	m_view->addWidget(m_navigatorManager->view());
}

void ScenarioManager::initConnections()
{
	connect(m_textEditManager, &ScenarioTextEditManager::showNavigatorRequested, [=](){
		m_toolbar->setCurrentIndex(NAVIGATOR_TAB_INDEX);
		m_view->setCurrentIndex(NAVIGATOR_TAB_INDEX);
	});
	connect(m_navigatorManager, &ScenarioNavigatorManager::showTextRequested, [=](){
		m_toolbar->setCurrentIndex(TEXT_TAB_INDEX);
		m_view->setCurrentIndex(TEXT_TAB_INDEX);
	});

	connect(m_navigatorManager, SIGNAL(addItem(QModelIndex,int,QString,QColor,QString)), this, SLOT(aboutAddItem(QModelIndex,int,QString,QColor,QString)));
	connect(m_navigatorManager, SIGNAL(removeItems(QModelIndexList)), this, SLOT(aboutRemoveItems(QModelIndexList)));
	connect(m_navigatorManager, SIGNAL(setItemColors(QModelIndex,QString)), this, SLOT(aboutSetItemColors(QModelIndex,QString)));
	connect(m_navigatorManager, SIGNAL(sceneChoosed(QModelIndex)), this, SLOT(aboutMoveCursorToItem(QModelIndex)));
	connect(m_navigatorManager, SIGNAL(sceneChoosed(int)), this, SLOT(aboutMoveCursorToItem(int)));
	connect(m_navigatorManager, SIGNAL(undoPressed()), m_textEditManager, SLOT(aboutUndo()));
	connect(m_navigatorManager, SIGNAL(redoPressed()), m_textEditManager, SLOT(aboutRedo()));

	connect(m_draftNavigatorManager, SIGNAL(addItem(QModelIndex,int,QString,QColor,QString)), this, SLOT(aboutAddItem(QModelIndex,int,QString,QColor,QString)));
	connect(m_draftNavigatorManager, SIGNAL(removeItems(QModelIndexList)), this, SLOT(aboutRemoveItems(QModelIndexList)));
	connect(m_draftNavigatorManager, SIGNAL(setItemColors(QModelIndex,QString)), this, SLOT(aboutSetItemColors(QModelIndex,QString)));
	connect(m_draftNavigatorManager, SIGNAL(sceneChoosed(QModelIndex)), this, SLOT(aboutMoveCursorToItem(QModelIndex)));
	connect(m_draftNavigatorManager, SIGNAL(sceneChoosed(int)), this, SLOT(aboutMoveCursorToItem(int)));
	connect(m_draftNavigatorManager, SIGNAL(undoPressed()), m_textEditManager, SLOT(aboutUndo()));
	connect(m_draftNavigatorManager, SIGNAL(redoPressed()), m_textEditManager, SLOT(aboutRedo()));

//	connect(m_sceneSynopsisManager, SIGNAL(synopsisChanged(QString)), this, SLOT(aboutUpdateCurrentSceneSynopsis(QString)));

//	connect(m_dataEditManager, SIGNAL(buildSynopsisFromScenes()), this, SLOT(aboutBuildSynopsisFromScenes()));

//	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateDuration(int)));
//	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateCurrentSynopsis(int)));
	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutSelectItemInNavigator(int)), Qt::QueuedConnection);
//	connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateCounters()));

	connect(&m_saveChangesTimer, SIGNAL(timeout()), this, SLOT(aboutSaveScenarioChanges()));

	//
	// Настраиваем отслеживание изменений документа
	//
//	connect(m_sceneSynopsisManager, SIGNAL(synopsisChanged(QString)), this, SIGNAL(scenarioChanged()));
//	connect(m_dataEditManager, SIGNAL(scenarioNameChanged()), this, SIGNAL(scenarioChanged()));
//	connect(m_dataEditManager, SIGNAL(scenarioSynopsisChanged()), this, SIGNAL(scenarioChanged()));
	connect(m_textEditManager, SIGNAL(textChanged()), this, SIGNAL(scenarioChanged()));
}

void ScenarioManager::initStyleSheet()
{
}

void ScenarioManager::setWorkingMode(QObject* _sender)
{
	if (ScenarioNavigatorManager* manager = qobject_cast<ScenarioNavigatorManager*>(_sender)) {
		const bool workingModeIsDraft = manager == m_draftNavigatorManager;

		if (m_workModeIsDraft != workingModeIsDraft) {
			m_workModeIsDraft = workingModeIsDraft;

			if (!m_workModeIsDraft) {
				m_textEditManager->setScenarioDocument(m_scenario->document());
				m_textEditManager->setAdditionalCursors(m_cleanCursors);
				m_draftNavigatorManager->clearSelection();
			} else {
				m_textEditManager->setScenarioDocument(m_scenarioDraft->document(), IS_DRAFT);
				m_textEditManager->setAdditionalCursors(m_draftCursors);
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
