#ifndef SCROLLERHELPER
#define SCROLLERHELPER

#include <QScroller>
#include <QScrollerProperties>


/**
 * @brief Вспомогательный класс, для настройки скроллера
 */
class ScrollerHelper
{
public:
	/**
	 * @brief Добавить скроллер к заданному виджету
	 */
	static void addScroller(QWidget* _forWidget) {
		//
		// Добавляем скроллер
		//
		QScroller::grabGesture(_forWidget, QScroller::LeftMouseButtonGesture);
		QScroller* scroller = QScroller::scroller(_forWidget);

		//
		// Настраиваем параметры скроллера
		//
		QScrollerProperties properties = scroller->scrollerProperties();
		properties.setScrollMetric(QScrollerProperties::MousePressEventDelay, qreal(1.0));
		properties.setScrollMetric(QScrollerProperties::DragStartDistance, qreal(0.001));
		properties.setScrollMetric(QScrollerProperties::MinimumVelocity, qreal(20.0 / 1000));
		properties.setScrollMetric(QScrollerProperties::MaximumVelocity, qreal(350.0 / 1000));
		properties.setScrollMetric(QScrollerProperties::MaximumClickThroughVelocity, qreal(20.0 / 1000));
		properties.setScrollMetric(QScrollerProperties::AcceleratingFlickSpeedupFactor, qreal(2.0));
		QVariant overshootPolicy = QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
		properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, overshootPolicy);
		properties.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor, qreal(0.3));
		properties.setScrollMetric(QScrollerProperties::OvershootDragDistanceFactor, qreal(0.02));
		properties.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor, qreal(0.3));
		properties.setScrollMetric(QScrollerProperties::OvershootScrollTime, qreal(0.4));
		scroller->setScrollerProperties(properties);
	}
};

#endif // SCROLLERHELPER

