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

		/**
		 * @brief Установить логин авторизованного пользователя
		 */
		void setUserLogged(const QString& _userName = QString::null);

		/**
		 * @brief Показать пункты меню для проекта
		 */
		void showProjectSubmenu(const QString& _projectName);

		/**
		 * @brief Спрятать пункты меню проекта
		 */
		void hideProjectSubmenu();

	signals:
		/**
		 * @brief Сигналы нажатий кнопок меню
		 */
		/** @{ */
		void backClicked();
		void cabinClicked();
		void projectsClicked();
		void projectSaveClicked();
		void projectTextClicked();
		void settingsClicked();
		/** @} */

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
