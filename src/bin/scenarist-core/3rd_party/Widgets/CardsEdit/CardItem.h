#ifndef CARDITEM_H
#define CARDITEM_H

#include <QGraphicsItem>

#include <QGraphicsDropShadowEffect>


/**
 * @brief Элемент сцены являющий собой сцену или папку сценария
 */
class CardItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    /**
     * @brief Тип карточки
     */
    static const int Type = UserType + 15;

    /**
     * @brief Майм тип карточки
     */
    static const QString MimeType;

public:
    explicit CardItem(QGraphicsItem* _parent = 0);
    explicit CardItem(const QByteArray& mimeData, QGraphicsItem* _parent = 0);

    /**
     * @brief Идентификатор
     */
    /** @{ */
    void setUuid(const QString& _uuid);
    QString uuid() const;
    /** @} */

    /**
     * @brief Является ли карточка группирующей
     */
    /** @{ */
    void setIsFolder(bool _isFolder);
    bool isFolder() const;
    /** @} */

    /**
     * @brief Заголовок
     */
    /** @{ */
    void setTitle(const QString& _title);
    QString title() const;
    /** @} */

    /**
     * @brief Описание
     */
    /** @{ */
    void setDescription(const QString& _description);
    QString description() const;
    /** @} */

    /**
     * @brief Штамп
     */
    /** @{ */
    void setStamp(const QString& _stamp);
    QString stamp() const;
    /** @} */

    /**
     * @brief Цвета
     */
    /** @{ */
    void setColors(const QString& _colors);
    QString colors() const;
    /** @} */

    /**
     * @brief Установить размер карточки
     */
    void setSize(const QSizeF& _size);

    /**
     * @brief Установить режим перемещения между сценами (true) или по сцене (false)
     */
    void setInDragOutMode(bool _inDragOutMode);

    /**
     * @brief Переопределяем метод, чтобы работал qgraphicsitem_cast
     */
    int type() const override;

    /**
     * @brief Область занимаемая карточкой
     */
    QRectF boundingRect() const override;

    /**
     * @brief Область занимаемая карточкой с учётом резервной области под декорацию папки
     */
    QRectF boundingRectCorrected() const;

    /**
     * @brief Отрисовка карточки
     */
    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) override;

    /**
     * @brief Взять карточку с доски
     */
    void takeFromBoard();

    /**
     * @brief Положить карточку обратно на доску
     */
    void putOnBoard();

protected:
    /**
     * @brief Переопределяем для реализации возможности перетаскивания сцен между двумя сценами, а так же для анимации выделения
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;

    /**
     * @brief Переопределяем для реализации анимации снятия выделения
     */
    /** @{ */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;
    /** @} */

private:
    /**
     * @brief Идентификатор
     */
    QString m_uuid;

    /**
     * @brief Является ли карточка папкой (true) или сценой (false)
     */
    bool m_isFolder = false;

    /**
     * @brief Заголовок карточки
     */
    QString m_title;

    /**
     * @brief Описание карточки
     */
    QString m_description;

    /**
     * @brief Штамп на карточке
     */
    QString m_stamp;

    /**
     * @brief Цвета карточки
     */
    QString m_colors;

    /**
     * @brief Размер карточки
     */
    QSizeF m_size = QSizeF(200, 150);

    /**
     * @brief Находится ли карточка в состояния переноса между сценами
     */
    bool m_isInDragOutMode = false;

    /**
     * @brief Эффект отбрасывания тени
     */
    QScopedPointer<QGraphicsDropShadowEffect> m_shadowEffect;
};

#endif // CARDITEM_H
