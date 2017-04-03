#include "qlightboxwidget.h"

#include <QAbstractScrollArea>
#include <QEvent>
#include <QEventLoop>
#include <QGraphicsOpacityEffect>
#include <QChildEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QParallelAnimationGroup>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QTimer>

namespace {
    /**
     * @brief Создать объект анимации появления/скрытия для заданного виджета
     */
    static QPropertyAnimation* createOpacityAnimation(QWidget* _forWidget) {
        QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(_forWidget);

        QPropertyAnimation* opacityAnimation = new QPropertyAnimation(opacityEffect, "opacity");
        opacityAnimation->setDuration(120);
        opacityAnimation->setEasingCurve(QEasingCurve::InCirc);
        opacityAnimation->setStartValue(0);
        opacityAnimation->setEndValue(1);

        opacityEffect->setOpacity(0);
        _forWidget->setGraphicsEffect(opacityEffect);

        return opacityAnimation;
    }
}


bool QLightBoxWidget::hasOpenedWidgets()
{
    return s_openedWidgetsCount > 0;
}

int QLightBoxWidget::s_openedWidgetsCount = 0;


QLightBoxWidget::QLightBoxWidget(QWidget* _parent, bool _folowToHeadWidget) :
    QWidget(_parent),
    m_isInUpdateSelf(false),
    m_animation(nullptr)
{
    //
    // Родительский виджет должен быть обязательно установлен
    //
    Q_ASSERT_X(_parent, "", Q_FUNC_INFO);

    //
    // Если необходимо, делаем родителем самый "старший" виджет
    //
    if (_folowToHeadWidget) {
        while (_parent->parentWidget() != 0) {
            _parent = _parent->parentWidget();
        }
        setParent(_parent);
    }

    //
    // Следим за событиями родительского виджета, чтобы
    // иметь возможность перерисовать его, когда изменяется размер и т.п.
    //
    _parent->installEventFilter(this);

    //
    // Скрываем виджет
    //
    ++s_openedWidgetsCount;
    setVisible(false);
}

QLightBoxWidget::~QLightBoxWidget()
{
    parent()->removeEventFilter(this);
}

void QLightBoxWidget::setVisible(bool _visible)
{
    if (_visible) {
        ++s_openedWidgetsCount;
        updateSelf();
        QWidget::setVisible(_visible);
    }

    animate(_visible);

    if (!_visible) {
        --s_openedWidgetsCount;
        QWidget::setVisible(_visible);
    }
}

bool QLightBoxWidget::eventFilter(QObject* _object, QEvent* _event)
{
    //
    // Виджету необходимо всегда быть последним ребёнком,
    // чтобы перекрывать остальные виджеты при отображении
    //
    if (_event->type() == QEvent::ChildAdded
        || _event->type() == QEvent::ChildRemoved) {
        QChildEvent* childEvent = dynamic_cast<QChildEvent*>(_event);
        if (childEvent->child() != this
            && isVisible()) {
            updateSelf();
        }
    }

    //
    // Если изменился размер родительского виджета, необходимо
    // перерисовать себя
    //
    else if (_event->type() == QEvent::Resize
             && isVisible()) {
        updateSelf();
    }

    return QWidget::eventFilter(_object, _event);
}

void QLightBoxWidget::paintEvent(QPaintEvent* _event)
{
    //
    // Рисуем фон
    //
    QPainter p;
    p.begin(this);
    // ... фото родительского виджета
    p.drawPixmap(0, 0, width(), height(), m_parentWidgetPixmap);
    // ... накладываем затемнённую область
    p.setBrush(QBrush(QColor(0, 0, 0, 200)));
    p.drawRect(0, 0, width(), height());
    p.end();

    //
    // Рисуем всё остальное
    //
    QWidget::paintEvent(_event);
}

void QLightBoxWidget::animate(bool _visible)
{
    const QAbstractAnimation::Direction direction = _visible ? QAbstractAnimation::Forward : QAbstractAnimation::Backward;
    if (m_animation != nullptr
        && (m_animation->state() == QAbstractAnimation::Running
            || m_animation->direction() == direction)) {
        return;
    }

    //
    // Приходится создавать несколько дополнительных анимаций помимо самого диалога
    // так же для всех viewport'ов областей прокрутки, т.к. к ним не применяется эффект прозрачности
    //
    m_animation = new QParallelAnimationGroup;
    m_animation->addAnimation(::createOpacityAnimation(this));
    foreach (QAbstractScrollArea* scrollArea, findChildren<QAbstractScrollArea*>()) {
        m_animation->addAnimation(::createOpacityAnimation(scrollArea->viewport()));
    }
    m_animation->setDirection(direction);
    m_animation->start();

    //
    // Ожидаем завершения анимации
    //
    QEventLoop animationEventLoop;
    connect(m_animation, SIGNAL(finished()), &animationEventLoop, SLOT(quit()));
    animationEventLoop.exec();

    //
    // Удаляем эффект анимации, т.к. иногда из-за него коряво отрисовываются некоторые виджеты
    //
    delete m_animation;
    m_animation = nullptr;
    setGraphicsEffect(nullptr);
    foreach (QAbstractScrollArea* scrollArea, findChildren<QAbstractScrollArea*>()) {
        scrollArea->setGraphicsEffect(nullptr);
    }
}

void QLightBoxWidget::updateSelf()
{
    if (!m_isInUpdateSelf) {
        m_isInUpdateSelf = true;

        {
            //
            // Переустановим родителя
            //
            raise();

            //
            // Обновляем отображение
            //
            resize(parentWidget()->size());
            m_parentWidgetPixmap = grabParentWidgetPixmap();
            update();
        }

        m_isInUpdateSelf = false;
    }
}

QPixmap QLightBoxWidget::grabParentWidgetPixmap() const
{
    QPixmap parentWidgetPixmap;
    if (isVisible()
        && parentWidget()->isVisible()) {
        parentWidget()->render(&parentWidgetPixmap);
    }
    return parentWidgetPixmap;
}
