#ifndef RESEARCHNAVIGATORPROXYSTYLE_H
#define RESEARCHNAVIGATORPROXYSTYLE_H

#include <QProxyStyle>


namespace UserInterface
{
	/**
	 * @brief Стиль отрисовки дерева навигатора
	 *
	 * Используется для изменения отображения индикатора вставки элемента после перетаскивания
	 */
	class ResearchNavigatorProxyStyle : public QProxyStyle
	{
	public:
		explicit ResearchNavigatorProxyStyle(QStyle* _style = 0);

        void drawPrimitive(PrimitiveElement _element, const QStyleOption* _option,
                           QPainter* _painter, const QWidget* _widget) const;
	};
}

#endif // RESEARCHNAVIGATORPROXYSTYLE_H
