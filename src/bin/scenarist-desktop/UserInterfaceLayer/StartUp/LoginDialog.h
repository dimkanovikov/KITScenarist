#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
	class LoginDialog;
}

class TabBarExpanded;
class PasswordLineEdit;

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
         * @brief Email с виджета авторизации
         */
        QString loginEmail() const;

        /**
         * @brief Email с виджета регистрации
         */
        QString signUpEmail() const;

        /**
         * @brief Пароль с виджета авторизации
         */
        QString loginPassword() const;

        /**
         * @brief Пароль с виджета регис
    m_tabs->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);трации
         */
        QString signUpPassword() const;

        /**
         * @brief Проверочный код
         */
        QString verificationCode() const;

        /**
         * @brief Тип аккаунта
         */
        QString signUpType() const;

		/**
		 * @brief Установить сообщение об ошибке
		 */
        void setLoginError(const QString& _error);
        void setSignUpError(const QString& _error);
        void setVerificationError(const QString& _error);

        void setLoginPage();

        /**
         * @brief Установить текст для виджета проверочного кода
         */
        void showVerification();

        /**
         * @brief Установить текст для виджета восстановления пароля
         */
        void showRestore();

        /**
         * @brief Разблокировать окно для пользователя
         */
        void unblock();

        void show();

    signals:
        /**
         * @brief Пользователь хочет зарегистрироваться
         */
        void signUp();

        /**
         * @brief Пользователь хочет авторизоваться
         */
        void login();

        /**
         * @brief Пользователь хочет отправить проверочный код
         */
        void verify();

        /**
         * @brief Пользователь хочет восстановить пароль
         */
        void restore();

	protected:
		/**
		 * @brief При запуске фокусируемся на поле ввода имени пользователя
		 */
		QWidget* focusedOnExec() const;

    private slots:
        /**
         * @brief Проверка, что проверочный код удовлетворяет шаблону проверочных кодов
         */
        void checkCode();

        /**
         * @brief Убрать окно ввода проверочного кода и показывать вместо него окно регистрации
         */
        void cancelVerify();

        /**
         * @brief Смена активного виджета
         */
        void switchWidget();

        /**
         * @brief Блокирует окно на время передачи данных
         */
        void block();

        /**
         * @brief Пользователь нажал Enter
         *        В зависимости от окна, сделать нужный сигнал
         */
        void needAccept();

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
         * @brief Очистить окно
         */
        void clear();

	private:
		Ui::LoginDialog *ui;

        /**
         * @brief Поскольку один чекбокс на 2 виджета
         *        True - окно ввода проверочного кода
         *        False - окно регистрации
         */
        bool isVerify;

        /**
         * @brief Табы авторизации/Регистрации
         */
        TabBarExpanded* m_tabs;

        PasswordLineEdit* loginPasswordEdit;
        PasswordLineEdit* signUpPasswordEdit;
	};
}

#endif // LOGINDIALOG_H
