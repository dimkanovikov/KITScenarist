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

#ifndef SYNCHRONIZATIONMANAGERV2_H
#define SYNCHRONIZATIONMANAGERV2_H

#include <QObject>

class WebLoader;
class QXmlStreamReader;

namespace ManagementLayer
{
    /**
     *  @brief Управляющий синхронизацией
     */
    class SynchronizationManagerV2 : public QObject
    {
        Q_OBJECT
    public:
        explicit SynchronizationManagerV2(QObject* _parent, QWidget* _parentView);

    public slots:
        /**
         * @brief Авторизоваться на сервере, используя сохраненные логин и пароль
         */
        void autoLogin();

        /**
         * @brief Авторизоваться на сервере
         */
        void login(const QString& _email, const QString& _password);

        /**
         * @brief Регистрация на сервере
         */
        void signUp(const QString& _email, const QString& _password,
                          const QString& _type);

        /**
         * @brief Подтверждение регистрации при помощи проверочного кода
         */
        void verification(const QString& _code);

        /**
         * @brief Восстановление пароля
         */
        void restorePassword(const QString& _email);

    signals:
        /**
         * @brief Авторизация пройдена успешно
         */
        void loginAccepted();

        /**
         * @brief Сервер успешно принял данные пользователя на регистрацию
         */
        void signUped();

        /**
         * @brief Сервер подтвердил регистрацию
         */
        void verified();

        /**
         * @brief Пароль отправлен на email
         */
        void restoredPassword();

        /**
         * @brief Ошибка
         */
        void syncClosedWithError(int, QString);

    private:
        /**
         * @brief Проверка, что статус ответа - ок
         */
        bool checkSuccess(QXmlStreamReader& _reader);
        /**
         * Обработка ошибок
         */
        void handleError(const QString& _error, int _code = 0);

        /**
         * @brief указатель на главную форму приложения
         */
        QWidget* m_view;

        /**
         * Ключ сессии
         */
        QString m_sessionKey;

        /**
         * Загрузчик
         */
        WebLoader *m_loader;
    };
}

#endif // SYNCHRONIZATIONMANAGERV2_H
