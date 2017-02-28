#ifndef ACTITEM_H
#define ACTITEM_H

#include <QGraphicsItem>

#include <QGraphicsDropShadowEffect>


/**
 * @brief Элемент сцены являющий собой акт сценария
 */
class ActItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    /**
     * @brief Тип акта
     */
    static const int Type = UserType + 16;

public:
    explicit ActItem(QGraphicsItem* _parent = 0);

    /**
     * @brief Идентификатор
     */
    /** @{ */
    void setUuid(const QString& _uuid);
    QString uuid() const;
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
     * @brief Цвета
     */
    /** @{ */
    void setColors(const QString& _colors);
    QString colors() const;
    /** @} */

    /**
     * @brief Переопределяем метод, чтобы работал qgraphicsitem_cast
     */
    int type() const override;

    /**
     * @brief Область занимаемая актом
     */
    /** @{ */
    void setBoundingRect(const QRectF& _boundingRect);
    QRectF boundingRect() const override;
    /** @} */

    /**
     * @brief Отрисовка акта
     */
    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) override;

    /**
     * @brief Переопределяем, чтобы смещать акт строго по вертикали и не пересекать с любыми другими элементами сцены
     */
    QVariant itemChange(GraphicsItemChange _change, const QVariant& _value) override;

private:
    /**
     * @brief Идентификатор
     */
    QString m_uuid;

    /**
     * @brief Заголовок акта
     */
    QString m_title;

    /**
     * @brief Описание акта
     */
    QString m_description;

    /**
     * @brief Цвета акта
     */
    QString m_colors;

    /**
     * @brief Область отрисовки
     */
    mutable QRectF m_boundingRect = QRectF(0, 0, 100, 30);

    /**
     * @brief Эффект отбрасывания тени
     */
    QScopedPointer<QGraphicsDropShadowEffect> m_shadowEffect;
};

#endif // ACTITEM_H
