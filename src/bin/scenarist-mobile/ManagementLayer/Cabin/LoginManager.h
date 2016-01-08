#ifndef LOGINMANAGER_H
#define LOGINMANAGER_H

#include <QObject>

namespace UserInterface {
	class LoginView;
}


namespace ManagementLayer
{
	/**
	 * @brief Управляющий страницей авторизации/регистрации
	 */
	class LoginManager : public QObject
	{
		Q_OBJECT

	public:
		explicit LoginManager(QObject* _parent, QWidget* _parentWidget);

		/**
		 * @brief Панель инструментов представления
		 */
		QWidget* toolbar() const;

		/**
		 * @brief Представление
		 */
		QWidget* view() const;

		/**
		 * @brief Очистить диалог авторизации
		 */
		void clear();

		/**
		 * @brief Установить имя пользователя
		 */
		void setUserName(const QString& _userName);

		/**
		 * @brief Установить пароль пользователя
		 */
		void setPassword(const QString& _password);

		/**
		 * @brief Установить сообщение об ошибке
		 */
		void setError(const QString& _error);

		/**
		 * @brief Установить видимость полосы загрузки
		 */
		/** @{ */
		void showProgressBar();
		void hideProgressBar();
		/** @{ */

	signals:
		/**
		 * @brief Пользователь хочет авторизоваться
		 */
		void loginRequested(const QString& _userName, const QString& _password);

		/**
		 * @brief Пользователь хочет выйти
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
		 * @brief Представление для страницы авторизации
		 */
		UserInterface::LoginView* m_view;
	};
}

#endif // LOGINMANAGER_H
