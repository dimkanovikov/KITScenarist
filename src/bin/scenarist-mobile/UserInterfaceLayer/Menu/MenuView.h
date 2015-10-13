#ifndef MENUVIEW_H
#define MENUVIEW_H

#include <QWidget>

namespace Ui {
	class MenuView;
}


namespace UserInterface
{
	/**
	 * @brief Представление меню
	 */
	class MenuView : public QWidget
	{
		Q_OBJECT

	public:
		explicit MenuView(QWidget* _parent = 0);
		~MenuView();

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

	private:
		Ui::MenuView* m_ui;
	};
}

#endif // MENUVIEW_H
