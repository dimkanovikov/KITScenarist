#ifndef LOCATIONSNAVIGATORITEMWIDGET_H
#define LOCATIONSNAVIGATORITEMWIDGET_H

#include <QWidget>

class ElidedLabel;


namespace UserInterface
{
	/**
	 * @brief Виджет для отображения элементов навигатора
	 */
	class LocationsNavigatorItemWidget : public QWidget
	{
		Q_OBJECT

	public:
		explicit LocationsNavigatorItemWidget(QWidget* _parent = 0);

		void setIcon(const QPixmap& _icon);
		void setName(const QString& _name);

	private:
		/**
		 * @brief Заголовок
		 */
		ElidedLabel* m_name;
	};
}

#endif // LOCATIONSNAVIGATORITEMWIDGET_H
