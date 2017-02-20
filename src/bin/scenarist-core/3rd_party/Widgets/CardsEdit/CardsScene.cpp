#include "CardsScene.h"

#include "ActItem.h"
#include "CardItem.h"

#include <3rd_party/Widgets/ColoredToolButton/GoogleColorsPane.h>

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

namespace {
    /**
     * @brief Обновить область отрисовки акта
     */
    void updateActBoundingRect(const QRectF& _sceneRect, ActItem* _act) {
        QRectF actRect = _act->boundingRect();
        actRect.setLeft(_sceneRect.left());
        actRect.setWidth(_sceneRect.width());
        _act->setBoundingRect(actRect);
    }
}


CardsScene::CardsScene(QObject *parent) :
    QGraphicsScene(parent)
{
    setItemIndexMethod(QGraphicsScene::NoIndex);

    connect(this, &CardsScene::sceneRectChanged, [=] (const QRectF& _rect) {
        //
        // Сохраним новое значение
        //
        m_sceneRect = _rect;

        //
        // Скорректируем области папок
        //
        for (QGraphicsItem* item : m_items) {
            if (ActItem* act = qgraphicsitem_cast<ActItem*>(item)) {
                updateActBoundingRect(m_sceneRect, act);
            }
        }
    });
}

CardsScene::~CardsScene()
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

bool CardsScene::isFixedMode() const
{
    return m_isFixedMode;
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
    if (m_isChangesBlocked) {
        return;
    }

    //
    // Вставим акт после самого последнего элемента
    //
    QString previousItemUuid = lastItemUuid();
    insertAct(_uuid, _title, _description, _colors, previousItemUuid);
}

void CardsScene::insertAct(const QString& _uuid, const QString& _title, const QString& _description,
    const QString& _colors, const QString& _previousItemUuid)
{
    if (m_isChangesBlocked) {
        return;
    }

    if (m_itemsMap.contains(_uuid)) {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Try to add contained item to scene");
    }

    ActItem* act = new ActItem;
    act->setUuid(_uuid);
    act->setTitle(_title);
    act->setDescription(_description);
    act->setColors(_colors);
    updateActBoundingRect(m_sceneRect, act);

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

void CardsScene::addCard(const QString& _uuid, bool _isFolder, int _number, const QString& _title,
    const QString& _description, const QString& _stamp, const QString& _colors, bool _isEmbedded,
    const QPointF& _position)
{
    if (m_isChangesBlocked) {
        return;
    }

    //
    // Вставим карточку после самого последнего элемента
    //
    QString previousItemUuid = lastItemUuid();
    insertCard(_uuid, _isFolder, _number, _title, _description, _stamp, _colors, _isEmbedded, _position, previousItemUuid);
}

void CardsScene::insertCard(const QString& _uuid, bool _isFolder, int _number, const QString& _title,
    const QString& _description, const QString& _stamp, const QString& _colors, bool _isEmbedded,
    const QPointF& _position, const QString& _previousItemUuid)
{
    if (m_isChangesBlocked) {
        return;
    }

    if (m_itemsMap.contains(_uuid)) {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Try to add contained item to scene");
    }

    CardItem* card = new CardItem;
    card->setUuid(_uuid);
    card->setIsFolder(_isFolder);
    card->setNumber(_number);
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
        // Если карточка кладётся после акта без вложения
        //
        if (previousItem->type() == ActItem::Type
            && _isEmbedded == false) {
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

void CardsScene::updateItem(const QString& _uuid, bool _isFolder, int _number, const QString& _title,
    const QString& _description, const QString& _stamp, const QString& _colors, bool _isEmbedded,
    bool _isAct)
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
                insertCard(_uuid, _isFolder, _number, _title, _description, _stamp, _colors, false, QPointF(), previousItemUuid);
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
                card->setNumber(_number);
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
    if (m_isChangesBlocked) {
        return;
    }

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
    if (m_isChangesBlocked) {
        return;
    }

    if (m_itemsMap.contains(_uuid)) {
        if (ActItem* act = qgraphicsitem_cast<ActItem*>(m_itemsMap.take(_uuid))) {
            //
            // Удаляем сам акт
            //
            removeItem(act);
            m_items.removeAll(act);
            act->deleteLater();

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
    if (m_isChangesBlocked) {
        return;
    }

    if (m_itemsMap.contains(_uuid)) {
        if (CardItem* card = qgraphicsitem_cast<CardItem*>(m_itemsMap.take(_uuid))) {
            //
            // Удаляем саму карточку
            //
            removeItem(card);
            m_items.removeAll(card);
            card->deleteLater();

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

void CardsScene::removeSelectedItem()
{
    if (!selectedItems().isEmpty()) {
        QGraphicsItem* item = selectedItems().first();
        if (ActItem* act = qgraphicsitem_cast<ActItem*>(item)) {
            emit actRemoveRequest(act->uuid());
        } else if (CardItem* card = qgraphicsitem_cast<CardItem*>(item)) {
            emit cardRemoveRequest(card->uuid());
        }
    }
}

void CardsScene::refresh()
{
    reorderItemsOnScene();
}

void CardsScene::updateActs()
{
    for (QGraphicsItem* item : m_items) {
        if (ActItem* act = qgraphicsitem_cast<ActItem*>(item)) {
            act->update();
        }
    }
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
            writer.writeAttribute("number", QString::number(card->number()));
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
    m_itemsMap.clear();


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
            const int number = attributes.namedItem("number").toAttr().value().toInt();
            const QString title = attributes.namedItem("title").toAttr().value();
            const QString description = attributes.namedItem("description").toAttr().value();
            const QString stamp = attributes.namedItem("stamp").toAttr().value();
            const QString colors = attributes.namedItem("colors").toAttr().value();
            const bool isEmbedded = attributes.namedItem("is_embedded").toAttr().value() == "true";
            const qreal x = attributes.namedItem("x").toAttr().value().toDouble();
            const qreal y = attributes.namedItem("y").toAttr().value().toDouble();
            addCard(uuid, isFolder, number, title, description, stamp, colors, isEmbedded, QPointF(x, y));
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

    const bool hasSelectedItem = !selectedItems().isEmpty();
    ActItem* act = nullptr;
    CardItem* card = nullptr;
    if (hasSelectedItem) {
        act = qgraphicsitem_cast<ActItem*>(selectedItems().first());
        card = qgraphicsitem_cast<CardItem*>(selectedItems().first());
    }

    //
    // Сформируем контекстное меню в зависимости от того, выбраны ли карточки на экране
    //
    QMenu* menu = new QMenu(views().value(0, nullptr));

    //
    // Возможность конвертирования
    //
    QAction* convertToSceneAction = menu->addAction(tr("Convert to scene"));
    QAction* convertToFolderAction = menu->addAction(tr("Convert to folder"));

    //
    // Настройка карточки
    //
    QAction* propertiesSeparator = menu->addSeparator();
    QAction* editAction = menu->addAction(tr("Edit"));
    //
    // Цвета
    //
    QString colorsNames;
    if (act != nullptr) {
        colorsNames = act->colors();
    } else if (card != nullptr) {
        colorsNames = card->colors();
    }
    int colorIndex = 1;
    QList<GoogleColorsPane*> colorsPanesList;
    //
    // ... добавляем каждый цвет
    //
    foreach (const QString& colorName, colorsNames.split(";", QString::SkipEmptyParts)) {
        QAction* color = menu->addAction(tr("Color %1").arg(colorIndex));
        QMenu* colorMenu = new QMenu(views().value(0, nullptr));
        QAction* removeColor = colorMenu->addAction(tr("Remove"));
        removeColor->setData(QString("removeColor:%1").arg(colorIndex));
        QWidgetAction* wa = new QWidgetAction(colorMenu);
        GoogleColorsPane* colorsPane = new GoogleColorsPane(colorMenu);
        colorsPane->setCurrentColor(QColor(colorName));
        wa->setDefaultWidget(colorsPane);
        colorMenu->addAction(wa);
        color->setMenu(colorMenu);

        connect(colorsPane, &GoogleColorsPane::selected, menu, &QMenu::close);

        colorsPanesList.append(colorsPane);

        ++colorIndex;
    }
    //
    // ... пункт для нового цвета
    //
    QAction* color = menu->addAction(tr("Add color"));
    {
        QMenu* colorMenu = new QMenu(views().value(0, nullptr));
        QWidgetAction* wa = new QWidgetAction(colorMenu);
        GoogleColorsPane* colorsPane = new GoogleColorsPane(colorMenu);
        wa->setDefaultWidget(colorsPane);
        colorMenu->addAction(wa);
        color->setMenu(colorMenu);

        connect(colorsPane, &GoogleColorsPane::selected, menu, &QMenu::close);

        colorsPanesList.append(colorsPane);
    }

    //
    // Остальное
    //
    QAction* endSeparator = menu->addSeparator();
    QAction* addAction = menu->addAction(tr("Create card"));
    QAction* removeAction = menu->addAction(tr("Remove"));

    //
    // Определим видимые
    //
    if (!hasSelectedItem) {
        convertToSceneAction->setVisible(false);
        convertToFolderAction->setVisible(false);
        color->setVisible(false);
        propertiesSeparator->setVisible(false);
        endSeparator->setVisible(false);
        editAction->setVisible(false);
        removeAction->setVisible(false);
    } else {
        if (act != nullptr) {
            convertToFolderAction->setVisible(false);
            convertToSceneAction->setVisible(false);
            propertiesSeparator->setVisible(false);
        }
        if (card != nullptr) {
            card->putOnBoard();
            if (card->isFolder()) {
                convertToFolderAction->setVisible(false);
            } else {
                convertToSceneAction->setVisible(false);
            }

            //
            // ... виджет редактирования штампа
            //
            QWidgetAction* stateAction = new QWidgetAction(menu);
            QWidget* stampWidget = new QWidget;
            QLineEdit* stampEditor = new QLineEdit(stampWidget);
            stampEditor->setText(card->stamp());
            connect(stampEditor, &QLineEdit::textChanged, [=] (const QString& _text) {
                card->setStamp(_text);
                emit cardsChanged();
            });
            QHBoxLayout* stampLayout = new QHBoxLayout(stampWidget);
            stampLayout->setContentsMargins(QMargins(27, 2, 5, 3));
            stampLayout->setSpacing(2);
            stampLayout->addWidget(new QLabel(tr("Stamp:"), stampWidget));
            stampLayout->addWidget(stampEditor);
            stateAction->setDefaultWidget(stampWidget);
            menu->insertAction(menu->actions().at(4), stateAction);
        }
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
        // Пользователь хочет добавить карточку
        //
        if (triggered == addAction) {
            const QPointF insertPosition = fixCollidesForCardPosition(_event->scenePos());
            emit cardAddRequest(insertPosition);
        }
        //
        // Пользователь хочет изменить карточку
        //
        else if (triggered == editAction) {
            if (act != nullptr) {
                emit actChangeRequest(act->uuid());
            } else if (card != nullptr) {
                emit cardChangeRequest(card->uuid());
            }
        }
        //
        // Пользователь хочет удалить карточку
        //
        else if (triggered == removeAction) {
            if (act != nullptr) {
                emit actRemoveRequest(act->uuid());
            } else if (card != nullptr) {
                emit cardRemoveRequest(card->uuid());
            }
        }
        //
        // Сменить тип карточки на папку
        //
        else if (triggered == convertToFolderAction ) {
            const QString uuid = act != nullptr ? act->uuid() : card->uuid();
            emit cardTypeChanged(uuid, true);
        }
        //
        // ... или на сцену
        //
        else if (triggered == convertToSceneAction) {
            const QString uuid = act != nullptr ? act->uuid() : card->uuid();
            emit cardTypeChanged(uuid, false);
        }
        //
        // Удалить цвет
        //
        else if (triggered->data().toString().startsWith("removeColor")) {
            //
            // Удаляем выбранный цвет из списка и обновляемся
            //
            const int removeColorIndex = triggered->data().toString().split(":").last().toInt();
            QString newColorsNames;
            int colorIndex = 1;
            foreach (const QString& colorName, colorsNames.split(";", QString::SkipEmptyParts)) {
                if (colorIndex != removeColorIndex) {
                    if (!newColorsNames.isEmpty()) {
                        newColorsNames.append(";");
                    }
                    newColorsNames.append(colorName);
                }

                ++colorIndex;
            }

            const QString uuid = act != nullptr ? act->uuid() : card->uuid();
            emit cardColorsChanged(uuid, newColorsNames);
        }
    }
    //
    // Возможно было изменение цвета
    //
    else {
        if (hasSelectedItem) {
            //
            // Добавляем новый цвет и обновляемся
            //
            QString newColorsNames;
            foreach (GoogleColorsPane* colorsPane, colorsPanesList) {
                if (colorsPane->currentColor().isValid()) {
                    if (!newColorsNames.isEmpty()) {
                        newColorsNames.append(";");
                    }
                    newColorsNames.append(colorsPane->currentColor().name());
                }
            }
            const QString uuid = act != nullptr ? act->uuid() : card->uuid();
            emit cardColorsChanged(uuid, newColorsNames);
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

    emit cardsChanged();
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

                m_isChangesBlocked = true;
                emit cardMoved(movedCard->uuid(), actId, previusCardId);
                m_isChangesBlocked = false;
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
    // Определим размер окна
    //
    qreal viewWidth = 100;
    if (!views().isEmpty()) {
        const QGraphicsView* view = views().first();
        const QPointF viewTopLeftPoint = view->mapToScene(QPoint(0, 0));
        const int scrollDelta = view->verticalScrollBar()->isVisible() ? view->verticalScrollBar()->width() : 0;
        const QPointF viewTopRightPoint = view->mapToScene(QPoint(view->width() - scrollDelta, 0));
        viewWidth = viewTopRightPoint.x() - viewTopLeftPoint.x();
    }

    //
    // Определим количество карточек в ряду
    //
    int cardsInRowCount = m_cardsInRowCount;
    if (cardsInRowCount == 0) {
        cardsInRowCount = viewWidth / (m_cardsSize.width() + m_cardsDistance);
        if (cardsInRowCount < 1) {
            cardsInRowCount = 1;
        }
    }

    int x = m_sceneRect.left() + m_cardsDistance;
    int y = m_sceneRect.top() + m_cardsDistance;
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
            x = m_sceneRect.left() + m_cardsDistance;
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
                y += m_cardsSize.height() + m_cardsDistance;
            }
            //
            // ... сдвигаем акт
            //
            QPropertyAnimation* moveAnimation = new QPropertyAnimation(act, "pos");
            moveAnimation->setDuration(100);
            moveAnimation->setStartValue(act->pos());
            moveAnimation->setEndValue(QPointF(x, y));
            moveAnimation->start(QAbstractAnimation::DeleteWhenStopped);
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
                x = m_sceneRect.left() + m_cardsDistance;
                y += lastItemHeight + m_cardsDistance;
            }
            ++currentCardInRow;
            //
            // ... позиционируем карточку
            //
            QPropertyAnimation* moveAnimation = new QPropertyAnimation(card, "pos");
            moveAnimation->setDuration(100);
            moveAnimation->setStartValue(card->pos());
            moveAnimation->setEndValue(QPointF(x, y));
            moveAnimation->start(QAbstractAnimation::DeleteWhenStopped);
            //
            // ... и корректируем координаты для позиционирования следующих элементов
            //
            x += m_cardsSize.width() + m_cardsDistance;
            lastItemHeight = m_cardsSize.height();
            lastCardIsEmbedded = card->isEmbedded();
        }
    }

    setSceneRect(QRectF());
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
        for (QGraphicsItem* item : m_items) {
            if (CardItem* card = qgraphicsitem_cast<CardItem*>(item)) {
                card->setInDragOutMode(m_isInDragOutMode);
            }
        }
    }
}
