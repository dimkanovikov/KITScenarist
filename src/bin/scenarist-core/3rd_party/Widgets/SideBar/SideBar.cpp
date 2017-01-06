#include "SideBar.h"

#include <3rd_party/Helpers/ImageHelper.h>

#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/WAF/Animation/Animation.h>

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QTimeLine>
#include <QWidgetAction>

namespace {
    /**
     * @brief Ширина панели
     */
    const int SIDEBAR_WIDTH = 90;
    const int SIDEBAR_WIDTH_COMPACT = 50;
    static int sidebarWidth(bool _compact) {
        return _compact ? SIDEBAR_WIDTH_COMPACT : SIDEBAR_WIDTH;
    }

    /**
     * @brief Высота вкладки
     */
    const int TAB_HEIGHT = 70;
    const int TAB_HEIGHT_COMPACT = 40;
    static int tabHeight(bool _compact) {
        return _compact ? TAB_HEIGHT_COMPACT : TAB_HEIGHT;
    }

    /**
     * @brief Размер иконки вкладки
     */
    const QSize TAB_ICON_SIZE(48, 48);
    const QSize TAB_ICON_SIZE_COMPACT(36, 36);
    static QSize tabIconSize(bool _compact) {
        return _compact ? TAB_ICON_SIZE_COMPACT : TAB_ICON_SIZE;
    }

    /**
     * @brief Высота индикатора
     */
    const int INDICATOR_HEIGHT = 30;

    /**
     * @brief Размер иконки индикатора
     */
    const QSize INDICATOR_ICON_SIZE(16, 16);

    /**
     * @brief Цвет текущей вкладки
     */
    const QColor CURRENT_TAB_BG_COLOR(38, 40, 42);

    /**
     * @brief Покрасить иконку в цвет
     */
    static QPixmap colorizeIcon(const QPixmap& _icon, const QColor& _color) {
        QPixmap baseIconPixmap = _icon;
        QPixmap newIconPixmap = baseIconPixmap;

        QPainter painter(&newIconPixmap);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(newIconPixmap.rect(), _color);
        painter.end();

        return newIconPixmap;
    }

    /**
     * @brief Сделать иконку "неактивной"
     */
    static QPixmap makeIconInactive(const QPixmap& _icon) {
        return colorizeIcon(_icon, QApplication::palette().color(QPalette::Inactive, QPalette::Text));
    }

    /**
     * @brief Сделать иконку "выключенной"
     */
    static QPixmap makeIconDisabled(const QPixmap& _icon) {
        return colorizeIcon(_icon, QApplication::palette().color(QPalette::Disabled, QPalette::Text));
    }

    /**
     * @brief Ключи доступа к тексту индикатора
     */
    /** @{ */
    const char* INDICATOR_TITLE_KEY = "title";
    const char* INDICATOR_TEXT_KEY = "text";
    const char* INDICATOR_FOOTER_KEY = "footer";
    const char* INDICATOR_ACTION_ICON_KEY = "action_icon";
    /** @} */
}


SideTabBar::SideTabBar(QWidget *parent) :
    QWidget(parent),
    m_pressedTab(0),
    m_checkedTab(0),
    m_compactMode(false),
    m_currentIndex(0),
    m_prevCurrentIndex(0),
    m_indicator(new QAction(this)),
    m_timeline(new QTimeLine(300, this))
{
    setFixedWidth(::sidebarWidth(m_compactMode));

    m_timeline->setFrameRange(100, 0);
    connect(m_timeline, &QTimeLine::frameChanged, [=] { update(); });
    connect(m_timeline, &QTimeLine::finished, [=] {
        m_timeline->setDirection(m_timeline->direction() == QTimeLine::Forward ? QTimeLine::Backward : QTimeLine::Forward);

        //
        // Если нужно, меняем иконку
        //
        m_indicator->setIcon(m_newIndicatorIcon);
        m_indicator->setVisible(!m_newIndicatorIcon.isNull());

        //
        // Если необходимо, запустить анимацию ещё раз
        //
        if (m_timelineLoops > 0) {
            --m_timelineLoops;
            m_timeline->start();
        }
    });

    //
    // По умолчанию индикатор скрыт
    //
    removeIndicator();
}

void SideTabBar::addTab(QAction *action)
{
    action->setCheckable(true);
    if (m_tabs.isEmpty()) {
        action->setChecked(true);
        m_checkedTab = action;
    }
    m_tabs.push_back(action);
    update();
    return;
}

QAction *SideTabBar::addTab(const QString &text, const QIcon &icon)
{
    QAction *action = new QAction(icon, text, this);
    addTab(action);
    return action;
}

QAction* SideTabBar::tab(int _index) const
{
    QAction* result = 0;
    if (_index < m_tabs.size()) {
        result = m_tabs.at(_index);
    }
    return result;
}

void SideTabBar::setCurrentTab(int _index)
{
    //
    // Если индекс в допустимом пределе и выделена не текущая вкладка
    //
    if (_index < m_tabs.size()
        && m_checkedTab != m_tabs.at(_index)) {
        //
        // Переключаемся только на видимую вкладку
        //
        if (m_tabs.at(_index)->isVisible()) {
            //
            // Запоминаем предыдущую активную вкладку
            //
            m_prevCurrentIndex = m_tabs.indexOf(m_checkedTab);

            //
            // Переключаемся на новую активную вкладку
            //
            m_checkedTab->setChecked(false);
            m_checkedTab = m_tabs.at(_index);
            m_checkedTab->setChecked(true);
            update();

            //
            // Уведомляем об изменении активной вкладки
            //
            m_currentIndex = _index;
            emit currentChanged(m_currentIndex);
        }
        //
        // Если вкладка невидима, переключаемся на следующую за ней
        //
        else {
            setCurrentTab(_index + 1);
        }
    }
}

int SideTabBar::currentTab() const
{
    return m_currentIndex;
}

int SideTabBar::prevCurrentTab() const
{
    return m_prevCurrentIndex;
}

void SideTabBar::addIndicator(const QIcon& _icon)
{
    if (m_indicator->icon().pixmap(::INDICATOR_ICON_SIZE).toImage()
        == _icon.pixmap(::INDICATOR_ICON_SIZE).toImage()) {
        return;
    }

    if (m_timeline->state() == QTimeLine::Running) {
        m_timeline->stop();
    }

    m_timeline->setDirection(QTimeLine::Forward);
    m_newIndicatorIcon = _icon;
    m_timelineLoops = 5;

    m_timeline->start();
}

void SideTabBar::setIndicatorTitle(const QString& _title)
{
    setIndicatorText(::INDICATOR_TITLE_KEY, _title);
}

void SideTabBar::setIndicatorText(const QString& _text)
{
    setIndicatorText(::INDICATOR_TEXT_KEY, _text);
}

void SideTabBar::setIndicatorFooterText(const QString& _text)
{
    setIndicatorText(::INDICATOR_FOOTER_KEY, _text);
}

void SideTabBar::setIndicatorActionIcon(const QIcon& _icon)
{
    m_indicator->setProperty(::INDICATOR_ACTION_ICON_KEY, _icon);
}

void SideTabBar::makeIndicatorWave(const QColor& _waveColor)
{
    //
    // Если нужно запустить волну другого цвета
    //
    if (m_waveColor != _waveColor) {
        m_waveColor = _waveColor;
        if (m_waveColor.isValid()) {
            const QPoint waveStartPosition(::sidebarWidth(m_compactMode)/2, height() - ::INDICATOR_HEIGHT/2);
            WAF::Animation::circleFillIn(parentWidget(), mapToParent(waveStartPosition), m_waveColor);
        }
    }
}

void SideTabBar::removeIndicator()
{
    addIndicator(QIcon());
}

void SideTabBar::setCompactMode(bool _compact)
{
    if (m_compactMode != _compact) {
        m_compactMode = _compact;

        setFixedWidth(::sidebarWidth(m_compactMode));

        repaint();
    }
}

void SideTabBar::paintEvent(QPaintEvent *event)
{
    QPainter p(this);


    //
    // Фон
    //
    p.fillRect(event->rect(), palette().button());
    //
    // Границы сверху и справа
    //
    p.setPen(palette().dark().color());
    p.drawLine(event->rect().topLeft(), event->rect().topRight());
    p.drawLine(event->rect().topLeft(), event->rect().bottomLeft());
    p.drawLine(event->rect().topRight(), event->rect().bottomRight());
    p.drawLine(event->rect().bottomLeft(), event->rect().bottomRight());


    //
    // Рисуем вкладки
    //
    const int tabHeight = ::tabHeight(m_compactMode);
    const int iconRectHeight = tabHeight - (m_compactMode ? 0 : 16);
    int tabY = 1;
    foreach (QAction* tab, m_tabs)
    {
        //
        // Рисуем только видимые вкладки
        //
        if (tab->isVisible()) {
            const QRect tabRect(1, tabY, event->rect().width() - 2, tabHeight);

            //
            // Текущая вкладка
            //
            if (tab->isChecked()) {
                p.fillRect(tabRect, palette().window());
            }

            //
            // Настроим иконку вкладки
            //
            const QRect tabIconRect(0, tabY, event->rect().width(), iconRectHeight);
            QPixmap tabImage = tab->icon().pixmap(::tabIconSize(m_compactMode));
            //
            // ... если действие недоступно
            //
            if (!tab->isEnabled()) {
                tabImage = ::makeIconDisabled(tabImage);
            }
            //
            // ... если действие неактивно
            //
            else if (!tab->isChecked()) {
                tabImage = ::makeIconInactive(tabImage);
            }

            //
            // Рисуем иконку
            //
            QIcon tabIcon(tabImage);
            tabIcon.paint(&p, tabIconRect);
            p.setOpacity(1);

            //
            // Рисуем текст
            //
            if (m_compactMode == false) {
                p.setPen(QApplication::palette().color(QPalette::Inactive, QPalette::Text)); // неактивный текст
                if (tab->isChecked())
                    p.setPen(QApplication::palette().color(QPalette::Active, QPalette::Text)); // активный текст
                if (!tab->isEnabled())
                    p.setPen(QApplication::palette().color(QPalette::Disabled, QPalette::Text)); // недоступный текст
                QRect tabTextRect(0, tabY + tabRect.height() - 23, event->rect().width(), 18);
                p.drawText(tabTextRect, Qt::AlignCenter, tab->text());
            }

            tabY += tabHeight;
        }
    }

    //
    // Рисуем индикатор
    //
    if (m_indicator->isVisible()) {
        const QRect indicatorRect(0, height() - INDICATOR_HEIGHT, ::sidebarWidth(m_compactMode), INDICATOR_HEIGHT);

        //
        // Граница сверху
        //
        p.setPen(palette().dark().color());
        p.drawLine(indicatorRect.topLeft(), indicatorRect.topRight());

        //
        // Иконка индикатора
        //
        p.setOpacity(m_timeline->currentFrame() / 100.);
        m_indicator->icon().paint(&p, indicatorRect);
    }
}
#include <QTimer>
void SideTabBar::mousePressEvent(QMouseEvent* _event)
{
    //
    // Нажата кнопка?
    //
    m_pressedTab = tabAt(_event->pos());
    if (m_pressedTab != 0 && m_pressedTab != m_checkedTab) {
        //
        // Обновим внешний вид
        //
        update();
    }
    //
    // Нажат индикатор?
    //
    else if (m_indicator->isVisible()
             && _event->pos().y() > (height() - INDICATOR_HEIGHT)) {
        //
        // Сформируем виджет для отображения
        //
        const QString titleText = QString("<b>%1</b>").arg(m_indicator->property(::INDICATOR_TITLE_KEY).toString());
        const QString bodyText = m_indicator->property(::INDICATOR_TEXT_KEY).toString();
        const QString footerText =
            m_indicator->property(::INDICATOR_FOOTER_KEY).toString().isEmpty()
            ? QString::null
            : QString("<p style='font-size:small;font-weight:bold;'><br/>%1</p>")
                     .arg(m_indicator->property(::INDICATOR_FOOTER_KEY).toString());

        QMenu menu(this);
        QFrame* menuFrame = new QFrame;
        QGridLayout* layout = new QGridLayout(menuFrame);
        layout->addWidget(new QLabel(titleText, menuFrame), 0, 0);
        QIcon icon = m_indicator->property(::INDICATOR_ACTION_ICON_KEY).value<QIcon>();
        if (!icon.isNull()) {
            FlatButton* button = new FlatButton(menuFrame);
            button->setIcons(icon);
            button->setAutoRaise(true);
            connect(button, &FlatButton::clicked, &menu, &QMenu::close);
            connect(button, &FlatButton::clicked, button, &FlatButton::hide);
            connect(button, &FlatButton::clicked, this, &SideTabBar::indicatorActionClicked);
            layout->addWidget(button, 0, 1);
        }
        if (!bodyText.isEmpty()) {
            layout->addWidget(new QLabel(bodyText, menuFrame), 1, 0, 1, -1);
            if (!footerText.isEmpty()) {
                layout->addWidget(new QLabel(footerText, menuFrame), 2, 0, 1, -1);
            }
        }
        menuFrame->setFixedSize(menuFrame->sizeHint());

        //
        // Покажем информацию
        //
        QWidgetAction menuText(&menu);
        menuText.setDefaultWidget(menuFrame);
        menu.addAction(&menuText);
        menu.exec(mapToGlobal(QPoint(::sidebarWidth(m_compactMode), height() - menu.sizeHint().height())));

    }
}

void SideTabBar::mouseReleaseEvent(QMouseEvent* _event)
{
    QAction* pressedTab = tabAt(_event->pos());

    //
    // Если мышка была отпущена на другой вкладке, не на той на которой нажата - игнорируем событие
    //
    if (m_pressedTab != pressedTab
        || pressedTab == 0
        || !pressedTab->isEnabled()) {
        m_pressedTab = 0;
        return;
    }

    //
    // Активируем выбранную вкладку
    //
    setCurrentTab(m_tabs.indexOf(pressedTab));
    m_pressedTab = 0;
}

QSize SideTabBar::minimumSizeHint() const
{
    const int width = ::sidebarWidth(m_compactMode);
    const int height = (m_tabs.size() * ::tabHeight(m_compactMode)) + (m_indicator->isVisible() ? INDICATOR_HEIGHT : 0);
    return QSize(width, height);
}

QAction* SideTabBar::tabAt(const QPoint& _pos)
{
    QAction* tabAtPos = 0;
    int tabY = 0;
    foreach(QAction* tab, m_tabs)
    {
        if (tab->isVisible()) {
            QRect tabRect(0, tabY, rect().width(), ::tabHeight(m_compactMode));
            if (tabRect.contains(_pos)) {
                tabAtPos = tab;
                break;
            }
            tabY += tabRect.height();
        }
    }
    return tabAtPos;
}

void SideTabBar::setIndicatorText(const char* _key, const QString& _text)
{
    m_indicator->setProperty(_key, _text);
    update();
}
