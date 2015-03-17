#ifndef SYNCHRONIZATIONMANAGER_H
#define SYNCHRONIZATIONMANAGER_H

#include <QObject>


namespace ManagementLayer
{
	/**
	 * @brief Управляющий синхронизацией
	 */
	class SynchronizationManager : public QObject
	{
		Q_OBJECT

	public:
		explicit SynchronizationManager(QObject* _parent = 0);

		/**
		 * @brief Авторизация
		 */
		/** @{ */
		bool logIn(const QString& _login, const QString& _password);
		bool logOut() { return true; }
		/** @} */

		/**
		 * @brief Получить список синхронизируемых проектов
		 * @note Тут должны быть как собственные, так и те, к которым предоставлен доступ
		 */
		void projectsList() {}

		/**
		 * @brief Синхронизировать проект
		 */
		void syncProject() {}

		/**
		 * @brief Передать управляющему "слепок" с изменениями
		 */
		void appendChanges() {}

		/**
		 * @brief Передать управляющему позицию курсора
		 */
		void setCursorPosition(int _position);

	signals:
		/**
		 * @brief Обновить информацию о курсорах
		 */
		void updateCursors();

		/**
		 * @brief Применить изменения принятые от сервера
		 */
		void applyChanges();

	private slots:
		/**
		 * @brief Оправить изменения на сервер
		 */
		void aboutSendChangesToServer() {}

		/**
		 * @brief Получены изменения от сервера
		 */
		void aboutGetChangesFromServer() {}

	private:
		/**
		 * @brief Авторизован ли пользователь в данный момент
		 */
		bool isLogged() const { return true; }

		/**
		 * @brief Настроить соединения
		 */
		void initConnections();

	private:
		/**
		 * @brief Позиция курсора
		 */
		int m_cursorPosition;
	};
}

#endif // SYNCHRONIZATIONMANAGER_H
