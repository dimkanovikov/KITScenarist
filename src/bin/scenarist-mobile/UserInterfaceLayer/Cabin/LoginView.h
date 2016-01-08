#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWidget>

namespace Ui {
	class LoginView;
}


namespace UserInterface
{
	/**
	 * @brief Класс формы авторизации пользователя
	 */
	class LoginView : public QWidget
	{
		Q_OBJECT

	public:
		explicit LoginView(QWidget *parent = 0);
		~LoginView();

		/**
		 * @brief Получить панель инструментов
		 */
		QWidget* toolbar();

		/**
		 * @brief Очистить форму
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
		 * @brief Пользователь нажал кнопку "Войти"
		 */
		void loginClicked();

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
		 * @brief Настроить доступность элементов управления формой
		 */
		void setControlsEnabled(bool _enabled);

	private:
		/**
		 * @brief Интерфейс формы
		 */
		Ui::LoginView* m_ui;
	};
}

#endif // LOGINVIEW_H
