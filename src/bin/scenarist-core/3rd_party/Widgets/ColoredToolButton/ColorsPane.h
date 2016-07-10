#ifndef COLORSPANE_H
#define COLORSPANE_H

#include <QWidget>


/**
 * @brief Интерфейс палитры цветов
 */
class ColorsPane : public QWidget
{
	Q_OBJECT

public:
	explicit ColorsPane(QWidget* _parent = 0) :
		QWidget(_parent)
	{}

	/**
	 * @brief Получить текущий цвет
	 */
	virtual QColor currentColor() const = 0;

    /**
     * @brief Содержит ли панель заданный цвет
     */
    virtual bool contains(const QColor& _color) const = 0;

public slots:
	/**
	 * @brief Установить текущий цвет
	 */
	virtual void setCurrentColor(const QColor& _color) = 0;

signals:
	/**
	* @brief Выбран цвет
	*/
	void selected(const QColor& _color);
};

#endif // COLORSPANE_H
