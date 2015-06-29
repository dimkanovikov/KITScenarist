#ifndef WORDHIGHLIGHTCOLORSPANE_H
#define WORDHIGHLIGHTCOLORSPANE_H


#include "ColorsPane.h"


/**
 * @brief Палитра цветов
 */
class WordHighlightColorsPane : public ColorsPane
{
	Q_OBJECT

public:
	explicit WordHighlightColorsPane(QWidget* _parent = 0);

	/**
	 * @brief Получить текущий цвет
	 */
	QColor currentColor() const;

public slots:
	/**
	 * @brief Установить текущий цвет
	 */
	void setCurrentColor(const QColor& _color);

signals:
	/**
	* @brief Выбран цвет
	*/
	void selected(const QColor& _color);

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

#endif // WORDHIGHLIGHTCOLORSPANE_H
