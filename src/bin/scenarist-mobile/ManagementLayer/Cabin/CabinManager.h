#ifndef CABINMANAGER_H
#define CABINMANAGER_H

#include <QObject>

namespace UserInterface {
	class CabinView;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий личным кабинетом пользователя
	 */
	class CabinManager : public QObject
	{
		Q_OBJECT

	public:
		explicit CabinManager(QObject* _parent, QWidget* _parentWidget);

		/**
		 * @brief Панель инструментов представления
		 */
		QWidget* toolbar() const;

		/**
		 * @brief Представление
		 */
		QWidget* view() const;

	signals:
		/**
		 * @brief Пользователь хочет разлогиниться
		 */
		void logoutRequested();

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

	private:
		/**
		 * @brief Представление
		 */
		UserInterface::CabinView* m_view;
	};
}

#endif // CABINMANAGER_H
