#include "ScenarioManager.h"

#include "ScenarioCardsManager.h"
#include "ScenarioNavigatorManager.h"
#include "ScenarioSceneDescriptionManager.h"
#include "ScenarioTextEditManager.h"

#include <Domain/Research.h>
#include <Domain/Scenario.h>
#include <Domain/ScenarioChange.h>

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioModelItem.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <DataLayer/Database/Database.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>
#include <DataLayer/DataStorageLayer/ScenarioChangeStorage.h>
#include <DataLayer/DataStorageLayer/ScenarioDataStorage.h>
#include <DataLayer/DataStorageLayer/ResearchStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/DiffMatchPatchHelper.h>
#include <3rd_party/Helpers/ShortcutHelper.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>
#include <3rd_party/Widgets/TabBar/TabBar.h>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QTextCursor>
#include <QTextBlock>
#include <QTimer>
#include <QSet>
#include <QStackedWidget>
#include <QWidget>

using ManagementLayer::ScenarioManager;
using ManagementLayer::ScenarioCardsManager;
using ManagementLayer::ScenarioNavigatorManager;
using ManagementLayer::ScenarioSceneDescriptionManager;
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
     * @brief Интервалы формирования патчей для отмены/повтора последнего действия, мс
     * @note Минимальный интервал = 1 секунда
     */
    /** @{ */
    const int SLOW_SAVE_CHANGES_INTERVAL = 5000;
    const int FAST_SAVE_CHANGES_INTERVAL = 1000;
    /** @} */

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
                    if (name == cursor.selectedText().toUpper()) {
                        replaceSelection = true;
                    }
                }
                //
                // Если в блоке участники сцены
                //
                else if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneCharacters) {
                    const QStringList names = BusinessLogic::SceneCharactersParser::characters(cursor.block().text());
                    if (names.contains(cursor.selectedText().toUpper())) {
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
                            checkCursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
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
                            checkCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
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
                    if (location == cursor.selectedText().toUpper()) {
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
     * @brief Обновить цвета текста и фона блоков для заданного документа
     */
    static void updateDocumentBlocksColors(QTextDocument* _document) {
        QTextCursor cursor(_document);
        cursor.beginEditBlock();
        do {
            cursor.movePosition(QTextCursor::StartOfBlock);
            ScenarioBlockStyle blockStyle =
                BusinessLogic::ScenarioTemplateFacade::getTemplate().blockStyle(cursor.block());
            //
            // Если в блоке есть выделения, обновляем цвет только тех частей, которые не входят в выделения
            //
            QTextBlock currentBlock = cursor.block();
            if (!currentBlock.textFormats().isEmpty()) {
                foreach (const QTextLayout::FormatRange& range, currentBlock.textFormats()) {
                    if (!range.format.boolProperty(ScenarioBlockStyle::PropertyIsReviewMark)) {
                        cursor.setPosition(currentBlock.position() + range.start);
                        cursor.setPosition(cursor.position() + range.length, QTextCursor::KeepAnchor);
                        cursor.mergeCharFormat(blockStyle.charFormat());
                        cursor.mergeBlockCharFormat(blockStyle.charFormat());
                        cursor.mergeBlockFormat(blockStyle.blockFormat());
                    }
                }
                cursor.movePosition(QTextCursor::EndOfBlock);
            }
            //
            // Если выделений нет, обновляем блок целиком
            //
            else {
                cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                cursor.mergeCharFormat(blockStyle.charFormat());
                cursor.mergeBlockCharFormat(blockStyle.charFormat());
                cursor.mergeBlockFormat(blockStyle.blockFormat());
            }
            cursor.movePosition(QTextCursor::NextBlock);
        } while (!cursor.atEnd());
        cursor.endEditBlock();
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
    m_cardsManager(new ScenarioCardsManager(this, _parentWidget)),
    m_navigatorManager(new ScenarioNavigatorManager(this, m_view)),
    m_draftNavigatorManager(new ScenarioNavigatorManager(this, m_view, IS_DRAFT)),
    m_sceneDescriptionManager(new ScenarioSceneDescriptionManager(this, m_view)),
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

QWidget* ScenarioManager::cardsView() const
{
    return m_cardsManager->view();
}

BusinessLogic::ScenarioDocument* ScenarioManager::scenario() const
{
    return m_scenario;
}

BusinessLogic::ScenarioDocument*ScenarioManager::scenarioDraft() const
{
    return m_scenarioDraft;
}

int ScenarioManager::cursorPosition() const
{
    return m_textEditManager->cursorPosition();
}

void ScenarioManager::setCursorPosition(int _position) const
{
    m_textEditManager->setCursorPosition(_position);
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
    m_navigatorManager->setNavigationModel(m_scenario->model());
    m_draftNavigatorManager->setNavigationModel(m_scenarioDraft->model());
    m_textEditManager->setScenarioDocument(m_scenarioDraft->document(), IS_DRAFT);
    m_textEditManager->setScenarioDocument(m_scenario->document());
    //
    // ... содержимое карточек устанавливаем в последнюю очередь, чтобы корректно загрузить схему
    //
    m_cardsManager->load(m_scenario->model(), currentScenario->scheme());

    //
    // Обновим счётчики, когда данные полностью загрузятся
    //
    QTimer::singleShot(100, this, SLOT(aboutUpdateCounters()));
}

void ScenarioManager::startChangesHandling()
{
    //
    // Запускаем таймер сохранения изменений
    //
    m_saveChangesTimer.start(SLOW_SAVE_CHANGES_INTERVAL);
}

void ScenarioManager::loadCurrentProjectSettings(const QString& _projectPath)
{
    //
    // Загрузим режим чистовик/черновик
    //
    const bool lastScenarioModeIsDraft =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                QString("projects/%1/last-scenario-mode-is-draft").arg(_projectPath),
                DataStorageLayer::SettingsStorage::ApplicationSettings
                ).toInt();
    setWorkingMode(lastScenarioModeIsDraft ? m_draftNavigatorManager : m_navigatorManager);

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
    m_scenario->scenario()->setScheme(m_cardsManager->save());
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
}

void ScenarioManager::saveCurrentProjectSettings(const QString& _projectPath)
{
    //
    // Сохраним текущий режим чистовик/черновик
    //
    DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                QString("projects/%1/last-scenario-mode-is-draft").arg(_projectPath),
                m_workModeIsDraft ? "1" : "0",
                DataStorageLayer::SettingsStorage::ApplicationSettings);

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
    m_cardsManager->clear();
    m_navigatorManager->setNavigationModel(nullptr);
    m_draftNavigatorManager->setNavigationModel(nullptr);
    m_textEditManager->setScenarioDocument(nullptr);

    //
    // Очистим сценарий
    //
    m_scenario->clear();
    m_scenarioDraft->clear();
}

void ScenarioManager::setCommentOnly(bool _isCommentOnly)
{
    m_cardsManager->setCommentOnly(_isCommentOnly);
    m_navigatorManager->setCommentOnly(_isCommentOnly);
    m_draftNavigatorManager->setCommentOnly(_isCommentOnly);
    m_sceneDescriptionManager->setCommentOnly(_isCommentOnly);
    m_textEditManager->setCommentOnly(_isCommentOnly);
}

bool ScenarioManager::workModeIsDraft() const
{
    return m_workModeIsDraft;
}

void ScenarioManager::aboutCardsSettingsUpdated()
{
    m_cardsManager->reloadSettings();
}

void ScenarioManager::aboutTextEditSettingsUpdated()
{
    BusinessLogic::ScenarioTemplateFacade::updateTemplatesColors();

    m_textEditManager->reloadTextEditSettings();

    ::updateDocumentBlocksColors(m_scenario->document());
    ::updateDocumentBlocksColors(m_scenarioDraft->document());
}

void ScenarioManager::aboutNavigatorSettingsUpdated()
{
    m_navigatorManager->reloadNavigatorSettings();
    m_draftNavigatorManager->reloadNavigatorSettings();
}

void ScenarioManager::aboutChronometrySettingsUpdated()
{
    aboutRefreshDuration(m_textEditManager->cursorPosition());
    m_textEditManager->reloadTextEditSettings();
}

void ScenarioManager::aboutCountersSettingsUpdated()
{
    aboutRefreshCounters();
    m_textEditManager->reloadTextEditSettings();
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
             DataStorageLayer::StorageFacade::researchStorage()->characters()->toList()) {
        Research* character = dynamic_cast<Research*>(domainObject);
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
            DataStorageLayer::StorageFacade::researchStorage()->removeCharacter(character);
        }
        DatabaseLayer::Database::commit();

        //
        // Добавить новых
        //
        DatabaseLayer::Database::transaction();
        foreach (const QString& character, characters) {
            if (!DataStorageLayer::StorageFacade::researchStorage()->hasCharacter(character)) {
                DataStorageLayer::StorageFacade::researchStorage()->storeCharacter(character);
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
             DataStorageLayer::StorageFacade::researchStorage()->locations()->toList()) {
        Research* location = dynamic_cast<Research*>(domainObject);
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
            DataStorageLayer::StorageFacade::researchStorage()->removeLocation(location);
        }
        DatabaseLayer::Database::commit();

        //
        // Добавить новых
        //
        DatabaseLayer::Database::transaction();
        foreach (const QString& location, locations) {
            if (!DataStorageLayer::StorageFacade::researchStorage()->hasLocation(location)) {
                DataStorageLayer::StorageFacade::researchStorage()->storeLocation(location);
            }
        }
        DatabaseLayer::Database::commit();
    }
}

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

    //
    // Изменим время синхронизации документа с облаком.
    // Конечно, setInterval перезапускает таймер, но в этом нет ничего страшного,
    // поскольку только что было сохранение и время отсчитывается заново.
    // Интервал устанавливается только при _isDraft = true, чтобы не устанавливать
    // его дважды при сохраненнии (на всяки случай)
    //
    if (_isDraft) {
        m_saveChangesTimer.setInterval((m_draftCursors.isEmpty() && m_cleanCursors.isEmpty())
                                       ? SLOW_SAVE_CHANGES_INTERVAL : FAST_SAVE_CHANGES_INTERVAL);
    }
}

void ScenarioManager::scrollToAdditionalCursor(int _additionalCursorIndex)
{
    m_textEditManager->scrollToAdditionalCursor(_additionalCursorIndex);
}

void ScenarioManager::aboutUndo()
{
    aboutSaveScenarioChanges();
    workingScenario()->document()->undoReimpl();
    m_cardsManager->undo();
}

void ScenarioManager::aboutRedo()
{
    workingScenario()->document()->redoReimpl();
    m_cardsManager->redo();
}

void ScenarioManager::aboutRefreshDuration(int _cursorPosition)
{
    if (BusinessLogic::ChronometerFacade::chronometryUsed()) {
        m_scenario->refresh();
    }
    aboutUpdateDuration(_cursorPosition);
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

void ScenarioManager::aboutRefreshCounters()
{
    m_scenario->refresh();
    aboutUpdateCounters();
}

void ScenarioManager::aboutUpdateCounters()
{
    m_textEditManager->setCountersInfo(workingScenario()->countersInfo());
}

void ScenarioManager::aboutUpdateCurrentSceneTitleAndDescription(int _cursorPosition)
{
    QString itemTitle = workingScenario()->itemTitleAtPosition(_cursorPosition);
    if (itemTitle.isEmpty()) {
        //
        // Если название сцены не задано, используем заголовок сцены
        //
        itemTitle = workingScenario()->itemHeaderAtPosition(_cursorPosition);
    }
    m_sceneDescriptionManager->setTitle(itemTitle);

    const QString description = workingScenario()->itemDescriptionAtPosition(_cursorPosition);
    m_sceneDescriptionManager->setDescription(description);
}

void ScenarioManager::aboutUpdateCurrentSceneTitle(const QString& _title)
{
    workingScenario()->setItemTitleAtPosition(m_textEditManager->cursorPosition(), _title);
}

void ScenarioManager::aboutUpdateCurrentSceneDescription(const QString& _description)
{
    workingScenario()->setItemDescriptionAtPosition(cursorPosition, _description);
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

void ScenarioManager::aboutAddItemFromCards(const QModelIndex& _afterItemIndex, int _itemType,
    const QString& _title, const QColor& _color, const QString& _description)
{
    //
    // Карточки добавляются только в режиме чистовика
    //
    setWorkingMode(m_navigatorManager);

    int position = 0;
    if (_itemType == BusinessLogic::ScenarioModelItem::Folder) {
        position = workingScenario()->itemEndPosition(_afterItemIndex);
    } else {
        position = workingScenario()->itemMiddlePosition(_afterItemIndex);
    }
    m_textEditManager->addScenarioItemFromCards(position, _itemType, _title, _color, _description);
}

void ScenarioManager::aboutAddItem(const QModelIndex& _afterItemIndex, int _itemType,
    const QString& _header, const QColor& _color, const QString& _description)
{
    setWorkingMode(sender());

    const int position = workingScenario()->itemEndPosition(_afterItemIndex);
    m_textEditManager->addScenarioItem(position, _itemType, _header, _color, _description);
}

void ScenarioManager::aboutUpdateItemFromCards(const QModelIndex& _itemIndex, int _itemType,
    const QString& _header, const QString& _colors, const QString& _description)
{
    //
    // Изменение элемента из карточек только в режиме чистовика
    //
    setWorkingMode(m_navigatorManager);

    const int startPosition = workingScenario()->itemStartPosition(_itemIndex);
    const int endPosition = workingScenario()->itemEndPosition(_itemIndex);
    m_textEditManager->editScenarioItem(startPosition, endPosition, _itemType, _header, _colors, _description);
}

void ScenarioManager::aboutRemoveItemFromCards(const QModelIndex& _index)
{
    //
    // Удаляем сцены из карточек только в режиме чистовика
    //
    setWorkingMode(m_navigatorManager);

    aboutRemoveItems({ _index });
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

void ScenarioManager::aboutChangeItemType(const QModelIndex& _index, int _type)
{
    setWorkingMode(sender());

    const int position = workingScenario()->itemStartPosition(_index);
    m_textEditManager->changeItemType(position, _type);

    emit scenarioChanged();
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

void ScenarioManager::aboutSaveScenarioChanges()
{
    //
    // Сохраняем изменения сценария
    //
    Domain::ScenarioChange* change = m_scenario->document()->saveChanges();
    if (change != nullptr) {
        change->setIsDraft(false);
    }
    //
    // ... и черновика
    //
    Domain::ScenarioChange* changeDraft = m_scenarioDraft->document()->saveChanges();
    if (changeDraft != nullptr) {
        changeDraft->setIsDraft(true);
    }

    //
    // Сохраняем изменения в карточках
    //
    m_cardsManager->saveChanges(change != nullptr);

#ifdef Q_OS_MAC
    //
    // FIXME: Если есть открытый диалог сохранения, или открытия, то он закрывается
    // после испускания последующих сигналов, так что мы просто их игнорируем,
    // пока не будет закрыт диалог
    //
    if (QApplication::activeModalWidget() != 0) {
        return;
    }
#endif

    //
    // Запросим обновление данных
    //
    emit updateScenarioRequest();
    emit updateCursorsRequest(cursorPosition(), m_workModeIsDraft);
}

void ScenarioManager::initData()
{
    m_navigatorManager->setNavigationModel(m_scenario->model());
    m_draftNavigatorManager->setNavigationModel(m_scenarioDraft->model());
    m_textEditManager->setScenarioDocument(m_scenario->document());
}

void ScenarioManager::initView()
{
    BusinessLogic::ScenarioTemplateFacade::updateTemplatesColors();

    m_view->setTabOrder(0, m_textEditManager->view());

    m_viewEditorsToolbars = new QStackedWidget(m_view);
    m_viewEditorsToolbars->addWidget(m_textEditManager->toolbar());

    m_viewEditors = new QStackedWidget(m_view);
    m_viewEditors->addWidget(m_textEditManager->view());

    m_showFullscreen = new FlatButton(m_view);
    m_showFullscreen->setIcons(QIcon(":/Graphics/Icons/Editing/fullscreen.png"),
        QIcon(":/Graphics/Icons/Editing/fullscreen_active.png"));
    m_showFullscreen->setToolTip(ShortcutHelper::makeToolTip(tr("On/off Fullscreen Mode"), "F5"));
    m_showFullscreen->setShortcut(Qt::Key_F5);
    m_showFullscreen->setCheckable(true);

    QWidget* rightWidget = new QWidget(m_view);
    rightWidget->setObjectName("scenarioRightWidget");

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(QMargins());
    topLayout->setSpacing(0);
    topLayout->addWidget(m_viewEditorsToolbars);
    topLayout->addWidget(m_showFullscreen);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(QMargins());
    rightLayout->setSpacing(0);
    rightLayout->addLayout(topLayout);
    rightLayout->addWidget(m_viewEditors, 1);

    m_draftViewSplitter->setObjectName("draftScenarioEditSplitter");
    m_draftViewSplitter->setHandleWidth(1);
    m_draftViewSplitter->setOrientation(Qt::Vertical);
    m_draftViewSplitter->addWidget(m_navigatorManager->view());
    m_draftViewSplitter->addWidget(m_draftNavigatorManager->view());
    m_draftViewSplitter->setSizes({1, 0});

    m_noteViewSplitter->setObjectName("noteScenarioEditSplitter");
    m_noteViewSplitter->setHandleWidth(1);
    m_noteViewSplitter->setOrientation(Qt::Vertical);
    m_noteViewSplitter->addWidget(m_draftViewSplitter);
    m_noteViewSplitter->addWidget(m_sceneDescriptionManager->view());
    m_noteViewSplitter->setSizes({1, 0});

    m_mainViewSplitter->setObjectName("mainScenarioEditSplitter");
    m_mainViewSplitter->setHandleWidth(1);
    m_mainViewSplitter->setOrientation(Qt::Horizontal);
    m_mainViewSplitter->setStretchFactor(1, 1);
    m_mainViewSplitter->setOpaqueResize(false);
    m_mainViewSplitter->addWidget(m_noteViewSplitter);
    m_mainViewSplitter->addWidget(rightWidget);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    layout->addWidget(m_mainViewSplitter);

    m_view->setLayout(layout);
}

void ScenarioManager::initConnections()
{
    connect(m_showFullscreen, SIGNAL(clicked()), this, SIGNAL(showFullscreen()));

    connect(m_cardsManager, &ScenarioCardsManager::addCardRequest, this, &ScenarioManager::aboutAddItemFromCards);
    connect(m_cardsManager, &ScenarioCardsManager::updateCardRequest, this, &ScenarioManager::aboutUpdateItemFromCards);
    connect(m_cardsManager, &ScenarioCardsManager::removeCardRequest, this, &ScenarioManager::aboutRemoveItemFromCards);
    connect(m_cardsManager, &ScenarioCardsManager::cardColorsChanged, this, &ScenarioManager::aboutSetItemColors);
    connect(m_cardsManager, &ScenarioCardsManager::cardTypeChanged, this, &ScenarioManager::aboutChangeItemType);
    connect(m_cardsManager, &ScenarioCardsManager::fullscreenRequest, this, &ScenarioManager::showFullscreen);
    connect(m_cardsManager, &ScenarioCardsManager::undoRequest, this, &ScenarioManager::aboutUndo);
    connect(m_cardsManager, &ScenarioCardsManager::redoRequest, this, &ScenarioManager::aboutRedo);

    connect(m_navigatorManager, SIGNAL(addItem(QModelIndex,int,QString,QColor,QString)), this, SLOT(aboutAddItem(QModelIndex,int,QString,QColor,QString)));
    connect(m_navigatorManager, SIGNAL(removeItems(QModelIndexList)), this, SLOT(aboutRemoveItems(QModelIndexList)));
    connect(m_navigatorManager, SIGNAL(setItemColors(QModelIndex,QString)), this, SLOT(aboutSetItemColors(QModelIndex,QString)));
    connect(m_navigatorManager, &ScenarioNavigatorManager::changeItemTypeRequested, this, &ScenarioManager::aboutChangeItemType);
    connect(m_navigatorManager, SIGNAL(showHideDraft()), this, SLOT(aboutShowHideDraft()));
    connect(m_navigatorManager, SIGNAL(showHideNote()), this, SLOT(aboutShowHideNote()));
    connect(m_navigatorManager, SIGNAL(sceneChoosed(QModelIndex)), this, SLOT(aboutMoveCursorToItem(QModelIndex)));
    connect(m_navigatorManager, SIGNAL(sceneChoosed(int)), this, SLOT(aboutMoveCursorToItem(int)));
    connect(m_navigatorManager, &ScenarioNavigatorManager::undoRequest, this, &ScenarioManager::aboutUndo);
    connect(m_navigatorManager, &ScenarioNavigatorManager::redoRequest, this, &ScenarioManager::aboutRedo);

    connect(m_draftNavigatorManager, SIGNAL(addItem(QModelIndex,int,QString,QColor,QString)), this, SLOT(aboutAddItem(QModelIndex,int,QString,QColor,QString)));
    connect(m_draftNavigatorManager, SIGNAL(removeItems(QModelIndexList)), this, SLOT(aboutRemoveItems(QModelIndexList)));
    connect(m_draftNavigatorManager, SIGNAL(setItemColors(QModelIndex,QString)), this, SLOT(aboutSetItemColors(QModelIndex,QString)));
    connect(m_draftNavigatorManager, &ScenarioNavigatorManager::changeItemTypeRequested, this, &ScenarioManager::aboutChangeItemType);
    connect(m_draftNavigatorManager, SIGNAL(sceneChoosed(QModelIndex)), this, SLOT(aboutMoveCursorToItem(QModelIndex)));
    connect(m_draftNavigatorManager, SIGNAL(sceneChoosed(int)), this, SLOT(aboutMoveCursorToItem(int)));
    connect(m_draftNavigatorManager, &ScenarioNavigatorManager::undoRequest, this, &ScenarioManager::aboutUndo);
    connect(m_draftNavigatorManager, &ScenarioNavigatorManager::redoRequest, this, &ScenarioManager::aboutRedo);

    connect(m_sceneDescriptionManager, &ScenarioSceneDescriptionManager::titleChanged, this, &ScenarioManager::aboutUpdateCurrentSceneTitle);
    connect(m_sceneDescriptionManager, &ScenarioSceneDescriptionManager::descriptionChanged, this, &ScenarioManager::aboutUpdateCurrentSceneDescription);

    connect(m_textEditManager, SIGNAL(textModeChanged()), this, SLOT(aboutRefreshCounters()));
    connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateDuration(int)));
    connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateCurrentSceneTitleAndDescription(int)));
    connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutSelectItemInNavigator(int)), Qt::QueuedConnection);
    connect(m_textEditManager, SIGNAL(cursorPositionChanged(int)), this, SLOT(aboutUpdateCounters()));
    connect(m_textEditManager, &ScenarioTextEditManager::undoRequest, this, &ScenarioManager::aboutUndo);
    connect(m_textEditManager, &ScenarioTextEditManager::redoRequest, this, &ScenarioManager::aboutRedo);

    connect(&m_saveChangesTimer, SIGNAL(timeout()), this, SLOT(aboutSaveScenarioChanges()));

    //
    // Настраиваем отслеживание изменений документа
    //
    connect(m_cardsManager, &ScenarioCardsManager::cardsChanged, this, &ScenarioManager::scenarioChanged);
    connect(m_sceneDescriptionManager, &ScenarioSceneDescriptionManager::titleChanged, this, &ScenarioManager::scenarioChanged);
    connect(m_sceneDescriptionManager, &ScenarioSceneDescriptionManager::descriptionChanged, this, &ScenarioManager::scenarioChanged);
    connect(m_textEditManager, &ScenarioTextEditManager::textChanged, this, &ScenarioManager::scenarioChanged);
}

void ScenarioManager::initStyleSheet()
{
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

            BusinessLogic::ScenarioTextDocument* prevTextDocument = 0;
            BusinessLogic::ScenarioTextDocument* nextTextDocument = 0;
            QMap<QString, int> additionalCursors;
            ScenarioNavigatorManager* prevNavigatorManager = 0;

            if (!m_workModeIsDraft) {
                prevTextDocument = m_scenarioDraft->document();
                nextTextDocument = m_scenario->document();
                additionalCursors = m_cleanCursors;
                prevNavigatorManager = m_draftNavigatorManager;
            } else {
                prevTextDocument = m_scenario->document();
                nextTextDocument = m_scenarioDraft->document();
                additionalCursors = m_draftCursors;
                prevNavigatorManager = m_navigatorManager;
            }

            nextTextDocument->setOutlineMode(prevTextDocument->outlineMode());
            m_textEditManager->setScenarioDocument(nextTextDocument, workingModeIsDraft);
            m_textEditManager->setAdditionalCursors(additionalCursors);
            prevNavigatorManager->clearSelection();

            emit scenarioChanged();
        }
    }
}

BusinessLogic::ScenarioDocument* ScenarioManager::workingScenario() const
{
    return m_workModeIsDraft ? m_scenarioDraft : m_scenario;
}
