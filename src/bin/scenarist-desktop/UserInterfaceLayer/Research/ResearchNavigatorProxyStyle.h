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

		void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
						   QPainter *painter, const QWidget *widget) const;
	};
}

#endif // RESEARCHNAVIGATORPROXYSTYLE_H
