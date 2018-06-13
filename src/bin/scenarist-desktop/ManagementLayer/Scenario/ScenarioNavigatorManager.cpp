#include "ScenarioNavigatorManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigator.h>
#include <UserInterfaceLayer/Scenario/ScenarioItemDialog/ScenarioItemDialog.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

using ManagementLayer::ScenarioNavigatorManager;
using BusinessLogic::ScenarioModel;
using BusinessLogic::ScenarioModelFiltered;
using UserInterface::ScenarioNavigator;
using UserInterface::ScenarioItemDialog;


ScenarioNavigatorManager::ScenarioNavigatorManager(QObject *_parent, QWidget* _parentWidget, bool _isDraft) :
    QObject(_parent),
    m_scenarioModel(0),
    m_scenarioModelProxy(new ScenarioModelFiltered(this)),
    m_navigator(new ScenarioNavigator(_parentWidget)),
    m_addItemDialog(new ScenarioItemDialog(m_navigator)),
    m_isDraft(_isDraft)
{
    initView();
    initConnections();
    reloadNavigatorSettings();
}

QWidget* ScenarioNavigatorManager::view() const
{
    return m_navigator;
}

void ScenarioNavigatorManager::setNavigationModel(ScenarioModel* _model)
{
    disconnectModel();
    m_scenarioModel = _model;
    m_scenarioModelProxy->setSourceModel(m_scenarioModel);
    m_navigator->setModel(m_scenarioModelProxy);
    connectModel();

    if (m_scenarioModel != 0) {
        aboutModelUpdated();
    }
}

void ScenarioNavigatorManager::reloadNavigatorSettings()
{
    //
    // Сбросим представление
    //
    m_navigator->resetView();
    m_navigator->setShowSceneNumber(
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "navigator/show-scenes-numbers",
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                .toInt()
                );
    m_navigator->setShowSceneTitle(
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "navigator/show-scene-title",
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                .toInt()
                );
    m_navigator->setShowSceneDescription(
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "navigator/show-scene-description",
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                .toInt()
                );
    m_navigator->setSceneDescriptionIsSceneText(
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "navigator/scene-description-is-scene-text",
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                .toInt()
                );
    m_navigator->setSceneDescriptionHeight(
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "navigator/scene-description-height",
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                .toInt()
                );
}

void ScenarioNavigatorManager::setCurrentIndex(const QModelIndex& _index)
{
    m_navigator->setCurrentIndex(m_scenarioModelProxy->mapFromSource(_index));
}

void ScenarioNavigatorManager::clearSelection()
{
    m_navigator->clearSelection();
}

void ScenarioNavigatorManager::setDraftVisible(bool _visible)
{
    m_navigator->setDraftVisible(_visible);
}

void ScenarioNavigatorManager::setSceneDescriptionVisible(bool _visible)
{
    m_navigator->setSceneDescriptionVisible(_visible);
}

void ScenarioNavigatorManager::setScriptBookmarksVisible(bool _visible)
{
    m_navigator->setScriptBookmarksVisible(_visible);
}

void ScenarioNavigatorManager::setScriptDictionariesVisible(bool _visible)
{
    m_navigator->setScriptDictionariesVisible(_visible);
}

void ScenarioNavigatorManager::setSceneNumbersPrefix(const QString& _prefix)
{
    m_navigator->setSceneNumbersPrefix(_prefix);
}

void ScenarioNavigatorManager::setCommentOnly(bool _isCommentOnly)
{
    m_navigator->setCommentOnly(_isCommentOnly);
}

void ScenarioNavigatorManager::aboutAddItem(const QModelIndex& _index)
{
    m_addItemDialog->clear();

    //
    // Если пользователь действительно хочет добавить элемент
    //
    if (m_addItemDialog->exec() == QLightBoxDialog::Accepted) {
        const int itemType = m_addItemDialog->itemType();
        const QString header = m_addItemDialog->header();
        const QColor color = m_addItemDialog->color();
        const QString description = m_addItemDialog->description();

        //
        // Если задан заголовок
        //
        if (!header.isEmpty()) {
            emit addItem(m_scenarioModelProxy->mapToSource(_index), itemType, header, color, description);
        }
    }
}

void ScenarioNavigatorManager::aboutRemoveItems(const QModelIndexList& _indexes)
{
    QModelIndexList removeIndexes;
    foreach (const QModelIndex& index, _indexes) {
        removeIndexes.append(m_scenarioModelProxy->mapToSource(index));
    }
    emit removeItems(removeIndexes);
}

void ScenarioNavigatorManager::aboutSetItemColors(const QModelIndexList& _indexes, const QString& _colors)
{
    QModelIndexList mappedIndexes;
    for (auto index : _indexes) {
        mappedIndexes.append(m_scenarioModelProxy->mapToSource(index));
    }
    emit setItemsColors(mappedIndexes, _colors);
}

void ScenarioNavigatorManager::aboutChangeItemType(const QModelIndex& _index, int _type)
{
    emit changeItemTypeRequested(m_scenarioModelProxy->mapToSource(_index), _type);
}

void ScenarioNavigatorManager::aboutSceneChoosed(const QModelIndex& _index)
{
    emit sceneChoosed(m_scenarioModelProxy->mapToSource(_index));
}

void ScenarioNavigatorManager::aboutModelUpdated()
{
    m_navigator->setScenesCount(m_scenarioModel->scenesCount());
}

void ScenarioNavigatorManager::initView()
{
    if (!m_isDraft) {
        m_navigator->setObjectName("scenarioNavigator");
    } else {
        m_navigator->setObjectName("scenarioDraftNavigator");
    }
    m_navigator->setIsDraft(m_isDraft);
}

void ScenarioNavigatorManager::initConnections()
{
    connectModel();

    connect(m_navigator, &ScenarioNavigator::addItem, this, &ScenarioNavigatorManager::aboutAddItem);
    connect(m_navigator, &ScenarioNavigator::removeItems, this, &ScenarioNavigatorManager::aboutRemoveItems);
    connect(m_navigator, &ScenarioNavigator::setItemsColors, this, &ScenarioNavigatorManager::aboutSetItemColors);
    connect(m_navigator, &ScenarioNavigator::changeItemTypeRequested, this, &ScenarioNavigatorManager::aboutChangeItemType);
    connect(m_navigator, &ScenarioNavigator::draftVisibleChanged, this, &ScenarioNavigatorManager::draftVisibleChanged);
    connect(m_navigator, &ScenarioNavigator::sceneDescriptionVisibleChanged, this, &ScenarioNavigatorManager::sceneDescriptionVisibleChanged);
    connect(m_navigator, &ScenarioNavigator::scriptBookmarksVisibleChanged, this, &ScenarioNavigatorManager::scriptBookmarksVisibleChanged);
    connect(m_navigator, &ScenarioNavigator::scriptDictionariesVisibleChanged, this, &ScenarioNavigatorManager::scriptDictionariesVisibleChanged);
    connect(m_navigator, &ScenarioNavigator::sceneChoosed, this, &ScenarioNavigatorManager::aboutSceneChoosed);
    connect(m_navigator, &ScenarioNavigator::undoRequest, this, &ScenarioNavigatorManager::undoRequest);
    connect(m_navigator, &ScenarioNavigator::redoRequest, this, &ScenarioNavigatorManager::redoRequest);
}

void ScenarioNavigatorManager::connectModel()
{
    if (m_scenarioModel != 0) {
        connect(m_scenarioModel, &ScenarioModel::rowsInserted, this, &ScenarioNavigatorManager::aboutModelUpdated);
        connect(m_scenarioModel, &ScenarioModel::rowsRemoved, this, &ScenarioNavigatorManager::aboutModelUpdated);
        connect(m_scenarioModel, &ScenarioModel::dataChanged, this, &ScenarioNavigatorManager::aboutModelUpdated);
        connect(m_scenarioModel, &ScenarioModel::mimeDropped,
                this, static_cast<void (ScenarioNavigatorManager::*)(int)>(&ScenarioNavigatorManager::sceneChoosed));
    }
}

void ScenarioNavigatorManager::disconnectModel()
{
    if (m_scenarioModel != 0) {
        disconnect(m_scenarioModel, &ScenarioModel::rowsInserted, this, &ScenarioNavigatorManager::aboutModelUpdated);
        disconnect(m_scenarioModel, &ScenarioModel::rowsRemoved, this, &ScenarioNavigatorManager::aboutModelUpdated);
        disconnect(m_scenarioModel, &ScenarioModel::dataChanged, this, &ScenarioNavigatorManager::aboutModelUpdated);
        disconnect(m_scenarioModel, &ScenarioModel::mimeDropped,
                   this, static_cast<void (ScenarioNavigatorManager::*)(int)>(&ScenarioNavigatorManager::sceneChoosed));
    }
}
