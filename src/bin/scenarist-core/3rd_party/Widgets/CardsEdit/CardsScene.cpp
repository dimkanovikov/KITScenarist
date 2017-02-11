#include "CardsScene.h"

#include "ActItem.h"
#include "CardItem.h"

#include "cmath"

#include <QDomDocument>
#include <QDomNode>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMimeData>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QWidgetAction>
#include <QXmlStreamReader>
#include <QVBoxLayout>


CardsScene::CardsScene(QObject *parent) :
    QGraphicsScene(parent)
{
}

void CardsScene::setCardsSize(const QSizeF& _size)
{
    if (m_cardsSize != _size) {
        m_cardsSize = _size;

        for (QGraphicsItem* item : m_items) {
            if (CardItem* card = qgraphicsitem_cast<CardItem*>(item)) {
                card->setSize(m_cardsSize);
            }
        }

        reorderItemsOnScene();
    }
}

void CardsScene::setCardsDistance(qreal _distance)
{
    if (m_cardsDistance != _distance) {
        m_cardsDistance =_distance;

        reorderItemsOnScene();
    }
}

void CardsScene::setCardsInRow(int _count)
{
    if (m_cardsInRowCount != _count) {
        m_cardsInRowCount = _count;

        reorderItemsOnScene();
    }
}

void CardsScene::setCanAddActs(bool _can)
{
    if (m_isCanAddActs != _can) {
        m_isCanAddActs = _can;
    }
}

void CardsScene::setFixedMode(bool _isFixed)
{
    if (m_isFixedMode != _isFixed) {
        m_isFixedMode = _isFixed;
        reorderItemsOnScene();
    }
}

QString CardsScene::lastItemUuid() const
{
    QString lastItemUuid;
    if (!m_items.isEmpty()) {
        if (ActItem* act = qgraphicsitem_cast<ActItem*>(m_items.last())) {
            lastItemUuid = act->uuid();
        } else if (CardItem* card = qgraphicsitem_cast<CardItem*>(m_items.last())) {
            lastItemUuid = card->uuid();
        }
    }

    return lastItemUuid;
}

void CardsScene::addAct(const QString& _uuid, const QString& _title, const QString& _description, const QString& _colors)
{
    //
    // Вставим акт после самого последнего элемента
    //
    QString previousItemUuid = lastItemUuid();
    insertAct(_uuid, _title, _description, _colors, previousItemUuid);
}

void CardsScene::insertAct(const QString& _uuid, const QString& _title, const QString& _description,
    const QString& _colors, const QString& _previousItemUuid)
{
    ActItem* act = new ActItem;
    act->setUuid(_uuid);
    act->setTitle(_title);
    act->setDescription(_description);
    act->setColors(_colors);

    //
    // Добавляем новый акт
    //
    addItem(act);

    //
    // Выделяем добавленный акт
    //
    clearSelection();
    act->setSelected(true);

    //
    // Запомним добавленный акт
    //
    int insertIndex = 0;
    if (!_previousItemUuid.isEmpty()
        && m_itemsMap.contains(_previousItemUuid)) {
        QGraphicsItem* previousItem = m_itemsMap[_previousItemUuid];
        insertIndex = m_items.indexOf(previousItem) + 1;
        //
        // ... сдвигаем индекс, пока не дойдём до конца вложенных элементов
        //
        for (; insertIndex < m_items.size(); ++insertIndex) {
            if (m_items[insertIndex]->type() == ActItem::Type) {
                break;
            } else if (CardItem* card = qgraphicsitem_cast<CardItem*>(m_items[insertIndex])) {
                if (card->isEmbedded() == false) {
                    break;
                }
            }
        }
    }
    m_items.insert(insertIndex, act);
    m_itemsMap.insert(act->uuid(), act);

    //
    // Упорядочим, если надо
    //
    reorderItemsOnScene();

    //
    // Уведомляем подписчиков о том, что акт был добавлен
    //
    emit actAdded(act->uuid());
}

void CardsScene::addCard(const QString& _uuid, bool _isFolder, const QString& _title, const QString& _description, const QString& _stamp, const QString& _colors, bool _isEmbedded, const QPointF& _position)
{
    //
    // Вставим карточку после самого последнего элемента
    //
    QString previousItemUuid = lastItemUuid();
    insertCard(_uuid, _isFolder, _title, _description, _stamp, _colors, _isEmbedded, _position, previousItemUuid);
}

void CardsScene::insertCard(const QString& _uuid, bool _isFolder, const QString& _title, const QString& _description,
    const QString& _stamp, const QString& _colors, bool _isEmbedded, const QPointF& _position, const QString& _previousItemUuid)
{
    CardItem* card = new CardItem;
    card->setUuid(_uuid);
    card->setIsFolder(_isFolder);
    card->setTitle(_title);
    card->setDescription(_description);
    card->setStamp(_stamp);
    card->setColors(_colors);
    card->setIsEmbedded(_isEmbedded);
    card->setPos(_position);
    card->setSize(m_cardsSize);
    addItem(card);

    //
    // Выделяем добавленную карточку
    //
    clearSelection();
    card->setSelected(true);

    //
    // Запомним добавленную карточку
    //
    int insertIndex = 0;
    if (!_previousItemUuid.isEmpty()
        && m_itemsMap.contains(_previousItemUuid)) {
        QGraphicsItem* previousItem = m_itemsMap[_previousItemUuid];
        insertIndex = m_items.indexOf(previousItem) + 1;
        //
        // Если карточка вкладывается после акта
        //
        if (previousItem->type() == ActItem::Type) {
            //
            // ... сдвигаем индекс, пока не дойдём до конца вложенных в этот акт элементов
            //
            for (; insertIndex < m_items.size(); ++insertIndex) {
                if (m_items[insertIndex]->type() == ActItem::Type) {
                    break;
                } else if (CardItem* card = qgraphicsitem_cast<CardItem*>(m_items[insertIndex])) {
                    if (card->isEmbedded() == false) {
                        break;
                    }
                }
            }
        }
    }
    m_items.insert(insertIndex, card);
    m_itemsMap.insert(card->uuid(), card);

    //
    // Упорядочим, если надо
    //
    reorderItemsOnScene();

    //
    // Уведомляем подписчиков о том, что карточка была добавлена
    //
    emit cardAdded(card->uuid());
}

void CardsScene::updateItem(const QString& _uuid, bool _isFolder, const QString& _title,
    const QString& _description, const QString& _stamp, const QString& _colors, bool _isEmbedded, bool _isAct)
{
    if (m_itemsMap.contains(_uuid)) {
        //
        // Обновляем акт
        //
        if (ActItem* act = qgraphicsitem_cast<ActItem*>(m_itemsMap[_uuid])) {
            //
            // Если тип не сменился, просто обновим
            //
            if (_isAct) {
                act->setTitle(_title);
                act->setDescription(_description);
                act->setColors(_colors);

                //
                // Уведомляем подписчиков
                //
                emit actChanged(_uuid);
            }
            //
            // Если тип сменился, то удаляем акт и создаём новую сцену на его месте
            //
            else {
                const int actIndex = m_items.indexOf(act);
                QString previousItemUuid;
                if (actIndex > 0) {
                    if (ActItem* previous = qgraphicsitem_cast<ActItem*>(m_items[actIndex - 1])) {
                        previousItemUuid = previous->uuid();
                    } else if (CardItem* previous = qgraphicsitem_cast<CardItem*>(m_items[actIndex - 1])) {
                        previousItemUuid = previous->uuid();
                    }
                }

                removeAct(act->uuid());
                insertCard(_uuid, _isFolder, _title, _description, _stamp, _colors, false, QPointF(), previousItemUuid);
            }
        }
        //
        // ... карточку
        //
        else if (CardItem* card = qgraphicsitem_cast<CardItem*>(m_itemsMap[_uuid])) {
            //
            // Если тип не сменился, обновляем
            //
            if (_isAct == false) {
                card->setIsFolder(_isFolder);
                card->setTitle(_title);
                card->setDescription(_description);
                card->setColors(_colors);
                card->setIsEmbedded(_isEmbedded);

                //
                // Уведомляем подписчиков
                //
                emit cardChanged(_uuid);
            }
            //
            // Если тип сменился, то удаляем карточку и добавляем акт
            //
            else {
                const int cardIndex = m_items.indexOf(card);
                QString previousItemUuid;
                if (cardIndex > 0) {
                    if (ActItem* previous = qgraphicsitem_cast<ActItem*>(m_items[cardIndex - 1])) {
                        previousItemUuid = previous->uuid();
                    } else if (CardItem* previous = qgraphicsitem_cast<CardItem*>(m_items[cardIndex - 1])) {
                        previousItemUuid = previous->uuid();
                    }
                }

                removeCard(card->uuid());
                insertAct(_uuid, _title, _description, _colors, previousItemUuid);
            }
        }
    }
}

void CardsScene::removeSceneItem(const QString& _uuid)
{
    if (m_itemsMap.contains(_uuid)) {
        if (m_itemsMap[_uuid]->type() == ActItem::Type) {
            removeAct(_uuid);
        } else if (m_itemsMap[_uuid]->type() == CardItem::Type) {
            removeCard(_uuid);
        }
    }
}

void CardsScene::removeAct(const QString& _uuid)
{
    if (m_itemsMap.contains(_uuid)) {
        if (ActItem* act = qgraphicsitem_cast<ActItem*>(m_itemsMap.take(_uuid))) {
            //
            // Удаляем сам акт
            //
            m_items.removeAll(act);
            removeItem(act);
            delete act;
            act = nullptr;

            //
            // Упорядочим, если надо
            //
            reorderItemsOnScene();

            //
            // Уведомляем подписчиков
            //
            emit actRemoved(_uuid);
        }
    }
}

void CardsScene::removeCard(const QString& _uuid)
{
    if (m_itemsMap.contains(_uuid)) {
        if (CardItem* card = qgraphicsitem_cast<CardItem*>(m_itemsMap.take(_uuid))) {
            //
            // Удаляем саму карточку
            //
            m_items.removeAll(card);
            removeItem(card);
            delete card;
            card = nullptr;

            //
            // Упорядочим, если надо
            //
            reorderItemsOnScene();

            //
            // Уведомляем подписчиков
            //
            emit actRemoved(_uuid);
        }
    }
}

void CardsScene::refresh()
{
    reorderItemsOnScene();
}

QString CardsScene::save() const
{
    QString xml;
    QXmlStreamWriter writer(&xml);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("cards");

    //
    // Сохраняем настройки вида
    //
    if (!views().isEmpty()) {
        const QGraphicsView* view = views().first();
        writer.writeAttribute("x", QString::number(sceneRect().x()));
        writer.writeAttribute("y", QString::number(sceneRect().y()));
        writer.writeAttribute("width", QString::number(sceneRect().width()));
        writer.writeAttribute("height", QString::number(sceneRect().height()));
        writer.writeAttribute("scale", QString::number(view->transform().m11()));
        writer.writeAttribute("scroll_x", QString::number(view->horizontalScrollBar()->value()));
        writer.writeAttribute("scroll_y", QString::number(view->verticalScrollBar()->value()));
    }

    //
    // Сохраняем расположение элементов
    //
    for (QGraphicsItem* item : m_items) {
        if (ActItem* act = qgraphicsitem_cast<ActItem*>(item)) {
            writer.writeEmptyElement("act");
            writer.writeAttribute("id", act->uuid());
            writer.writeAttribute("title", act->title());
            writer.writeAttribute("description", act->description());
            writer.writeAttribute("colors", act->colors());
        } else if (CardItem* card = qgraphicsitem_cast<CardItem*>(item)) {
            writer.writeEmptyElement("card");
            writer.writeAttribute("id", card->uuid());
            writer.writeAttribute("is_folder", card->isFolder() ? "true" : "false");
            writer.writeAttribute("title", card->title());
            writer.writeAttribute("description", card->description());
            writer.writeAttribute("stamp", card->stamp());
            writer.writeAttribute("colors", card->colors());
            writer.writeAttribute("is_embedded", card->isEmbedded() ? "true" : "false");
            writer.writeAttribute("x", QString::number(card->pos().x()));
            writer.writeAttribute("y", QString::number(card->pos().y()));
        }
    }

    writer.writeEndElement();
    writer.writeEndDocument();

    return xml;
}

bool CardsScene::load(const QString& _xml)
{
    //
    // Очищаем текущие данные
    //
    for (QGraphicsItem* item : m_items) {
        removeItem(item);
        delete item;
    }
    m_items.clear();


    QDomDocument doc;
    if (!doc.setContent(_xml)) {
        return false;
    }

    //
    // Считываем настройки вида
    //
    if (!views().isEmpty()) {
        QGraphicsView* view = views().first();

        //
        // Восстанавливаем масштаб
        //
        view->resetTransform();
        const qreal scaleFactor = doc.documentElement().attribute("scale").toDouble();
        view->scale(scaleFactor, scaleFactor);

        //
        // ... размер сцены
        //
        const qreal rectX = doc.documentElement().attribute("x").toDouble();
        const qreal rectY = doc.documentElement().attribute("y").toDouble();
        const qreal rectWidth = doc.documentElement().attribute("width").toDouble();
        const qreal rectHeight = doc.documentElement().attribute("height").toDouble();
        setSceneRect(rectX, rectY, rectWidth, rectHeight);

        //
        // ... и позиционирование
        //
        const int scrollX = doc.documentElement().attribute("scroll_x").toInt();
        view->horizontalScrollBar()->setValue(scrollX);
        const int scrollY = doc.documentElement().attribute("scroll_y").toInt();
        view->verticalScrollBar()->setValue(scrollY);
    }

    //
    // Считываем данные о карточках
    //
    const QDomNodeList& items = doc.documentElement().childNodes();
    for (int i = 0; i < items.count(); ++i) {
        QDomElement item = items.at(i).toElement();
        const QDomNamedNodeMap& attributes = item.attributes();
        if (item.tagName() == "act") {
            const QString uuid = attributes.namedItem("id").toAttr().value();
            const QString title = attributes.namedItem("title").toAttr().value();
            const QString description = attributes.namedItem("description").toAttr().value();
            const QString colors = attributes.namedItem("colors").toAttr().value();
            addAct(uuid, title, description, colors);
        } else if (item.tagName() == "card") {
            const QString uuid = attributes.namedItem("id").toAttr().value();
            const bool isFolder = attributes.namedItem("is_folder").toAttr().value() == "true";
            const QString title = attributes.namedItem("title").toAttr().value();
            const QString description = attributes.namedItem("description").toAttr().value();
            const QString stamp = attributes.namedItem("stamp").toAttr().value();
            const QString colors = attributes.namedItem("colors").toAttr().value();
            const bool isEmbedded = attributes.namedItem("is_embedded").toAttr().value() == "true";
            const qreal x = attributes.namedItem("x").toAttr().value().toDouble();
            const qreal y = attributes.namedItem("y").toAttr().value().toDouble();
            addCard(uuid, isFolder, title, description, stamp, colors, isEmbedded, QPointF(x, y));
        }
    }

    //
    // Сбрасываем размер сцены, чтобы она могла динамически расширяться при перемещении элементов
    //
    setSceneRect(QRectF());

    return true;
}

void CardsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* _event)
{
    //
    // Если контекстное меню вызывается над элементом
    //
    for (QGraphicsItem* item : items(_event->scenePos())) {
        clearSelection();
        item->setSelected(true);
        break;
    }


    //
    // Сформируем контекстное меню в зависимости от того, выбраны ли карточки на экране
    //
    QMenu* menu = new QMenu(views().value(0, nullptr));
    QAction* addActAction = menu->addAction(tr("Add act"));
    QAction* addCardAction = menu->addAction(tr("Add card"));
    QAction* editActAction = menu->addAction(tr("Edit act"));
    QAction* editCardAction = menu->addAction(tr("Edit card"));
    QAction* removeActAction = menu->addAction(tr("Remove act"));
    QAction* removeCardAction = menu->addAction(tr("Remove card"));
    if (selectedItems().isEmpty()) {
        editActAction->setVisible(false);
        editCardAction->setVisible(false);
        removeActAction->setVisible(false);
        removeCardAction->setVisible(false);
    } else {
        addActAction->setVisible(false);
        addCardAction->setVisible(false);
        if (qgraphicsitem_cast<ActItem*>(selectedItems().first())) {
            editCardAction->setVisible(false);
            removeCardAction->setVisible(false);
        } else if (CardItem* card = qgraphicsitem_cast<CardItem*>(selectedItems().first())) {
            card->putOnBoard();
            //
            editActAction->setVisible(false);
            removeActAction->setVisible(false);
            //
            // ... виджет редактирования штампа
            //
            QWidgetAction* stateAction = new QWidgetAction(menu);
            QWidget* stampWidget = new QWidget;
            stampWidget->setStyleSheet("margin-left:30px; min-height:24px;");
            QLineEdit* stampEditor = new QLineEdit(stampWidget);
            stampEditor->setText(card->stamp());
            connect(stampEditor, &QLineEdit::textChanged, [=] (const QString& _text) {
                if (CardItem* card = qgraphicsitem_cast<CardItem*>(selectedItems().first())) {
                    card->setStamp(_text);
                }
            });
            QVBoxLayout* stampLayout = new QVBoxLayout(stampWidget);
            stampLayout->setContentsMargins(QMargins());
            stampLayout->setSpacing(0);
            stampLayout->addWidget(new QLabel(tr("Stamp:"), stampWidget));
            stampLayout->addWidget(stampEditor);
            stateAction->setDefaultWidget(stampWidget);
            menu->insertAction(editCardAction, stateAction);
            menu->insertSeparator(editCardAction);
        }
    }
    //
    // ... если нельзя добавлять акты, то принудительно скрываем все действия над ними
    //
    if (!m_isCanAddActs) {
        addActAction->setVisible(false);
        editActAction->setVisible(false);
        removeActAction->setVisible(false);
    }

    //
    // Покажем контекстное меню
    //
    QAction* triggered = menu->exec(_event->screenPos());

    //
    // Определим выбранное действие
    //
    if (triggered != nullptr) {
        //
        // Пользователь хочет добавить акт
        //
        if (triggered == addActAction) {
            const QPointF insertPosition = fixCollidesForCardPosition(_event->scenePos());
            emit actAddRequest(insertPosition);
        }
        //
        // ... или карточку
        //
        else if (triggered == addCardAction) {
            const QPointF insertPosition = fixCollidesForCardPosition(_event->scenePos());
            emit cardAddRequest(insertPosition);
        }
        //
        // Пользователь хочет изменить акт
        //
        else if (triggered == editActAction) {
            const ActItem* act = qgraphicsitem_cast<ActItem*>(selectedItems().first());
            emit cardChangeRequest(act->uuid());
        }
        //
        // ... или карточку
        //
        else if (triggered == editCardAction) {
            const CardItem* card = qgraphicsitem_cast<CardItem*>(selectedItems().first());
            emit cardChangeRequest(card->uuid());
        }
        //
        // Пользователь хочет удалить акт
        //
        else if (triggered == removeActAction) {
            const ActItem* act = qgraphicsitem_cast<ActItem*>(selectedItems().first());
            emit actRemoveRequest(act->uuid());
        }
        //
        // ... или карточку
        //
        else if (triggered == removeCardAction) {
            const CardItem* card = qgraphicsitem_cast<CardItem*>(selectedItems().first());
            emit cardRemoveRequest(card->uuid());
        }
    }

    //
    // Очищаем память
    //
    menu->deleteLater();
}

void CardsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event)
{
    QGraphicsScene::mouseReleaseEvent(_event);

    //
    // Упорядочим карточки, если нужно
    //
    reorderSelectedItem();
    reorderItemsOnScene();
}

void CardsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event)
{
    //
    // Если ни одного элемента не выделено, добавляем новый в позиции курсора
    //
    if (selectedItems().isEmpty()) {
        const QPointF insertPosition = fixCollidesForCardPosition(_event->scenePos());
        emit cardAddRequest(insertPosition);
    } else {
        //
        // И испускаем сигнал об изменении параметров выделенного элемента
        //
        if (const ActItem* act = qgraphicsitem_cast<ActItem*>(selectedItems().first())) {
            emit actChangeRequest(act->uuid());
        } else if (const CardItem* card = qgraphicsitem_cast<CardItem*>(selectedItems().first())) {
            emit cardChangeRequest(card->uuid());
        }
    }

    QGraphicsScene::mouseDoubleClickEvent(_event);
}

void CardsScene::keyPressEvent(QKeyEvent* _event)
{
    if (_event->key() == Qt::Key_Control) {
        setInDragOutMode(true);
    }
    //
    // Запрос на удаление элемента
    //
    else if (!selectedItems().isEmpty()
             && (_event->key() == Qt::Key_Backspace
                 || _event->key() == Qt::Key_Delete)) {
        if (const ActItem* act = qgraphicsitem_cast<ActItem*>(selectedItems().first())) {
            emit actRemoveRequest(act->uuid());
        } else if (const CardItem* card = qgraphicsitem_cast<CardItem*>(selectedItems().first())) {
            emit cardRemoveRequest(card->uuid());
        }
    }

    QGraphicsScene::keyPressEvent(_event);
}

void CardsScene::keyReleaseEvent(QKeyEvent* _event)
{
    if (_event->key() == Qt::Key_Control) {
        setInDragOutMode(false);
    }

    QGraphicsScene::keyReleaseEvent(_event);
}

void CardsScene::focusOutEvent(QFocusEvent* _event)
{
    //
    // При потере фокуса снимаем флаг перетаскивания между сценами, если он был установлен
    //
    setInDragOutMode(false);

    QGraphicsScene::focusOutEvent(_event);
}

void CardsScene::dropEvent(QGraphicsSceneDragDropEvent* _event)
{
    //
    // Обрабатываем событие только если оно пришло из другой сцены
    //
    if (!m_isInDragOutMode) {
        clearSelection();

        //
        // Посылаем сигнал с запросом на добавление карточки
        //
        QByteArray byteArray = _event->mimeData()->data(CardItem::MimeType);
        if (!byteArray.isEmpty()) {
            const CardItem card(byteArray);
            const QRectF cardRect = card.boundingRect();
            QPointF position(_event->scenePos() - QPointF(cardRect.width()/2, cardRect.height()/2));
            position = fixCollidesForCardPosition(position);
            emit cardAddCopyRequest(card.isFolder(), card.title(), card.description(), card.stamp(), card.colors(), position);
        }
    }
}

void CardsScene::reorderSelectedItem()
{
    if (!selectedItems().isEmpty()) {
        if (CardItem* movedCard = qgraphicsitem_cast<CardItem*>(selectedItems().first())) {
            //
            // Определим место, куда перемещена карточка
            //
            const QPointF movedCardPosition = movedCard->pos();
            const QRectF movedCardRect = movedCard->boundingRect();
            const qreal movedCardLeft = movedCardPosition.x();
            const qreal movedCardTop = movedCardPosition.y();
            const qreal movedCardBottom = movedCardTop + movedCardRect.height();

            //
            // Ищем элемент, который будет последним перед карточкой в её новом месте
            //
            QGraphicsItem* previousItem = nullptr;
            for (QGraphicsItem* item : m_items) {
                if (item != movedCard) {
                    const QPointF itemPosition = item->pos();
                    const QRectF itemRect = item->boundingRect();
                    const qreal itemLeft = itemPosition.x();
                    const qreal itemTop = itemPosition.y();
                    const qreal itemBottom = itemTop + itemRect.height();

                    //
                    // Если после акта
                    //
                    if (qgraphicsitem_cast<ActItem*>(item)
                        && movedCardTop > itemTop) { // ниже акта
                        previousItem = item;
                    }
                    //
                    // Если после карточки
                    //
                    else if (qgraphicsitem_cast<CardItem*>(item)
                                 // на разных линиях
                             && ((movedCardTop > itemTop
                                  && fabs(movedCardTop - itemTop) >= movedCardRect.height()/2.)
                                 // на одной линии, но левее
                                 || (movedCardTop < itemBottom
                                     && movedCardBottom > itemTop
                                     && fabs(movedCardTop - itemTop) < movedCardRect.height()/2.
                                     && movedCardLeft > itemLeft))) {
                        previousItem = item;
                    }
                    //
                    // Если не после данного элемента, то прерываем поиск
                    //
                    else {
                        break;
                    }
                }
            }

            //
            // Перемещаем карточку в списке
            //
            const int cardCurrentIndex = m_items.indexOf(movedCard);
            int cardNewIndex = 0;
            if (previousItem != nullptr) {
                cardNewIndex = m_items.indexOf(previousItem) + 1;
                if (cardNewIndex > cardCurrentIndex) {
                    --cardNewIndex;
                }
            }

            //
            // Если карточка в самом деле сместилась
            //
            if (cardNewIndex != cardCurrentIndex) {
                m_items.move(cardCurrentIndex, cardNewIndex);

                //
                // Уведомляем подписчиков о перемещении
                //
                QString actId;
                QString previusCardId;
                if (previousItem != nullptr) {
                    if (ActItem* act = qgraphicsitem_cast<ActItem*>(previousItem)) {
                        actId = act->uuid();
                    } else if (CardItem* card = qgraphicsitem_cast<CardItem*>(previousItem)) {
                        previusCardId = card->uuid();
                        if (card->isEmbedded()) {
                            for (int searchIndex = cardNewIndex; searchIndex >= 0; --searchIndex) {
                                if (ActItem* act = qgraphicsitem_cast<ActItem*>(m_items[searchIndex])) {
                                    actId = act->uuid();
                                    break;
                                }
                            }
                        }
                    }
                }
                emit cardMoved(movedCard->uuid(), actId, previusCardId);
            }
        }
    }
}

void CardsScene::reorderItemsOnScene()
{
    if (!m_isFixedMode) {
        return;
    }

    //
    // Упорядочиваем в такой последовательности:
    //
    // Акт <---------------
    // |                  |
    // v                  |
    // Вложенные карточки |
    //

    //
    // Определим размеры акта и карточки
    //
    QRectF actRect = ActItem().boundingRect();
    if (!views().isEmpty()) {
        const QGraphicsView* view = views().first();
        const QPointF viewTopLeftPoint = view->mapToScene(QPoint(0, 0));
        const int scrollDelta = view->verticalScrollBar()->isVisible() ? view->verticalScrollBar()->width() : 0;
        const QPointF viewTopRightPoint = view->mapToScene(QPoint(view->width() - scrollDelta, 0));
        actRect.setLeft(viewTopLeftPoint.x());
        actRect.setWidth(viewTopRightPoint.x() - viewTopLeftPoint.x());
    }
    const QRectF cardRect = CardItem().boundingRect();

    //
    // Определим количество карточек в ряду
    //
    int cardsInRowCount = m_cardsInRowCount;
    if (cardsInRowCount == 0) {
        cardsInRowCount = actRect.width() / (cardRect.width() + m_cardsDistance);
    }

    int x = sceneRect().left() + m_cardsDistance;
    int y = sceneRect().top() + m_cardsDistance;
    int lastItemHeight = -1;
    int currentCardInRow = 0;
    bool lastCardIsEmbedded = false;

    //
    // Проходим все элементы (они упорядочены так, как должны идти элементы в сценарии
    //
    for (QGraphicsItem* item : m_items) {
        //
        // Если текущий элемент акт
        //
        if (ActItem* act = qgraphicsitem_cast<ActItem*>(item)) {
            //
            // ... возвращаем х в начальное положение
            //
            x = sceneRect().left() + m_cardsDistance;
            //
            // ... если вставляется после карточки, то добавим вертикальный отступ
            //
            if (lastItemHeight > 0) {
                y += lastItemHeight + m_cardsDistance;
            }
            //
            // ... если вставляется после другого акта, то оставим место для того, чтобы можно было встроить карточку
            //
            else if (lastItemHeight == 0) {
                y += cardRect.height() + m_cardsDistance;
            }
            //
            // ... сдвигаем акт
            //
            act->setPos(x, y);
//            QPropertyAnimation* moveAnimation = new QPropertyAnimation(act, "pos");
//            moveAnimation->setDuration(100);
//            moveAnimation->setStartValue(act->pos());
//            moveAnimation->setEndValue(QPointF(x, y));
//            moveAnimation->start(QAbstractAnimation::DeleteWhenStopped);
            //
            // ... корректируем координату y и сбрасываем счётчик карточек в ряду
            //
            y += act->boundingRect().height() + m_cardsDistance;
            lastItemHeight = 0;
            currentCardInRow = 0;
            lastCardIsEmbedded = false;
        }
        //
        // Если текущий элемент карточка
        //
        else if (CardItem* card = qgraphicsitem_cast<CardItem*>(item)) {
            //
            // ... корректируем позицию в соответствии с позицией карточки в ряду,
            //     или если предыдущая была вложена, а текущая нет
            //
            if (currentCardInRow == cardsInRowCount
                || (lastCardIsEmbedded == true
                    && card->isEmbedded() == false)) {
                currentCardInRow = 0;
                x = sceneRect().left() + m_cardsDistance;
                y += lastItemHeight + m_cardsDistance;
            }
            ++currentCardInRow;
            //
            // ... позиционируем карточку
            //
            card->setPos(x,  y);
//            QPropertyAnimation* moveAnimation = new QPropertyAnimation(card, "pos");
//            moveAnimation->setDuration(100);
//            moveAnimation->setStartValue(card->pos());
//            moveAnimation->setEndValue(QPointF(x, y));
//            moveAnimation->start(QAbstractAnimation::DeleteWhenStopped);
            //
            // ... и корректируем координаты для позиционирования следующих элементов
            //
            x += card->boundingRect().width() + m_cardsDistance;
            lastItemHeight = card->boundingRect().height();
            lastCardIsEmbedded = card->isEmbedded();
        }
    }
}

QPointF CardsScene::fixCollidesForCardPosition(const QPointF& _position)
{
    //
    // Проверим не будет ли карточка пересекаться с актом
    //
    QRectF cardRect = CardItem().boundingRect();
    cardRect.moveCenter(_position);
    QPointF position = cardRect.topLeft();
    for (const QGraphicsItem* item : items(cardRect)) {
        if (const ActItem* act = qgraphicsitem_cast<const ActItem*>(item)) {
            //
            // ... сместим карточку наверх, если её создают перед актом
            //
            if (_position.y() < act->pos().y()) {
                position.setY(act->pos().y() - cardRect.height());
            }
            //
            // ... или вниз, если после акта
            //
            else {
                position.setY(act->pos().y() + act->boundingRect().height());
            }
        }
    }

    return position;
}

void CardsScene::setInDragOutMode(bool _inDragOutMode)
{
    if (m_isInDragOutMode != _inDragOutMode) {
        m_isInDragOutMode = _inDragOutMode;
        for (QGraphicsItem* item : items()) {
            if (CardItem* card = qgraphicsitem_cast<CardItem*>(item)) {
                card->setInDragOutMode(m_isInDragOutMode);
            }
        }
    }
}
