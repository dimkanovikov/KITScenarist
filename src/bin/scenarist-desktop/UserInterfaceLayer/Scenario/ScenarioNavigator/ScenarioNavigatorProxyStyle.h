#ifndef SCENARIONAVIGATORPROXYSTYLE_H
#define SCENARIONAVIGATORPROXYSTYLE_H

#include <QProxyStyle>


namespace UserInterface
{
	/**
	 * @brief Стиль отрисовки дерева навигатора
	 *
	 * Используется для изменения отображения индикатора вставки элемента после перетаскивания
	 */
	class ScenarioNavigatorProxyStyle : public QProxyStyle
	{
	public:
		explicit ScenarioNavigatorProxyStyle(QStyle* _style = 0);

		void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
						   QPainter *painter, const QWidget *widget) const;
	};
}

#endif // SCENARIONAVIGATORPROXYSTYLE_H
