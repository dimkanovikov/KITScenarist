#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QtWidgets//QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QIcon>
#include <QAction>

class QTimeLine;


class SideTabBar : public QWidget
{
    Q_OBJECT

public:
    SideTabBar(QWidget *parent = 0);

    /**
     * @brief Добавить вкладку
     */
    /** @{ */
    void addTab(QAction* action);
    QAction* addTab(const QString& text, const QIcon& icon = QIcon());
    /** @} */

    /**
     * @brief Получить вкладку по заданному индексу
     */
    QAction* tab(int _index) const;

    /**
     * @brief Установить текущую вкладку
     */
    void setCurrentTab(int _index);

    /**
     * @brief Получить индекс текущей вкладки
     */
    int currentTab() const;

    /**
     * @brief Получить индекс предыдущей активной вкладки
     */
    int prevCurrentTab() const;

    /**
     * @brief Добавить индикатор вниз панели
     * @note Вызов с пустыми параметрами удаляет индикатор
     */
    void addIndicator(const QIcon& _icon);

    /**
     * @brief Установить заголовок сообщения индикатора
     */
    void setIndicatorTitle(const QString& _title);

    /**
     * @brief Установить текст сообщения индикатора
     */
    void setIndicatorText(const QString& _text);

    /**
     * @brief Установить текст подвала в сообщении индикатора
     */
    void setIndicatorFooterText(const QString& _text);

    /**
     * @brief Установить иконку действия индикатора
     */
    void setIndicatorActionIcon(const QIcon& _icon);

    /**
     * @brief Убрать индикатор из панели
     */
    void removeIndicator();

    /**
     * @brief Установить компактный режим (по умолчанию установлен расширенный режим)
     */
    void setCompactMode(bool _compact);

signals:
    /**
     * @brief Изменилась текущая вкладка
     */
    void currentChanged(int _index);

    /**
     * @brief Нажата кнопка действия индикатора
     */
    void indicatorActionClicked();

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* _event);
    void mouseReleaseEvent(QMouseEvent* _event);
    QSize minimumSizeHint() const;

private:
    /**
     * @brief Определить вкладку в заданной координате
     */
    QAction* tabAt(const QPoint& _pos);

    /**
     * @brief Установить заданную часть текста индикатора
     */
    void setIndicatorText(const char* _key, const QString& _text);

private:
    /**
     * @brief Список вкладок
     */
    QList<QAction*> m_tabs;

    /**
     * @brief Вкладка на которой был выполнен клик мышкой
     * @note Используется для внутренних нужд
     */
    QAction* m_pressedTab;

    /**
     * @brief Текущая активная вкладка
     */
    QAction* m_checkedTab;

    /**
     * @brief Флаг компактного режима
     */
    bool m_compactMode;

    /**
     * @brief Индекс текущей вкладки
     */
    int m_currentIndex;

    /**
     * @brief Индекс предыдущей активной вкладки
     */
    int m_prevCurrentIndex;

    /**
     * @brief Индикатор внизу панели
     */
    QAction* m_indicator = nullptr;

    /**
     * @brief Таймлайн для реализации анимации индикации смены индикатора
     */
    QTimeLine* m_timeline = nullptr;

    /**
     * @brief Количество анимаций иконки (одна анимация - это исчезание или появление)
     */
    int m_timelineLoops = 1;

    /**
     * @brief Новая иконка индикатора
     */
    QIcon m_newIndicatorIcon;
};

#endif // SIDEBAR_H
