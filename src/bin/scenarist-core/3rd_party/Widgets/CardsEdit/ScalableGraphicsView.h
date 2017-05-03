#ifndef SCALABLEGRAPHICSVIEW_H
#define SCALABLEGRAPHICSVIEW_H

#include <QGraphicsView>

class QGestureEvent;


/**
 * @brief Область отрисовки графических элементов с масштабированием и улучшенной прокруткой
 */
class ScalableGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ScalableGraphicsView(QWidget *parent = 0);

    /**
     * @brief Методы масштабирования
     */
    /** @{ */
    void zoomIn();
    void zoomOut();
    /** @} */

signals:
    /**
     * @brief Изменился масштаб
     */
    void scaleChanged();

    /**
     * @brief Нажата кнопка Delete или Backspace
     */
    void deletePressed();

protected:
    /**
     * @brief Переопределяем для обработки жестов
     */
    bool event(QEvent* _event);

    /**
     * @brief Обрабатываем жест увеличения масштаба
     */
    void gestureEvent(QGestureEvent* _event);

    /**
     * @brief Обрабатываем изменение масштаба при помощи ролика
     */
    void wheelEvent(QWheelEvent* _event);

    /**
     * @brief Переопределяем для обработки горячих клавиш изменения масштаба и скроллинга
     */
    /** @{ */
    void keyPressEvent(QKeyEvent* _event);
    void keyReleaseEvent(QKeyEvent* _event);
    /** @} */

    /**
     * @brief Реализация перетаскивания
     */
    /** @{ */
    void mousePressEvent(QMouseEvent* _event);
    void mouseMoveEvent(QMouseEvent* _event);
    void mouseReleaseEvent(QMouseEvent* _event);
    /** @} */

private:
    /**
     * @brief Масштабировать представление на заданный коэффициент
     */
    void scaleView(qreal _factor);

private:
    /**
     * @brief Можно ли изменять содержимое
     */
    bool m_isReadOnly = false;

    /**
     * @brief Последняя позиция мыши в момент скроллинга полотна
     */
    QPoint m_scrollingLastPos;

    /**
     * @brief Происходит ли в данный момент скроллинг с зажатым пробелом
     */
    bool m_inScrolling = false;

    /**
     * @brief Инерционный тормоз масштабирования при помощи жестов
     */
    int m_gestureZoomInertionBreak = 0;
};

#endif // SCALABLEGRAPHICSVIEW_H
