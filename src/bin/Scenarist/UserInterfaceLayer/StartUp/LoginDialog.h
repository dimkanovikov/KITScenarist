#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
	class LoginDialog;
}


namespace UserInterface
{
	/**
	 * @brief Класс диалога авторизации пользователя
	 */
	class LoginDialog : public QLightBoxDialog
	{
		Q_OBJECT

	public:
		explicit LoginDialog(QWidget* _parent = 0);
		~LoginDialog();

		/**
		 * @brief Имя пользователя
		 */
		/** @{ */
		QString userName() const;
		void setUserName(const QString& _userName);
		/** @} */

		/**
		 * @brief Пароль пользователя
		 */
		/** @{ */
		QString password() const;
		void setPassword(const QString& _password);
		/** @} */

		/**
		 * @brief Установить сообщение об ошибке
		 */
		void setError(const QString& _error);

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения для формы
		 */
		void initConnections();

	private:
		Ui::LoginDialog *ui;
	};
}

#endif // LOGINDIALOG_H
