#ifndef QLIGHTBOXWIDGET_H
#define QLIGHTBOXWIDGET_H

#include <QWidget>

class QParallelAnimationGroup;


/**
 * @brief Класс перекрытия
 */
class QLightBoxWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Открыт ли хотя бы один диалог в данный момент
     */
    static bool hasOpenedWidgets();

public:
    explicit QLightBoxWidget(QWidget* _parent, bool _folowToHeadWidget = false);
    ~QLightBoxWidget();

    /**
     * @brief Переопределяется для того, чтобы перед отображением настроить внешний вид
     */
    void setVisible(bool _visible);

protected:
    /**
     * @brief Переопределяется для отслеживания собитий родительского виджета
     */
    bool eventFilter(QObject* _object, QEvent* _event);

    /**
     * @brief Переопределяется для того, чтобы эмитировать эффект перекрытия
     */
    void paintEvent(QPaintEvent* _event);

private:
    /**
     * @brief Анимировать отображение/сокрытие диалога
     */
    void animate(bool _visible);

    /**
     * @brief Обновить размер и картинку фона
     */
    void updateSelf();

    /**
     * @brief Разрешает конфликт рекурсивного обновления
     */
    bool m_isInUpdateSelf;

    /**
     * @brief Обновить картинку фона
     */
    QPixmap grabParentWidgetPixmap() const;

    /**
     * @brief Картинка фона
     */
    QPixmap m_parentWidgetPixmap;

    /**
     * @brief Анимация отображаения/скрытия
     */
    QParallelAnimationGroup* m_animation;

    /**
     * @brief Количество открытых диалогов
     */
    static int s_openedWidgetsCount;
};

#endif // QLIGHTBOXWIDGET_H
