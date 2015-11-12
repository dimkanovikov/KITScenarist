#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>

namespace Ui {
	class LoginDialog;
}


namespace UserInterface
{
	/**
	 * @brief Класс диалога авторизации пользователя
	 */
	class LoginDialog : public QWidget
	{
		Q_OBJECT

	public:
		explicit LoginDialog(QWidget* _parent = 0);
		~LoginDialog();

		/**
		 * @brief Очистить диалог
		 */
		void clear();

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

		/**
		 * @brief Установить видимость полосы загрузки
		 */
		/** @{ */
		void showProgressBar();
		void hideProgressBar();
		/** @{ */

	signals:
		/**
		 * @brief Сигналы нажатия кнопок
		 */
		/** @{ */
		void loginClicked();
		void cancelClicked();
		/** @} */

	private:
		/**
		 * @brief Настроить представление
		 */
		void initView();

		/**
		 * @brief Настроить соединения для формы
		 */
		void initConnections();

		/**
		 * @brief Настроить внешний вид
		 */
		void initStyleSheet();

		/**
		 * @brief Скорректировать размер
		 * @note Используется после отображения/сокрытия дополнительных виджетов
		 */
		void updateSize();

	private:
		/**
		 * @brief Интерфейс формы
		 */
		Ui::LoginDialog* m_ui;
	};
}

#endif // LOGINDIALOG_H
