#ifndef GOOGLECOLORSPANE_H
#define GOOGLECOLORSPANE_H

#include "ColorsPane.h"


/**
 * @brief Палитра цветов
 */
class GoogleColorsPane : public ColorsPane
{
	Q_OBJECT

public:
	explicit GoogleColorsPane(QWidget* _parent = 0);

	/**
	 * @brief Получить текущий цвет
	 */
	QColor currentColor() const;

    /**
     * @brief Содержит ли панель заданный цвет
     */
    bool contains(const QColor& _color) const;

public slots:
	/**
	 * @brief Установить текущий цвет
	 */
	void setCurrentColor(const QColor& _color);

protected:
	/**
	 * @brief Переопределяются, для реализации логики работы палитры
	 */
	/** @{ */
	void paintEvent(QPaintEvent* _event);
	void mouseMoveEvent(QMouseEvent* _event);
	void mousePressEvent(QMouseEvent* _event);
	/** @} */

private:
	/**
	 * @brief Инициилизировать цвета панели
	 */
	void initColors();

private:
	/**
	 * @brief Вспомогательный класс, для хранения связи цвета и его прямоугольника в палитре
	 */
	class ColorKeyInfo
	{
	public:
		QColor color;
		QRectF rect;

		ColorKeyInfo() {}
		ColorKeyInfo(const QColor& _color, const QRectF& _rect) : color(_color), rect(_rect) {}

		bool isValid() const {
			return color.isValid() && rect.isValid();
		}
	};

	/**
	 * @brief Все цвета палитры
	 */
	QList<ColorKeyInfo> m_colorInfos;

	/**
	 * @brief Выбранный цвет палитры
	 */
	ColorKeyInfo m_currentColorInfo;
};

#endif // GOOGLECOLORSPANE_H
