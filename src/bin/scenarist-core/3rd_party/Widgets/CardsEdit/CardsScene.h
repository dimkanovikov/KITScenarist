#ifndef CARDSSCENE_H
#define CARDSSCENE_H

#include <QGraphicsScene>
#include <QMap>

class ActItem;
class CardItem;


/**
 * @brief Сцена с черновиком
 */
class CardsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit CardsScene(QObject *parent = 0);

    /**
     * @brief Задать размер карточек
     */
    void setCardsSize(const QSizeF& _size);

    /**
     * @brief Задать расстояние между элементами
     */
    void setCardsDistance(qreal _distance);

    /**
     * @brief Установить количество карточек в ряду
     */
    void setCardsInRow(int _count);

    /**
     * @brief Установить доступность возможности добавления актов
     */
    void setCanAddActs(bool _can);

    /**
     * @brief Установить режим привязки карточек к сетке
     */
    void setFixedMode(bool _isFixed);

    /**
     * @brief Получить идентификатор последнего элемента
     */
    QString lastItemUuid() const;

    /**
     * @brief Добавить акт
     */
    void addAct(const QString& _uuid, const QString& _title, const QString& _description, const QString& _colors);

    /**
     * @brief Вставить акт после заданного элемента
     */
    void insertAct(const QString& _uuid, const QString& _title, const QString& _description, const QString& _colors, const QString& _previousItemUuid);

    /**
     * @brief Добавить карточку
     */
    void addCard(const QString& _uuid, bool _isFolder, const QString& _title, const QString& _description,
        const QString& _stamp, const QString& _colors, bool _isEmbedded, const QPointF& _position);

    /**
     * @brief Вставить карточку после заданного элемента
     */
    void insertCard(const QString& _uuid, bool _isFolder, const QString& _title, const QString& _description,
        const QString& _stamp, const QString& _colors, bool _isEmbedded, const QPointF& _position, const QString& _previousItemUuid);

    /**
     * @brief Обновить заданную карточку
     */
    void updateItem(const QString& _uuid, bool _isFolder, const QString& _title, const QString& _description,
        const QString& _stamp, const QString& _colors, bool _isEmbedded, bool _isAct);

    /**
     * @brief Удалить элемент по идентификатору
     */
    void removeSceneItem(const QString& _uuid);

    /**
     * @brief Удалить акт
     */
    void removeAct(const QString& _uuid);

    /**
     * @brief Удалить карточку
     */
    void removeCard(const QString& _uuid);

    /**
     * @brief Обновить сцену
     */
    void refresh();

    /**
     * @brief Сформировать xml на основе сцены
     */
    QString save() const;

    /**
     * @brief Загрузить сцены из xml
     */
    bool load(const QString& _xml);

signals:
    /**
     * @brief Пользователь хочет добавить элемент в заданной позиции
     */
    /** @{ */
    void actAddRequest(const QPointF& _position);
    void cardAddRequest(const QPointF& _position);
    /** @} */

    /**
     * @brief Пользователь хочет добавить копию карточки в заданной позиции
     */
    void cardAddCopyRequest(bool _isFolder, const QString& _title, const QString& _description, const QString& _state,
        const QString& _colors, const QPointF& _position);

    /**
     * @brief Элемент был добавлен
     */
    /** @{ */
    void actAdded(const QString& _uuid);
    void cardAdded(const QString& _uuid);
    /** @} */

    /**
     * @brief Пользователь хочет изменить данные акта/карточки
     */
    /** @{ */
    void actChangeRequest(const QString& _uuid);
    void cardChangeRequest(const QString& _uuid);
    /** @} */

    /**
     * @brief Данные акта/карточки были изменены
     */
    /** @{ */
    void actChanged(const QString& _uuid);
    void cardChanged(const QString& _uuid);
    /** @} */

    /**
     * @brief Пользователь хочет удалить акт/карточку
     */
    /** @{ */
    void actRemoveRequest(const QString& _uuid);
    void cardRemoveRequest(const QString& _uuid);
    /** @} */

    /**
     * @brief Акт/карточка была удалена
     */
    /** @{ */
    void actRemoved(const QString& _uuid);
    void cardRemoved(const QString& _uuid);
    /** @} */

    /**
     * @brief Карточка перенесена
     * @param _cardId идентификатор карточки
     * @param _actId идентификатор акта, QString::null если карточка выносится за пределы акта
     * @param _previousCardId идентификатор карточки, после которой будет вставлена перетаскиваемая
     *                        карточка, QString::null если карточка вставляется в начало акта
     */
    void cardMoved(const QString& _cardId, const QString& _actId, const QString& _previousCardId);

    /**
     * @brief Картосчка была вложена в группирующую карточку
     */
    void cardMovedToGroup(const QString& _cardId, const QString& _groupId);

    /**
     * @brief Изменились цвета карточки
     */
    void cardColorsChanged(const QString& _uuid, const QString& _colors);

protected:
    /**
     * @brief Отображаем собственное контекстное меню
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* _event) override;

    /**
     * @brief Переопределяем для опеределения новой позиции элемента в схеме
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;

    /**
     * @brief Переопределяем для добавления карточки, если ни одна не выделена,
     *        либо для испускания сиграла о том, что пользователь хочет изменить выбранную карточку
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) override;

    /**
     * @brief Переопределяем для инициилизации события перетаскивания карточек между сценами
     */
    /** @{ */
    void keyPressEvent(QKeyEvent* _event);
    void keyReleaseEvent(QKeyEvent* _event);
    void focusOutEvent(QFocusEvent* _event);
    /** @} */

    /**
     * @brief Переопределяем для реализации возможности перетаскивания карточек между сценами
     */
    /** @{ */
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent*) {}
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent*) {}
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent*) {}
    virtual void dropEvent(QGraphicsSceneDragDropEvent* _event);
    /** @} */

private:
    /**
     * @brief Определить позицию, где должна находиться выделенная карточка
     */
    void reorderSelectedItem();

    /**
     * @brief Упорядочить элементы по сетке
     */
    void reorderItemsOnScene();

    /**
     * @brief Исключить возможность пересечения добавляемой карточки с актом
     */
    QPointF fixCollidesForCardPosition(const QPointF& _position);

    /**
     * @brief Включить/отключить режим перетаскивания карточек между сценами
     */
    void setInDragOutMode(bool _inDragOutMode);

private:
    /**
     * @brief Размер карточек
     */
    QSizeF m_cardsSize = QSizeF(200, 150);

    /**
     * @brief Можно ли добавлять акты в сцену
     */
    bool m_isCanAddActs = false;

    /**
     * @brief Включён ли режим привязки к сетке
     */
    bool m_isFixedMode = false;

    /**
     * @brief Расстояние между элементами в режиме привязки к сетке
     */
    qreal m_cardsDistance = 30.;

    /**
     * @brief Количество карточек в ряду в режиме привязки к сетке
     */
    int m_cardsInRowCount = 0;

    /**
     * @brief Список элементов сцены
     */
    QVector<QGraphicsItem*> m_items;

    /**
     * @brief Навигационная карта по элементам <uuid, item>
     */
    QMap<QString, QGraphicsItem*> m_itemsMap;

    /**
     * @brief Находятся ли карточки сцены в режиме перемещения между сценами
     */
    bool m_isInDragOutMode = false;

    /**
     * @brief Заблокированы ли изменения извне
     */
    bool m_isChangesBlocked = false;
};

#endif // CARDSSCENE_H
