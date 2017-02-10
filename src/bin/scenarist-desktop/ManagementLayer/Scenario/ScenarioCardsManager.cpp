#include "ScenarioCardsManager.h"

#include <Domain/Scenario.h>

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>
#include <BusinessLayer/ScenarioDocument/ScenarioModelItem.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <UserInterfaceLayer/Scenario/ScenarioCards/ScenarioCardsView.h>
#include <UserInterfaceLayer/Scenario/ScenarioItemDialog/ScenarioSchemeItemDialog.h>

using ManagementLayer::ScenarioCardsManager;
using UserInterface::ScenarioCardsView;
using UserInterface::ScenarioSchemeItemDialog;

namespace {
    const bool ISDRAFT = true;
    const bool IS_SCRIPT = false;
}


ScenarioCardsManager::ScenarioCardsManager(QObject* _parent, QWidget* _parentWidget) :
    QObject(_parent),
    m_view(new ScenarioCardsView(IS_SCRIPT, _parentWidget)),
    m_addItemDialog(new ScenarioSchemeItemDialog(_parentWidget)),
    m_scenario(nullptr),
    m_model(nullptr)
{
    initConnections();
    reloadSettings();
}

QWidget* ScenarioCardsManager::view() const
{
    return m_view;
}

void ScenarioCardsManager::reloadSettings()
{
    m_view->setUseCorkboardBackground(
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "cards/use-corkboard",
                    DataStorageLayer::SettingsStorage::ApplicationSettings)
                .toInt()
                );

    const bool useDarkTheme =
            DataStorageLayer::StorageFacade::settingsStorage()->value(
                "application/use-dark-theme",
                DataStorageLayer::SettingsStorage::ApplicationSettings)
            .toInt();
    const QString colorSuffix = useDarkTheme ? "-dark" : "";
    m_view->setBackgroundColor(
                QColor(
                    DataStorageLayer::StorageFacade::settingsStorage()->value(
                        "cards/background-color" + colorSuffix,
                        DataStorageLayer::SettingsStorage::ApplicationSettings)
                    )
                );
}

QString ScenarioCardsManager::save() const
{
    return m_view->save();
}

void ScenarioCardsManager::saveChanges(bool _hasChangesInText)
{
    m_view->saveChanges(_hasChangesInText);
}

void ScenarioCardsManager::load(BusinessLogic::ScenarioModel* _model, const QString& _xml)
{
    //
    // Сохраним модель
    //
    if (m_model != _model) {
        m_model = _model;
//		connect(m_model, &BusinessLogic::ScenarioModel::rowsInserted, [=] (const QModelIndex& _parent, int _first, int _last) {
//			for (int row = _first; row <= _last; ++row) {
//				const QModelIndex index = m_model->index(row, 0, _parent);
//				BusinessLogic::ScenarioModelItem* item = m_model->itemForIndex(index);
//				QModelIndex currentCardIndex = _parent;
//				const bool isCardFirstInParent = row == 0;
//				if (row > 0) {
//					//
//					// -1 т.к. нужен предыдущий элемент
//					//
//					const int itemRow = row - 1;
//					if (_parent.isValid()) {
//						currentCardIndex = _parent.child(itemRow, 0);
//					} else {
//						currentCardIndex = m_model->index(itemRow, 0);
//					}
//				}

//				BusinessLogic::ScenarioModelItem* currentCard = m_model->itemForIndex(currentCardIndex);
//				m_view->selectCard(currentCard->uuid());
//				m_view->addCard(
//					item->uuid(),
//					item->type(),
//					item->title().isEmpty() ? item->header() : item->title(),
//					item->description(),
//					item->colors(),
//					isCardFirstInParent);
//			}
//		});
//		connect(m_model, &BusinessLogic::ScenarioModel::rowsAboutToBeRemoved, [=] (const QModelIndex& _parent, int _first, int _last) {
//			for (int row = _last; row >= _first; --row) {
//				QModelIndex currentCardIndex = _parent;
//				if (_parent.isValid()) {
//					currentCardIndex = _parent.child(row, 0);
//				} else {
//					currentCardIndex = m_model->index(row, 0);
//				}
//				BusinessLogic::ScenarioModelItem* currentCard = m_model->itemForIndex(currentCardIndex);
//				m_view->removeCard(currentCard->uuid());
//			}
//		});
        connect(m_model, &BusinessLogic::ScenarioModel::dataChanged, [=] (const QModelIndex& _topLeft, const QModelIndex& _bottomRight) {
            for (int row = _topLeft.row(); row <= _bottomRight.row(); ++row) {
                const QModelIndex index = m_model->index(row, 0, _topLeft.parent());
                const BusinessLogic::ScenarioModelItem* item = m_model->itemForIndex(index);
                m_view->updateCard(
                    item->uuid(),
                    item->type() == BusinessLogic::ScenarioModelItem::Folder,
                    item->title().isEmpty() ? item->header() : item->title(),
                    item->description(),
                    item->colors());
            }
        });
    }

    //
    // Загрузим сценарий
    //
    // ... если схема есть, то просто загружаем её
    //
    if (!_xml.isEmpty()) {
        m_view->load(_xml);
    }
    //
    // ... а если схема пуста, сформируем её на основе модели
    //
    else {
        m_view->load(m_model->simpleScheme());
    }
}

void ScenarioCardsManager::clear()
{
    if (m_model != nullptr) {
        m_model->disconnect();
        m_model = nullptr;
    }
    m_view->clear();
}

void ScenarioCardsManager::undo()
{
    m_view->undo();
}

void ScenarioCardsManager::redo()
{
    m_view->redo();
}

void ScenarioCardsManager::setCommentOnly(bool _isCommentOnly)
{
    m_view->setCommentOnly(_isCommentOnly);
}

void ScenarioCardsManager::addCard()
{
    m_addItemDialog->clear();

    //
    // Если пользователь действительно хочет добавить элемент
    //
    if (m_addItemDialog->exec() == QLightBoxDialog::Accepted) {
        const int type = m_addItemDialog->cardType();
        const QString title = m_addItemDialog->cardTitle();
        const QString color = m_addItemDialog->cardColor();
        const QString description = m_addItemDialog->cardDescription();

        //
        // Если задан заголовок
        //
        if (!title.isEmpty()) {
            //
            // Определим карточку, после которой нужно добавить элемент
            //
            QModelIndex index;
            const QString lastItemUuid = m_view->lastItemUuid();
            if (!lastItemUuid.isEmpty()) {
                index = m_model->indexForUuid(lastItemUuid);
            }

            emit addCardRequest(index, type, title, QColor(color), description);
        }
    }
}

void ScenarioCardsManager::editCard(const QString& _uuid)
{
    m_addItemDialog->clear();

    const QModelIndex indexForUpdate = m_model->indexForUuid(_uuid);
    const auto* itemForUpdate = m_model->itemForIndex(indexForUpdate);
    m_addItemDialog->setCardType(itemForUpdate->type());
    m_addItemDialog->setCardTitle(itemForUpdate->title());
    const QString firstColor = itemForUpdate->colors().split(";").first();
    m_addItemDialog->setCardColor(firstColor);
    m_addItemDialog->setCardDescription(itemForUpdate->description());

    //
    // Если пользователь действительно хочет изменить элемент
    //
    if (m_addItemDialog->exec() == QLightBoxDialog::Accepted) {
        const int type = m_addItemDialog->cardType();
        const QString title = m_addItemDialog->cardTitle();
        QString colors = itemForUpdate->colors();
        if (firstColor != m_addItemDialog->cardColor()) {
            colors.replace(firstColor, m_addItemDialog->cardColor());
        }
        const QString description = m_addItemDialog->cardDescription();

        //
        // Испускаем запрос на изменение
        //
        emit updateCardRequest(indexForUpdate, type, title, colors, description);
    }
}

void ScenarioCardsManager::removeCard(const QString& _uuid)
{
//	emit removeCardRequest(m_model->indexForUuid(_uuid));
}

void ScenarioCardsManager::moveCard(const QString& _cardId, const QString& _actId, const QString& _previousCardId)
{
    if (!_cardId.isEmpty()) {
        const QModelIndex parentIndex = m_model->indexForUuid(_actId);
        const QModelIndex previousIndex = m_model->indexForUuid(_previousCardId);
        const QModelIndex movedIndex = m_model->indexForUuid(_cardId);

        //
        // Синхронизируем перемещение с моделью
        //
        int previousRow = 0;
        if (previousIndex.isValid()) {
            previousRow = previousIndex.row() + 1;
        }
        QMimeData* mime = m_model->mimeData({ movedIndex });
        m_model->dropMimeData(mime, Qt::MoveAction, previousRow, 0, parentIndex);
    }
}

void ScenarioCardsManager::changeCardColors(const QString& _uuid, const QString& _colors)
{
//	if (!_uuid.isEmpty()) {
//		const QModelIndex index = m_model->indexForUuid(_uuid);
//		emit cardColorsChanged(index, _colors);
//	}
}

void ScenarioCardsManager::changeCardType(const QString& _uuid, int _cardType)
{
//	if (!_uuid.isEmpty()) {
//		const QModelIndex index = m_model->indexForUuid(_uuid);
//		int mappedType = BusinessLogic::ScenarioModelItem::Scene;
//		if (_cardType == CardShape::TypeScenesGroup) {
//			mappedType = BusinessLogic::ScenarioModelItem::SceneGroup;
//		} else if (_cardType == CardShape::TypeFolder) {
//			mappedType = BusinessLogic::ScenarioModelItem::Folder;
//		}
//		emit cardTypeChanged(index, mappedType);
//	}
}

void ScenarioCardsManager::initConnections()
{
    //
    // Если не удалось загрузить сохранённую схему, построим её заново
    //
    connect(m_view, &ScenarioCardsView::schemeNotLoaded, [=] {
        m_view->load(m_model->simpleScheme());
    });

    connect(m_view, &ScenarioCardsView::undoRequest, this, &ScenarioCardsManager::undoRequest);
    connect(m_view, &ScenarioCardsView::redoRequest, this, &ScenarioCardsManager::redoRequest);

    connect(m_view, &ScenarioCardsView::addCardClicked, this, &ScenarioCardsManager::addCard);
    connect(m_view, &ScenarioCardsView::editCardRequest, this, &ScenarioCardsManager::editCard);
//	connect(m_view, &ScenarioCardsView::removeCardRequest, this, &ScenarioCardsManager::removeCard);
    connect(m_view, &ScenarioCardsView::cardMoved, this, &ScenarioCardsManager::moveCard);
//	connect(m_view, &ScenarioCardsView::cardColorsChanged, this, &ScenarioCardsManager::changeCardColors);
//	connect(m_view, &ScenarioCardsView::itemTypeChanged, this, &ScenarioCardsManager::changeCardType);

    connect(m_view, &ScenarioCardsView::fullscreenRequest, this, &ScenarioCardsManager::fullscreenRequest);

//	connect(m_view, &ScenarioCardsView::schemeChanged, this, &ScenarioCardsManager::schemeChanged);
}
