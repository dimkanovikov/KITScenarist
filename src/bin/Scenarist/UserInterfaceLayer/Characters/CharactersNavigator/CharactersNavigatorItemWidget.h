#ifndef CHARACTERSNAVIGATORITEMWIDGET_H
#define CHARACTERSNAVIGATORITEMWIDGET_H

#include <QWidget>

class ElidedLabel;


namespace UserInterface
{
	/**
	 * @brief Виджет для отображения элементов навигатора
	 */
	class CharactersNavigatorItemWidget : public QWidget
	{
		Q_OBJECT

	public:
		explicit CharactersNavigatorItemWidget(QWidget* _parent = 0);

		void setIcon(const QPixmap& _icon);
		void setName(const QString& _name);

	private:
		/**
		 * @brief Заголовок
		 */
		ElidedLabel* m_name;
	};
}

#endif // CHARACTERSNAVIGATORITEMWIDGET_H
