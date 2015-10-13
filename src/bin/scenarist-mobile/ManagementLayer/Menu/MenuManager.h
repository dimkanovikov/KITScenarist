#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <QObject>

namespace UserInterface {
	class MenuView;
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
		/**
		 * @brief Представление меню
		 */
		UserInterface::MenuView* m_view;
	};
}

#endif // MENUMANAGER_H
