/*
* Copyright (C) 2014 Dimka Novikov, to@dimkanovikov.pro
* Copyright (C) 2016 Alexey Polushkin, armijo38@yandex.ru
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation; either
* version 3 of the License, or any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* Full license: http://dimkanovikov.pro/license/GPLv3
*/

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

class QLineEdit;
class QDialogButtonBox;
class QPushButton;

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
         * @brief Пароль с виджета регистрации
         */
        QString signUpPassword() const;

        /**
         * @brief Проверочный код
         */
        QString verificationCode() const;

		/**
		 * @brief Установить сообщение об ошибке
		 */
        void setLoginError(const QString& _error);
        void setSignUpError(const QString& _error);
        void setVerificationError(const QString& _error);

        /**
         * @brief Установить текст для виджета проверочного кода
         */
        void showVerificationSuccess();

        /**
         * @brief Установить текст для виджета восстановления пароля
         */
        void showRestoreSuccess();

        /**
         * @brief Показать окно, предварительно очистив
         *        и установив на авторизацию
         */
        void showPrepared();

    signals:
        /**
         * @brief Пользователь хочет зарегистрироваться
         */
        void signUpRequested();

        /**
         * @brief Пользователь хочет авторизоваться
         */
        void loginRequested();

        /**
         * @brief Пользователь хочет отправить проверочный код
         */
        void verifyRequested();

        /**
         * @brief Пользователь хочет восстановить пароль
         */
        void restoreRequested();

	protected:
		/**
		 * @brief При запуске фокусируемся на поле ввода имени пользователя
		 */
        QWidget* focusedOnExec() const override;

        /**
         * @brief Табы в заголовке окна
         */
        QWidget* titleWidget() const override;

    private slots:
        /**
         * @brief Проверка, что проверочный код удовлетворяет шаблону проверочных кодов
         */
        void checkVerificationCode();

        /**
         * @brief Проверка, что email для авторизации удовлетворяет шаблону
         */
        void checkLoginEmail();

        /**
         * @brief Проверка, что email для регистрации удовлетворяет шаблону
         */
        void checkSignUpEmail();

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
         * @brief Разблокировать окно для пользователя
         */
        void unblock();

        /**
         * @brief Пользователь нажал Enter
         *        В зависимости от окна, сделать нужный сигнал
         */
        void emitAccept();

	private:
		/**
		 * @brief Настроить представление
		 */
        void initView() override;

		/**
		 * @brief Настроить соединения для формы
		 */
        void initConnections() override;

        /**
         * @brief Очистить окно
         */
        void clear();

        /**
         * @brief Установить текст для метки
         */
        void updateLabel(QLabel* _label,
                           const QString& _message, bool _isError);

        /**
         * @brief Проверят, является ли строка email
         */
        void isEmail(QLineEdit* _line, QPushButton* _button);
	private:
        Ui::LoginDialog *m_ui;

        /**
         * @brief Поскольку один чекбокс на 2 виджета
         *        True - окно ввода проверочного кода
         *        False - окно регистрации
         */
        bool m_isVerify;

        QPushButton* m_loginButton;
        QPushButton* m_signUpButton;
	};
}

#endif // LOGINDIALOG_H
