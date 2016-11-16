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

#ifndef RENEWSUBSCRIPTIONDIALOG_H
#define RENEWSUBSCRIPTIONDIALOG_H

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxdialog.h>

namespace Ui {
class RenewSubscriptionDialog;
}

namespace UserInterface
{
    class RenewSubscriptionDialog : public QLightBoxDialog
    {
        Q_OBJECT

    public:
        explicit RenewSubscriptionDialog(QWidget *_parent = 0);
        ~RenewSubscriptionDialog();

        /**
         * @brief Получить длительность периода запрашиваемой подписки
         */
        unsigned duration() const;

        /**
         * @brief Получить стоимость запрашиваемой подписки
         */
        unsigned amount() const;

        /**
         * @brief Получить тип оплаты. 0 банковская карта, 1 - Яндекс.Деньги
         */
        unsigned paymentSystemType() const;

    private:
        /**
         * @brief Сменился период запрашиваемой подписки
         */
        void setPaymentText();

    private:
        Ui::RenewSubscriptionDialog *m_ui;

        /**
         * @brief Принимая на вход число месяцев,
         *        выдает сумму платежа с учетом скидки
         */
        unsigned durationToAmount(unsigned _duration) const;
    };
}

#endif // RENEWSUBSCRIPTIONDIALOG_H
