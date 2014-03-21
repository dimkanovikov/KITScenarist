#ifndef ZOOMABLETEXTEDIT_H
#define ZOOMABLETEXTEDIT_H

#include <QTextEdit>


/**
 * @brief Класс текстового редактора с возможностью масштабирования
 */
class ZoomableTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	explicit ZoomableTextEdit(QWidget* _parent = 0);

	/**
	 * @brief Установить коэффициент масштабирования
	 */
	void setZoomRange(int _zoomRange);

signals:
	/**
	 * @brief Изменился коэффициент масштабирования
	 */
	void zoomRangeChanged(int);

protected:
	/**
	 * @brief Возобновить масштабирование
	 */
	void resetZoom();

	/**
	 * @brief Переопределяется для реализации увеличения/уменьшения текста
	 */
	void wheelEvent(QWheelEvent* _event);

private:
	/**
	 * @brief Текущий коэффициент масштабирования
	 */
	int m_zoomRange;
};

#endif // ZOOMABLETEXTEDIT_H
