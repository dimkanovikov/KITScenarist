#include "CardItem.h"

#include "ActItem.h"
#include "CardsScene.h"

#include "TextUtils.h"

#include <3rd_party/Helpers/ImageHelper.h>

#include <QApplication>
#include <QDrag>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QPainter>
#include <QPropertyAnimation>
#include <QStyleOptionGraphicsItem>


const QString CardItem::MimeType = "application/kit-card";

CardItem::CardItem(QGraphicsItem* _parent) :
    QObject(),
    QGraphicsItem(_parent),
    m_shadowEffect(new QGraphicsDropShadowEffect),
    m_animation(new QParallelAnimationGroup)
{
    setCursor(Qt::OpenHandCursor);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    setAcceptHoverEvents(true);

    m_shadowEffect->setBlurRadius(7);
    m_shadowEffect->setXOffset(0);
    m_shadowEffect->setYOffset(1);
    setGraphicsEffect(m_shadowEffect.data());
}

CardItem::CardItem(const QByteArray& mimeData, QGraphicsItem* _parent) :
    QObject(),
    QGraphicsItem(_parent),
    m_shadowEffect(new QGraphicsDropShadowEffect)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    setAcceptHoverEvents(true);

    QDataStream mimeStream(mimeData);
    mimeStream >> m_isFolder >> m_title >> m_description >> m_stamp >> m_colors;

    m_shadowEffect->setBlurRadius(2);
    m_shadowEffect->setOffset(1);
    setGraphicsEffect(m_shadowEffect.data());
}

void CardItem::setUuid(const QString& _uuid)
{
    if (m_uuid != _uuid) {
        m_uuid = _uuid;
    }
}

QString CardItem::uuid() const
{
    return m_uuid;
}

void CardItem::setIsFolder(bool _isFolder)
{
    if (m_isFolder != _isFolder) {
        m_isFolder = _isFolder;
        prepareGeometryChange();
    }
}

bool CardItem::isFolder() const
{
    return m_isFolder;
}

void CardItem::setNumber(int _number)
{
    if (m_number != _number) {
        m_number = _number;
        prepareGeometryChange();
    }
}

int CardItem::number() const
{
    return m_number;
}

void CardItem::setTitle(const QString& _title)
{
    if (m_title != _title) {
        m_title = _title;
        prepareGeometryChange();
    }
}

QString CardItem::title() const
{
    return m_title;
}

void CardItem::setDescription(const QString& _description)
{
    if (m_description != _description) {
        m_description = _description;
        prepareGeometryChange();
    }
}

QString CardItem::description() const
{
    return m_description;
}

void CardItem::setStamp(const QString& _stamp)
{
    if (m_stamp != _stamp) {
        m_stamp = _stamp;
        prepareGeometryChange();
    }
}

QString CardItem::stamp() const
{
    return m_stamp;
}

void CardItem::setColors(const QString& _colors)
{
    if (m_colors != _colors) {
        m_colors = _colors;
        prepareGeometryChange();
    }
}

QString CardItem::colors() const
{
    return m_colors;
}

void CardItem::setIsEmbedded(bool _embedded)
{
    if (m_isEmbedded != _embedded) {
        m_isEmbedded = _embedded;
        prepareGeometryChange();
    }
}

bool CardItem::isEmbedded() const
{
    return m_isEmbedded;
}

void CardItem::setSize(const QSizeF& _size)
{
    if (m_size != _size) {
        m_size = _size;
        prepareGeometryChange();
    }
}

QSizeF CardItem::size() const
{
    return m_size;
}

void CardItem::setInDragOutMode(bool _inDragOutMode)
{
    if (m_isInDragOutMode != _inDragOutMode) {
        m_isInDragOutMode = _inDragOutMode;
    }
}

int CardItem::type() const
{
    return Type;
}

QRectF CardItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), m_size);
}

QRectF CardItem::boundingRectCorrected() const
{
    if (m_isFolder) {
        return boundingRect();
    }
    return boundingRect().adjusted(0, 9, 0, 0);
}

void CardItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget)
{
    Q_UNUSED(_option);
    Q_UNUSED(_widget);

    _painter->save();

    {
        const QPalette palette = QApplication::palette();
        const QRectF cardRect = boundingRect().adjusted(0, 9, 0, 0);
        const QStringList colors = m_colors.split(";", QString::SkipEmptyParts);
        const int additionalColorsHeight = (colors.size() > 1) ? 10 : 0;

        //
        // Рисуем фон
        //
        // ... заданным цветом, если он задан
        //
        if (!colors.isEmpty()) {
            _painter->setBrush(QColor(colors.first()));
            _painter->setPen(QColor(colors.first()));
        }
        //
        // ... или стандартным цветом
        //
        else {
             _painter->setBrush(palette.base());
             _painter->setPen(palette.base().color());
        }
        _painter->drawRect(cardRect);

        //
        // Если это группирующий элемент, рисуем декорацию папки
        //
        if (m_isFolder) {
            if (isSelected()) {
                _painter->setPen(QPen(palette.highlight(), 1));
            } else {
                _painter->setPen(QPen(palette.dark(), 1));
            }
            const int margin = 9;
            for (int distance = margin; distance >= 0; distance -= 3) {
                _painter->drawPolygon(QPolygonF()
                                      << QPointF(distance, margin)
                                      << QPointF(distance, margin - distance)
                                      << QPointF(cardRect.width() - distance, margin - distance)
                                      << QPointF(cardRect.width() - distance, margin)
                                      );
            }
        }

        //
        // Рисуем дополнительные цвета
        //
        if (!m_colors.isEmpty()) {
            QStringList colorsNamesList = m_colors.split(";", QString::SkipEmptyParts);
            colorsNamesList.removeFirst();
            //
            // ... если они есть
            //
            if (!colorsNamesList.isEmpty()) {
                //
                // Выссчитываем ширину занимаемую одним цветом
                //
                const qreal fullCardHeight = boundingRect().height();
                const qreal colorRectWidth = cardRect.width() / colorsNamesList.size();
                QRectF colorRect(0, fullCardHeight - additionalColorsHeight, colorRectWidth, additionalColorsHeight);
                for (const QString& colorName : colorsNamesList) {
                    _painter->fillRect(colorRect, QColor(colorName));
                    colorRect.moveLeft(colorRect.right());
                }
            }
        }

        //
        // Рисуем заголовок
        //
        QTextOption textoption;
        textoption.setAlignment(Qt::AlignTop | Qt::AlignLeft);
        textoption.setWrapMode(QTextOption::NoWrap);
        QFont font = _painter->font();
        font.setBold(true);
        _painter->setFont(font);
        _painter->setPen(palette.text().color());
        const int titleHeight = _painter->fontMetrics().height();
        const QRectF titleRect(7, 18, cardRect.size().width() - 7*2, titleHeight);
        QString titleText = title();
        if (!isFolder()) {
            titleText.prepend(QString("%1. ").arg(number()));
        }
        titleText = TextUtils::elidedText(titleText, _painter->font(), titleRect.size(), textoption);
        _painter->drawText(titleRect, titleText, textoption);

        //
        // Рисуем штамп
        //
        _painter->setOpacity(0.33);
        QFont stateFont = font;
        stateFont.setPointSize(stateFont.pointSize()*8);
        stateFont.setBold(true);
        stateFont.setCapitalization(QFont::AllUppercase);
        //
        // Нужно, чтобы состояние влезало в пределы карточки, если не влезает уменьшаем его шрифт
        // до тех пор, пока не будет найден подходящий размер, или пока он не будет равен двум
        // размерам базового шрифта
        //
        QFontMetricsF stateFontMetrics(stateFont);
        while (stateFont.pointSize() > font.pointSizeF()*1.2) {
            if (stateFontMetrics.width(m_stamp) < cardRect.width() - 7*2) {
                break;
            }
            stateFont.setPointSize(stateFont.pointSize() - 1);
            stateFontMetrics = QFontMetricsF(stateFont);
        }
        _painter->setFont(stateFont);
        const qreal stateHeight = stateFontMetrics.lineSpacing();
        QRectF stateRect(0, cardRect.height() - stateHeight - additionalColorsHeight, cardRect.width(), stateHeight);
        textoption.setAlignment(Qt::AlignCenter);
        _painter->drawText(stateRect, m_stamp, textoption);
        _painter->setOpacity(1.);

        //
        // Рисуем описание
        //
        textoption.setAlignment(Qt::AlignTop | Qt::AlignLeft);
        textoption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        font.setBold(false);
        _painter->setFont(font);
        const int spacing = titleRect.height() / 2;
        const QRectF descriptionRect(9, titleRect.bottom() + spacing, cardRect.size().width() - 18, cardRect.size().height() - titleRect.bottom() - spacing - 9);
        const QString descriptionText = TextUtils::elidedText(m_description, _painter->font(), descriptionRect.size(), textoption);
        _painter->drawText(descriptionRect, descriptionText, textoption);

        //
        // Рисуем рамку выделения
        //
        if (isSelected()) {
            _painter->setBrush(Qt::transparent);
            _painter->setPen(QPen(palette.highlight(), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
            _painter->drawRect(cardRect);
        }
    }

    _painter->restore();
}

void CardItem::takeFromBoard()
{
    setCursor(Qt::ClosedHandCursor);

    if (m_animation->state() == QAbstractAnimation::Running) {
        m_animation->stop();
    }
    m_animation.reset(new QParallelAnimationGroup);

    setZValue(10000);

    const int duration = 80;
    QPropertyAnimation* radiusAnimation = new QPropertyAnimation(m_shadowEffect.data(), "blurRadius");
    radiusAnimation->setDuration(duration);
    radiusAnimation->setStartValue(7);
    radiusAnimation->setEndValue(34);
    QPropertyAnimation* colorAnimation = new QPropertyAnimation(m_shadowEffect.data(), "color");
    colorAnimation->setDuration(duration);
    colorAnimation->setStartValue(QColor(63, 63, 63, 180));
    colorAnimation->setEndValue(QColor(23, 23, 23, 240));
    QPropertyAnimation* yOffsetAnimation = new QPropertyAnimation(m_shadowEffect.data(), "yOffset");
    yOffsetAnimation->setDuration(duration);
    yOffsetAnimation->setStartValue(1);
    yOffsetAnimation->setEndValue(6);
    QPropertyAnimation* scaleAnimation = new QPropertyAnimation(this, "scale");
    scaleAnimation->setDuration(duration);
    scaleAnimation->setStartValue(scale());
    scaleAnimation->setEndValue(scale() + 0.005);

    m_animation->addAnimation(radiusAnimation);
    m_animation->addAnimation(colorAnimation);
    m_animation->addAnimation(yOffsetAnimation);
    m_animation->addAnimation(scaleAnimation);
    m_animation->start();
}

void CardItem::putOnBoard()
{
    setCursor(Qt::OpenHandCursor);

    if (zValue() == 10000) {
        if (m_animation->state() == QAbstractAnimation::Running) {
            m_animation->stop();
        }

        qreal newZValue = 1;
        for (QGraphicsItem* item : collidingItems()) {
            if (item->zValue() >= newZValue) {
                newZValue = item->zValue() + 0.1;
            }
        }
        setZValue(newZValue);

        m_animation->setDirection(QAbstractAnimation::Backward);
        m_animation->start();
    }
}

void CardItem::mousePressEvent(QGraphicsSceneMouseEvent* _event)
{
    //
    // Если в режиме переноса между сценами, инициилизируем событие переноса
    //
    if (m_isInDragOutMode) {
        //
        // Создаём майм для переноса между сценами
        //
        QMimeData* mime = new QMimeData;

        //
        // Формируем массив данных из карточки
        //
        QByteArray mimeData;
        QDataStream mimeStream(&mimeData, QIODevice::WriteOnly);
        mimeStream << m_isFolder << m_title << m_description << m_stamp << m_colors;
        mime->setData(MimeType, mimeData);

        //
        // Начинаем событие переноса
        //
        QDrag* drag = new QDrag(_event->widget());
        drag->setMimeData(mime);
        drag->start();
    }
    //
    // В остальных случаях работаем стандартным образом
    //
    else {
        QGraphicsItem::mousePressEvent(_event);

        if (_event->button() == Qt::LeftButton) {
            takeFromBoard();
        }
    }
}

void CardItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event)
{
    QGraphicsItem::mouseReleaseEvent(_event);

    putOnBoard();
}

void CardItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event)
{
    QGraphicsItem::mouseDoubleClickEvent(_event);

    putOnBoard();
}

void CardItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event)
{
    QGraphicsItem::hoverLeaveEvent(_event);

    putOnBoard();
}
