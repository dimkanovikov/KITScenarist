#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <QObject>

namespace UserInterface {
	class MenuView;
	class LoginDialog;
}

namespace ManagementLayer
{
	/**
	 * @brief Управляющий меню навигации приложения
	 */
	class MenuManager : public QObject
	{
		Q_OBJECT
	public:
		explicit MenuManager(QObject* _parent, QWidget* _parentWidget);

		/**
		 * @brief Получить представление меню
		 */
		QWidget* view() const;

		/**
		 * @brief Открыть меню
		 */
		void showMenu();

		/**
		 * @brief Пользователь с заданным именем успешно авторизован на сервере
		 */
		void userLogged();


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
		 * @brief Сигналы о нажатии соответствующих кнопок в меню
		 */
		/** @{ */
		void cabinRequested();
		void projectsRequested();
		void projectSaveRequested();
		void projectTextRequested();
		void settingsRequested();
		/** @} */

	private:
		/**
		 * @brief Настроить данные
		 */
		void initData();

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
		/**
		 * @brief Представление меню
		 */
		UserInterface::MenuView* m_view;
	};
}

#endif // MENUMANAGER_H
