/*
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

#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
    class ChangePasswordDialog;
}

class QPushButton;

namespace UserInterface
{
    /**
     * @brief Класс для окна смены пароля пользователя
     */
    class ChangePasswordDialog : public QLightBoxDialog
    {
        Q_OBJECT

    public:
        explicit ChangePasswordDialog(QWidget *_parent = 0);
        ~ChangePasswordDialog();

        /**
         * @brief Показать окно, очистив его предварительно
         */
        void showPrepared();

        /**
         * @brief Показать окно, не очищая его
         */
        void showUnprepared();

        /**
         * @brief Получить старый пароль
         */
        QString password() const;

        /**
         * @brief Получить новый пароль
         */
        QString newPassword() const;

        /**
         * @brief Скроем окно, предварительно разблокировав его
         */
        void stopAndHide();

        /**
         * @brief Блокируем окно и показываем прогрессбар
         */
        void block();

    signals:
        /**
         * @brief Пользователь нажал кнопку Сменить
         */
        void changeRequested();

    private:
        /**
         * @brief Пользователь ввел в одно из полей паролей
         */
        void setAcceptButtonAvailability();

        void initConnections();
        void initView();

    private:
        Ui::ChangePasswordDialog *m_ui;

        QPushButton *m_accept;
    };
}

#endif // CHANGEPASSWORDDIALOG_H
