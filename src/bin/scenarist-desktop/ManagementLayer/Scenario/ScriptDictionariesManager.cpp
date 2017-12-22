#include "ScriptDictionariesManager.h"

#include <Domain/CharacterState.h>
#include <Domain/Place.h>
#include <Domain/ScenarioDay.h>
#include <Domain/SceneTime.h>
#include <Domain/Transition.h>

#include <DataLayer/DataStorageLayer/CharacterStateStorage.h>
#include <DataLayer/DataStorageLayer/PlaceStorage.h>
#include <DataLayer/DataStorageLayer/ScenarioDayStorage.h>
#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/TimeStorage.h>
#include <DataLayer/DataStorageLayer/TransitionStorage.h>

#include <UserInterfaceLayer/Scenario/ScriptDictionaries/ScriptDictionaries.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxinputdialog.h>
#include <3rd_party/Widgets/QLightBoxWidget/qlightboxmessage.h>

using ManagementLayer::ScriptDictionariesManager;
using UserInterface::ScriptDictionaries;


ScriptDictionariesManager::ScriptDictionariesManager(QObject* _parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_view(new ScriptDictionaries(_parentWidget))
{
    initView();
    initConnection();

    refresh();
}

QWidget* ScriptDictionariesManager::view() const
{
    return m_view;
}

void ScriptDictionariesManager::refresh()
{
    changeDictionary(0);
}

void ScriptDictionariesManager::setCommentOnly(bool _isCommentOnly)
{
    m_view->setCommentOnly(_isCommentOnly);
}

void ScriptDictionariesManager::changeDictionary(int _type)
{
    m_currentType = _type;

    QAbstractItemModel* model = nullptr;
    switch(static_cast<ScriptDictionaries::Type>(_type)) {
        case ScriptDictionaries::Type::SceneIntros: {
            model = DataStorageLayer::StorageFacade::placeStorage()->all();
            break;
        }

        case ScriptDictionaries::Type::SceneTimes: {
            model = DataStorageLayer::StorageFacade::timeStorage()->all();
            break;
        }

        case ScriptDictionaries::Type::SceneDays: {
            model = DataStorageLayer::StorageFacade::scenarioDayStorage()->all();
            break;
        }

        case ScriptDictionaries::Type::CharacterStates: {
            model = DataStorageLayer::StorageFacade::characterStateStorage()->all();
            break;
        }

        case ScriptDictionaries::Type::Transitions: {
            model = DataStorageLayer::StorageFacade::transitionStorage()->all();
            break;
        }

        default: {
            Q_ASSERT_X(0, Q_FUNC_INFO, QString("Unknown dictionary type %1").arg(_type).toUtf8().data());
            break;
        }
    }
    m_view->setModel(model);
}

void ScriptDictionariesManager::addItem()
{
    const QString itemName = QLightBoxInputDialog::getText(m_view, QString(), tr("Enter new item name"));
    if (itemName.isEmpty()) {
        return;
    }

    switch(static_cast<ScriptDictionaries::Type>(m_currentType)) {
        case ScriptDictionaries::Type::SceneIntros: {
            DataStorageLayer::StorageFacade::placeStorage()->storePlace(itemName);
            break;
        }

        case ScriptDictionaries::Type::SceneTimes: {
            DataStorageLayer::StorageFacade::timeStorage()->storeTime(itemName);
            break;
        }

        case ScriptDictionaries::Type::SceneDays: {
            DataStorageLayer::StorageFacade::scenarioDayStorage()->storeScenarioDay(itemName);
            break;
        }

        case ScriptDictionaries::Type::CharacterStates: {
            DataStorageLayer::StorageFacade::characterStateStorage()->storeCharacterState(itemName);
            break;
        }

        case ScriptDictionaries::Type::Transitions: {
            DataStorageLayer::StorageFacade::transitionStorage()->storeTransition(itemName);
            break;
        }

        default: {
            Q_ASSERT_X(0, Q_FUNC_INFO, QString("Unknown dictionary type %1").arg(m_currentType).toUtf8().data());
            break;
        }
    }
}

void ScriptDictionariesManager::removeItem(const QModelIndex& _index)
{
    const QString itemName = _index.data().toString();
    if (itemName.isEmpty()) {
        return;
    }

    if (QLightBoxMessage::question(m_view, QString(), tr("Are you sure to remove <b>%1</b>").arg(itemName))
        == QDialogButtonBox::No) {
        return;
    }

    switch(static_cast<ScriptDictionaries::Type>(m_currentType)) {
        case ScriptDictionaries::Type::SceneIntros: {
            DataStorageLayer::StorageFacade::placeStorage()->removePlace(itemName);
            break;
        }

        case ScriptDictionaries::Type::SceneTimes: {
            DataStorageLayer::StorageFacade::timeStorage()->removeTime(itemName);
            break;
        }

        case ScriptDictionaries::Type::SceneDays: {
            DataStorageLayer::StorageFacade::scenarioDayStorage()->removeScenarioDay(itemName);
            break;
        }

        case ScriptDictionaries::Type::CharacterStates: {
            DataStorageLayer::StorageFacade::characterStateStorage()->removeCharacterState(itemName);
            break;
        }

        case ScriptDictionaries::Type::Transitions: {
            DataStorageLayer::StorageFacade::transitionStorage()->removeTransition(itemName);
            break;
        }

        default: {
            Q_ASSERT_X(0, Q_FUNC_INFO, QString("Unknown dictionary type %1").arg(m_currentType).toUtf8().data());
            break;
        }
    }
}

void ScriptDictionariesManager::initView()
{
    m_view->setObjectName("scenarioScriptDictionariesView");
}

void ScriptDictionariesManager::initConnection()
{
    connect(m_view, &ScriptDictionaries::dictionaryChanged, this, &ScriptDictionariesManager::changeDictionary);
    connect(m_view, &ScriptDictionaries::addItemRequested, this, &ScriptDictionariesManager::addItem);
    connect(m_view, &ScriptDictionaries::removeItemRequested, this, &ScriptDictionariesManager::removeItem);
}
