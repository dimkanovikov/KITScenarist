#ifndef NAVIGATORITEMWIDGET_H
#define NAVIGATORITEMWIDGET_H

#include <QWidget>

class QLabel;
class ElidedLabel;


/**
 * @brief Виджет для отображения элементов навигатора
 */
class NavigatorItemWidget : public QWidget
{
	Q_OBJECT

public:
	explicit NavigatorItemWidget(QWidget *parent = 0);

	void setIcon(const QPixmap& _icon);
	void setHeader(const QString& _header);
	void setDescription(const QString& _description);
	void setTiming(const QString& _timing);

private:
	/**
	 * @brief Значёк элемента
	 */
	QLabel* m_icon;

	/**
	 * @brief Заголовок
	 */
	ElidedLabel* m_header;

	/**
	 * @brief Описание
	 */
	ElidedLabel* m_description;

	/**
	 * @brief Длительность
	 */
	QLabel* m_timing;
};

#endif // NAVIGATORITEMWIDGET_H
