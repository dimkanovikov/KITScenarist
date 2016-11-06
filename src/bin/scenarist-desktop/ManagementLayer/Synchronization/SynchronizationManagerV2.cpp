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

#include "SynchronizationManagerV2.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Helpers/PasswordStorage.h>

#include <WebLoader.h>

#include <QXmlStreamReader>
#include <QEventLoop>
#include <QTimer>

using ManagementLayer::SynchronizationManagerV2;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;

namespace {
    /**
     * @brief Список URL адресов, по которым осуществляются запросы
     */
    /** @{ */
    const QUrl URL_SIGNUP = QUrl("https://kitscenarist.ru/api/account/register/");
    const QUrl URL_RESTORE = QUrl("https://kitscenarist.ru/api/account/restore/");
    const QUrl URL_LOGIN = QUrl("https://kitscenarist.ru/api/account/login/");
    const QUrl URL_LOGOUT = QUrl("https://kitscenarist.ru/api/account/logout/");
    /** @} */

    /**
     * @brief Список названий параметров для запросов
     */
    /** @{ */
    const QString KEY_EMAIL = "email";
    const QString KEY_LOGIN = "login";
    const QString KEY_PASSWORD = "password";
    const QString KEY_SESSION_KEY = "session_key";

    /**
     * @brief Список кодов ошибок и соответствующих им описаний
     */
    /** @{ */
    const int UNKNOWN_ERROR_CODE = 100;
    const QString UNKNOWN_ERROR_STRING = QObject::tr("Unknown error");

    const int SESSION_KEY_NOT_FOUND_CODE = 101;
    const QString SESSION_KEY_NOT_FOUND_STRING = QObject::tr("Session key not found");
    /** @} */
}

SynchronizationManagerV2::SynchronizationManagerV2(QObject* _parent, QWidget* _parentView) :
    QObject(_parent),
    m_view(_parentView),
    m_loader(new WebLoader(this))
{

}

void SynchronizationManagerV2::autoLogin()
{
    //
    // Получим параметры из хранилища
    //
    const QString email =
            StorageFacade::settingsStorage()->value(
                "application/email",
                SettingsStorage::ApplicationSettings);
    const QString password =
            PasswordStorage::load(
                StorageFacade::settingsStorage()->value(
                    "application/password",
                    SettingsStorage::ApplicationSettings),
                email
                );

    //
    // Если они не пусты, авторизуемся
    //
    if (!email.isEmpty() && !password.isEmpty()) {
        login(email, password);
    }
}

void SynchronizationManagerV2::login(const QString &_email, const QString &_password)
{

    m_loader->setRequestMethod(WebLoader::Post);
    m_loader->clearRequestAttributes();
    m_loader->addRequestAttribute(KEY_LOGIN, _email);
    m_loader->addRequestAttribute(KEY_PASSWORD, _password);
    QByteArray response = m_loader->loadSync(URL_LOGIN);

    //
    // Считываем результат авторизации
    //
    QXmlStreamReader responseReader(response);
    //
    // Успешно ли завершилась авторизация
    //
    if(!isOperationSucceed(responseReader)) {
        return;
    }

    //
    // Найдем наш ключ сессии
    //
    m_sessionKey.clear();
    while (!responseReader.atEnd()) {
        responseReader.readNext();
        if (responseReader.name().toString() == "session_key") {
            responseReader.readNext();
            m_sessionKey = responseReader.text().toString();
            break;
        }
    }

    //
    // Не нашли ключ сессии
    //
    if (m_sessionKey.isEmpty()) {
        handleError(SESSION_KEY_NOT_FOUND_STRING, SESSION_KEY_NOT_FOUND_CODE);
        return;
    }

    //
    // Если авторизация успешна, сохраним информацию о пользователе
    //
    StorageFacade::settingsStorage()->setValue(
                "application/email",
                _email,
                SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                "application/password",
                PasswordStorage::save(_password, _email),
                SettingsStorage::ApplicationSettings);

    emit loginAccepted();

}

void SynchronizationManagerV2::signUp(const QString& _email, const QString& _password,
                                            const QString& _type)
{
    m_loader->setRequestMethod(WebLoader::Post);
    m_loader->clearRequestAttributes();
    m_loader->addRequestAttribute(KEY_EMAIL, _email);
    m_loader->addRequestAttribute(KEY_PASSWORD, _password);
    QByteArray response = m_loader->loadSync(URL_SIGNUP);

    //
    // Считываем результат авторизации
    //
    QXmlStreamReader responseReader(response);
    //
    // Успешно ли завершилась авторизация
    //
    if(!isOperationSucceed(responseReader)) {
        return;
    }

    //
    // Найдем наш ключ сессии
    //
    m_sessionKey.clear();
    while (!responseReader.atEnd()) {
        responseReader.readNext();
        if (responseReader.name().toString() == "session_key") {
            responseReader.readNext();
            m_sessionKey = responseReader.text().toString();
            break;
        }
    }

    //
    // Не нашли ключ сессии
    //
    if (m_sessionKey.isEmpty()) {
        handleError(SESSION_KEY_NOT_FOUND_STRING, SESSION_KEY_NOT_FOUND_CODE);
        return;
    }

    //
    // Если авторизация прошла
    //
    emit signUped();
}

void SynchronizationManagerV2::verify(const QString& _code)
{
    //
    // FIXME: Поменять в рабочей версии
    //
    QEventLoop event;
    QTimer::singleShot(2000, &event, SLOT(quit()));
    event.exec();

    bool success = false;
    if (_code == "11111") {
        success = true;
    } else {
        handleError("Wrong code", 505);
    }

    if (success) {
        emit verified();
    }
}

void SynchronizationManagerV2::restorePassword(const QString &_email)
{
    m_loader->setRequestMethod(WebLoader::Post);
    m_loader->clearRequestAttributes();
    m_loader->addRequestAttribute(KEY_EMAIL, _email);
    QByteArray response = m_loader->loadSync(URL_RESTORE);

    //
    // Считываем результат авторизации
    //
    QXmlStreamReader responseReader(response);
    //
    // Успешно ли завершилась авторизация
    //
    if(!isOperationSucceed(responseReader)) {
        return;
    }

    //
    // Найдем статус
    //
    m_sessionKey.clear();
    while (!responseReader.atEnd()) {
        responseReader.readNext();
        if (responseReader.name().toString() == "send_mail_result") {
            responseReader.readNext();
            QString status = responseReader.text().toString();
            if (status != "success") {
                handleError(status, UNKNOWN_ERROR_CODE);
                return;
            }
            break;
        }
    }

    //
    // Если успешно отправили письмо
    //
    emit restoredPassword();
}

void SynchronizationManagerV2::logout()
{
    QByteArray response;
    if (!m_sessionKey.isEmpty()) {
        m_loader->setRequestMethod(WebLoader::Post);
        m_loader->clearRequestAttributes();
        m_loader->addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
        response = m_loader->loadSync(URL_LOGOUT);
    }

    m_sessionKey.clear();

    //
    // Удаляем сохраненные значения, если они были
    //
    StorageFacade::settingsStorage()->setValue(
                "application/email",
                QString::null,
                SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                "application/password",
                QString::null,
                SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                "application/remote-projects",
                QString::null,
                SettingsStorage::ApplicationSettings);

    //
    // Если деавторизация прошла
    //
    emit logouted();
}

bool SynchronizationManagerV2::isOperationSucceed(QXmlStreamReader& _responseReader)
{
    while (!_responseReader.atEnd()) {
        _responseReader.readNext();
        if (_responseReader.name().toString() == "status") {
            //
            // Авторизация успешна
            //
            if (_responseReader.attributes().value("result").toString() == "true") {
                return true;
            } else {
                //
                // Попытаемся извлечь код ошибки
                //
                if (!_responseReader.attributes().hasAttribute("errorCode")) {
                    //
                    // Неизвестная ошибка
                    //
                    handleError(UNKNOWN_ERROR_STRING, UNKNOWN_ERROR_CODE);
                    return false;
                }

                int errorCode = _responseReader.attributes().value("errorCode").toInt();

                //
                // Попытаемся извлечь текст ошибки
                //
                QString errorText = UNKNOWN_ERROR_STRING;
                if (_responseReader.attributes().hasAttribute("error")) {
                    errorText = _responseReader.attributes().value("error").toString();
                }

                //
                // Скажем про ошибку
                //
                handleError(errorText, errorCode);
                return false;
            }
        }
    }
    //
    // Ничего не нашли про статус
    //
    handleError(tr("Unknown error"), 100);
    return false;
}

void SynchronizationManagerV2::handleError(const QString &_error, int _code)
{
    m_sessionKey.clear();
    emit syncClosedWithError(_code, _error);
}
