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

class QPushButton;

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

        /**
         * @brief Покажем окно, предварительно очистив его
         */
        void showPrepared();

        /**
         * @brief Покажем окно с благодарностью
         */
        void showThanks(const QString& _expDate);

        /**
         * @brief Установить стоимость подписки 1 месяца
         */
        void setPaymentMonth(int _paymentMonth);

    signals:
        /**
         * @brief Запрос на оплату в браузере
         */
        void renewSubsciptionRequested();

        /**
         * @brief Запрос, изменилась ли подписка
         */
        void subscriptionChangedRequest();

    private:
        /**
         * @brief Сменился период запрашиваемой подписки
         */
        void setPaymentText();

        /**
         * @brief Переводим окно в/из режим ожидания
         */
        void setWindowWaiting(bool _isWaiting);

        /**
         * @brief Настроить представление
         */
        void initView() override;

        /**
         * @brief Настроить соединения
         */
        void initConnections() override;

    private:
        Ui::RenewSubscriptionDialog *m_ui;

        /**
         * @brief Кнопка отправки запроса на платеж в браузер
         */
        QPushButton* m_acceptButton;

        int m_paymentMonth = 299;

        /**
         * @brief Принимая на вход число месяцев,
         *        выдает сумму платежа с учетом скидки
         */
        unsigned durationToAmount(unsigned _duration) const;
    };
}

#endif // RENEWSUBSCRIPTIONDIALOG_H
