#include "ScenarioNavigatorItemDelegate.h"

#include <BusinessLayer/ScenarioDocument/ScenarioModel.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <3rd_party/Helpers/ImageHelper.h>

#include <QPainter>

namespace {
    static int s_iconSize = 20;
    static int s_topMargin = 8;
    static int s_bottomMargin = 8;
    static int s_itemsSpacing = 6;
}

using UserInterface::ScenarioNavigatorItemDelegate;


ScenarioNavigatorItemDelegate::ScenarioNavigatorItemDelegate(QObject* _parent) :
    QStyledItemDelegate(_parent),
    m_showSceneNumber(false),
    m_showSceneTitle(false),
    m_showSceneDescription(true),
    m_sceneDescriptionIsSceneText(true),
    m_sceneDescriptionHeight(1)
{
}

void ScenarioNavigatorItemDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
    //
    // Получим настройки стиля
    //
    QStyleOptionViewItem opt = _option;
    initStyleOption(&opt, _index);

    //
    // Рисуем ручками
    //
    _painter->save();
    _painter->setRenderHint(QPainter::Antialiasing, true);

    //
    // Определим кисти и шрифты
    //
    QBrush backgroundBrush = opt.palette.background();
    QBrush textBrush = opt.palette.text();
    QFont headerFont = opt.font;
    headerFont.setBold(m_showSceneDescription ? true : false);
    QFont textFont = opt.font;
    textFont.setBold(false);
    //
    // ... для выделенных элементов
    //
    if(opt.state.testFlag(QStyle::State_Selected))
    {
        backgroundBrush = opt.palette.highlight();
        textBrush = opt.palette.highlightedText();
    }
    //
    // ... для остальных
    //
    else
    {
        //
        // Реализация альтернативных цветов в представлении
        //
        if(opt.features.testFlag(QStyleOptionViewItem::Alternate))
        {
            backgroundBrush = opt.palette.alternateBase();
            textBrush = opt.palette.windowText();
        }
        else
        {
            backgroundBrush = opt.palette.base();
            textBrush = opt.palette.windowText();
        }
    }

    //
    // Рисуем
    //
    const int TREE_INDICATOR_WIDTH = 20;
    const int COLOR_RECT_WIDTH = 12;
    const int RIGHT_MARGIN = 12;
    const int TEXT_LINE_HEIGHT = _painter->fontMetrics().height();

    //
    // ... фон
    //
    _painter->fillRect(opt.rect, backgroundBrush);

    //
    // ... разделитель
    //
    QPoint borderLeft = opt.rect.bottomLeft();
    borderLeft.setX(0);
    _painter->setPen(QPen(opt.palette.dark(), 1));
    _painter->drawLine(borderLeft, opt.rect.bottomRight());

    //
    // Меняем координаты, чтобы рисовать было удобнее
    //
    _painter->translate(opt.rect.topLeft());

    //
    // ... иконка
    //
    const QRect iconRect(0, s_topMargin, s_iconSize, s_iconSize);
    QPixmap icon = _index.data(Qt::DecorationRole).value<QPixmap>();
    QIcon iconColorized(icon);
    QColor iconColor = textBrush.color();
    // ... если есть заметка, рисуем красноватым цветом
    if (_index.data(BusinessLogic::ScenarioModel::HasNoteIndex).toBool()) {
        iconColor = QColor("#ec3838");
    }
    ImageHelper::setIconColor(iconColorized, iconRect.size(), iconColor);
    icon = iconColorized.pixmap(iconRect.size());
    _painter->drawPixmap(iconRect, icon);

    //
    // ... цвета сцены
    //
    const QString colorsNames = _index.data(BusinessLogic::ScenarioModel::ColorIndex).toString();
    QStringList colorsNamesList = colorsNames.split(";", QString::SkipEmptyParts);
    int colorsCount = colorsNamesList.size();
    int colorRectX = TREE_INDICATOR_WIDTH + opt.rect.width() - COLOR_RECT_WIDTH - s_itemsSpacing - RIGHT_MARGIN;
    if (colorsCount > 0) {
        //
        // Если цвет один, то просто рисуем его
        //
        if (colorsCount == 1) {
            const QColor color(colorsNamesList.first());
            const QRectF colorRect(colorRectX, 0, COLOR_RECT_WIDTH, opt.rect.height());
            _painter->fillRect(colorRect, color);
        }
        //
        // Если цветов много
        //
        else {
            //
            // ... первый цвет рисуем на всю вышину в первой колонке
            //
            {
                colorRectX -= COLOR_RECT_WIDTH;
                const QColor color(colorsNamesList.takeFirst());
                const QRectF colorRect(colorRectX, 0, COLOR_RECT_WIDTH, opt.rect.height());
                _painter->fillRect(colorRect, color);
            }
            //
            // ... остальные цвета рисуем во второй колонке цветов
            //
            colorRectX += COLOR_RECT_WIDTH;
            colorsCount = colorsNamesList.size();
            for (int colorIndex = 0; colorIndex < colorsCount; ++colorIndex) {
                const QString colorName = colorsNamesList.takeFirst();
                const QColor color(colorName);
                const QRectF colorRect(
                            colorRectX,
                            opt.rect.height() / qreal(colorsCount) * colorIndex,
                            COLOR_RECT_WIDTH,
                            opt.rect.height() / qreal(colorsCount)
                            );
                _painter->fillRect(colorRect, color);
            }
            //
            // ... смещаем позицию назад, для корректной отрисовки остального контента
            //
            colorRectX -= COLOR_RECT_WIDTH;
        }
    }

    //
    // ... текстовая часть
    //
    _painter->setPen(textBrush.color());

    //
    // ... длительность
    //
    _painter->setFont(textFont);
    const int duration = _index.data(BusinessLogic::ScenarioModel::DurationIndex).toInt();
    const QString chronometry =
            BusinessLogic::ChronometerFacade::chronometryUsed()
            ? "(" + BusinessLogic::ChronometerFacade::secondsToTime(duration)+ ") "
            : "";
    const int chronometryRectWidth = _painter->fontMetrics().width(chronometry);
    const QRect chronometryRect(
        colorRectX - chronometryRectWidth - s_itemsSpacing,
        s_topMargin,
        chronometryRectWidth,
        s_iconSize
        );
    _painter->drawText(chronometryRect, Qt::AlignLeft | Qt::AlignVCenter, chronometry);

    //
    // ... заголовок
    //
    _painter->setFont(headerFont);
    const QRect headerRect(
        iconRect.right() + s_itemsSpacing,
        s_topMargin,
        chronometryRect.left() - iconRect.right() - s_itemsSpacing*2,
        s_iconSize
        );
    QString header = _index.data(Qt::DisplayRole).toString().toUpper();
    if (m_showSceneTitle) {
        //
        // Если нужно выводим название сцены вместо заголовка
        //
        const QString title = _index.data(BusinessLogic::ScenarioModel::TitleIndex).toString().toUpper();
        if (!title.isEmpty()) {
            header = title;
        }
    }
    if (m_showSceneNumber) {
        //
        // Если нужно добавляем номер сцены
        //
        QVariant sceneNumber = _index.data(BusinessLogic::ScenarioModel::SceneNumberIndex);
        if (!sceneNumber.isNull()) {
            header = sceneNumber.toString() + ". " + header;
        }
    }
    header = _painter->fontMetrics().elidedText(header, Qt::ElideRight, headerRect.width());
    _painter->drawText(headerRect, Qt::AlignLeft | Qt::AlignVCenter, header);

    //
    // ... описание
    //
    if (m_showSceneDescription) {
        _painter->setFont(textFont);
        const QRect descriptionRect(
            headerRect.left(),
            headerRect.bottom() + s_itemsSpacing,
            chronometryRect.right() - headerRect.left(),
            TEXT_LINE_HEIGHT * m_sceneDescriptionHeight
            );
        const QString descriptionText =
                m_sceneDescriptionIsSceneText
                ? _index.data(BusinessLogic::ScenarioModel::SceneTextIndex).toString()
                : _index.data(BusinessLogic::ScenarioModel::DescriptionIndex).toString();
        _painter->drawText(descriptionRect, Qt::TextWordWrap, descriptionText);
    }

    _painter->restore();
}

QSize ScenarioNavigatorItemDelegate::sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
    Q_UNUSED(_option);
    Q_UNUSED(_index);

    //
    // Размер составляется из лейблов
    // строка на заголовок (ICON_SIZE) и m_sceneDescriptionHeight строк на описание
    // + отступы TOP_MARGIN сверху + BOTTOM_MARGIN снизу + ITEMS_SPACING между текстом
    //
    int lines = 0;
    int additionalHeight = s_topMargin + s_iconSize + s_bottomMargin;
    if (m_showSceneDescription) {
        lines += m_sceneDescriptionHeight;
        additionalHeight += s_itemsSpacing;
    }
    const int height = _option.fontMetrics.height() * lines + additionalHeight;
    const int width = 50;
    return QSize(width, height);
}

void ScenarioNavigatorItemDelegate::setShowSceneNumber(bool _show)
{
    if (m_showSceneNumber != _show) {
        m_showSceneNumber = _show;
    }
}

void ScenarioNavigatorItemDelegate::setShowSceneTitle(bool _show)
{
    if (m_showSceneTitle != _show) {
        m_showSceneTitle = _show;
    }
}

void ScenarioNavigatorItemDelegate::setShowSceneDescription(bool _show)
{
    if (m_showSceneDescription != _show) {
        m_showSceneDescription = _show;
    }
}

void ScenarioNavigatorItemDelegate::setSceneDescriptionIsSceneText(bool _isSceneText)
{
    if (m_sceneDescriptionIsSceneText != _isSceneText) {
        m_sceneDescriptionIsSceneText = _isSceneText;
    }
}

void ScenarioNavigatorItemDelegate::setSceneDescriptionHeight(int _height)
{
    if (m_sceneDescriptionHeight != _height) {
        m_sceneDescriptionHeight = _height;
    }
}
