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

#include "SynchronizationManager.h"
#include "Sync.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Helpers/PasswordStorage.h>

#include <NetworkRequest.h>
#include <NetworkRequestLoader.h>

#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QEventLoop>
#include <QTimer>
#include <QXmlStreamReader>

using ManagementLayer::SynchronizationManager;
using ManagementLayer::Sync;
using DataStorageLayer::StorageFacade;
using DataStorageLayer::SettingsStorage;

//
// **** из старого менеджера синхронизации
//

#include <ManagementLayer/Project/ProjectsManager.h>
#include <ManagementLayer/Project/Project.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioChangeStorage.h>
#include <DataLayer/DataStorageLayer/DatabaseHistoryStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>
#include <DataLayer/DataStorageLayer/ScenarioStorage.h>

#include <DataLayer/Database/Database.h>

#include <Domain/Scenario.h>
#include <Domain/ScenarioChange.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>
#include <3rd_party/Helpers/PasswordStorage.h>

#include <NetworkRequest.h>

#include <QEventLoop>
#include <QHash>
#include <QScopedPointer>
#include <QTimer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

using ManagementLayer::ProjectsManager;

namespace {
    /**
     * @brief Список URL адресов, по которым осуществляются запросы
     */
    /** @{ */
    const QUrl URL_SIGNUP = QUrl("https://kitscenarist.ru/api/account/register/");
    const QUrl URL_RESTORE = QUrl("https://kitscenarist.ru/api/account/restore/");
    const QUrl URL_LOGIN = QUrl("https://kitscenarist.ru/api/account/login/");
    const QUrl URL_LOGOUT = QUrl("https://kitscenarist.ru/api/account/logout/");
    const QUrl URL_UPDATE = QUrl("https://kitscenarist.ru/api/account/update/");
    const QUrl URL_SUBSCRIBE_STATE = QUrl("https://kitscenarist.ru/api/account/subscribe/state/");
    //
    const QUrl URL_PROJECTS = QUrl("https://kitscenarist.ru/api/projects/");
    const QUrl URL_CREATE_PROJECT = QUrl("https://kitscenarist.ru/api/projects/create/");
    const QUrl URL_UPDATE_PROJECT = QUrl("https://kitscenarist.ru/api/projects/edit/");
    const QUrl URL_REMOVE_PROJECT = QUrl("https://kitscenarist.ru/api/projects/remove/");
    const QUrl URL_CREATE_PROJECT_SUBSCRIPTION = QUrl("https://kitscenarist.ru/api/projects/share/create/");
    const QUrl URL_REMOVE_PROJECT_SUBSCRIPTION = QUrl("https://kitscenarist.ru/api/projects/share/remove/");
    //
    const QUrl URL_SCENARIO_CHANGE_LIST = QUrl("https://kitscenarist.ru/api/projects/scenario/change/list/");
    const QUrl URL_SCENARIO_CHANGE_LOAD = QUrl("https://kitscenarist.ru/api/projects/scenario/change/");
    const QUrl URL_SCENARIO_CHANGE_SAVE = QUrl("https://kitscenarist.ru/api/projects/scenario/change/save/");
    const QUrl URL_SCENARIO_CURSORS = QUrl("https://kitscenarist.ru/api/projects/scenario/cursor/");
    //
    const QUrl URL_SCENARIO_DATA_LIST = QUrl("https://kitscenarist.ru/api/projects/data/list/");
    const QUrl URL_SCENARIO_DATA_LOAD = QUrl("https://kitscenarist.ru/api/projects/data/");
    const QUrl URL_SCENARIO_DATA_SAVE = QUrl("https://kitscenarist.ru/api/projects/data/save/");
    //
    const QUrl URL_CHECK_NETWORK_STATE = QUrl("http://kitscenarist.ru/api/app/connection/");
    /** @} */

    /**
     * @brief Список названий параметров для запросов
     */
    /** @{ */
    const QString KEY_EMAIL = "email";
    const QString KEY_LOGIN = "login";
    const QString KEY_USERNAME = "username";
    const QString KEY_PASSWORD = "password";
    const QString KEY_NEW_PASSWORD = "new_password";
    const QString KEY_SESSION_KEY = "session_key";
    const QString KEY_ROLE = "role";
    //
    const QString KEY_PROJECT_ID = "project_id";
    const QString KEY_PROJECT_NAME = "project_name";
    /** @} */

    //
    // **** из старого менеджера синхронизации
    //

    /**
     * @brief Ключи для параметров запросов
     */
    /** @{ */
    const QString KEY_USER_NAME = "login";
    const QString KEY_PROJECT = "project_id";
    const QString KEY_CHANGES = "changes";
    const QString KEY_CHANGES_IDS = "changes_ids";
    const QString KEY_SCENARIO_IS_DRAFT = "scenario_is_draft";
    const QString KEY_FROM_LAST_MINUTES = "from_last_minutes";
    const QString KEY_CURSOR_POSITION = "cursor_position";
    /** @{ */

    /**
     * @brief Ключи для доступа к данным об изменении сценария
     */
    /** @{ */
    const QString SCENARIO_CHANGE_ID = "id";
    const QString SCENARIO_CHANGE_DATETIME = "datetime";
    const QString SCENARIO_CHANGE_USERNAME = "username";
    const QString SCENARIO_CHANGE_UNDO_PATCH = "undo_patch";
    const QString SCENARIO_CHANGE_REDO_PATCH = "redo_patch";
    const QString SCENARIO_CHANGE_IS_DRAFT = "is_draft";
    /** @} */

    /**
     * @brief Ключи для доступа к записям из истории изменений БД
     */
    /** @{ */
    const QString DBH_ID_KEY = "id";
    const QString DBH_QUERY_KEY = "query";
    const QString DBH_QUERY_VALUES_KEY = "query_values";
    const QString DBH_USERNAME_KEY = "username";
    const QString DBH_DATETIME_KEY = "datetime";
    const QString DBH_ORDER_KEY = "order";
    /** @} */

    const bool IS_CLEAN = false;
    const bool IS_DRAFT = true;

    /**
     * @brief Код ошибки означающий работу в автономном режиме
     */
    const QString INCORRECT_SESSION_KEY = "xxxxxxxxxxxxxxx";
}

namespace {
    /**
     * @brief Преобразовать дату из гггг.мм.дд чч:мм:сс в дд.мм.гггг
     */
    QString dateTransform(const QString &_date)
    {
        return QDateTime::fromString(_date, "yyyy-MM-dd hh:mm:ss").toString("dd.MM.yyyy");
    }
}

SynchronizationManager::SynchronizationManager(QObject* _parent, QWidget* _parentView) :
    QObject(_parent),
    m_view(_parentView),
    m_isSubscriptionActive(false)
{
    initConnections();
}

bool SynchronizationManager::isInternetConnectionActive() const
{
    return m_internetConnectionStatus == Active;
}

bool SynchronizationManager::isLogged() const
{
    return !m_sessionKey.isEmpty();
}

bool SynchronizationManager::isSubscriptionActive() const
{
    return m_isSubscriptionActive;
}

bool SynchronizationManager::autoLogin()
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
        return true;
    }
    return false;
}

void SynchronizationManager::login(const QString &_email, const QString &_password)
{
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_LOGIN, _email);
    loader.addRequestAttribute(KEY_PASSWORD, _password);
    QByteArray response = loader.loadSync(URL_LOGIN);

    //
    // Считываем результат авторизации
    //
    QXmlStreamReader responseReader(response);
    //
    // Успешно ли завершилась авторизация
    //
    if(!isOperationSucceed(responseReader)) {
        //
        // Если неполадки с интернетом, т.е. работает в оффлайн режиме
        //
        if (m_sessionKey == INCORRECT_SESSION_KEY) {
            //
            // Попробуем загрузить email
            //
            const QString userEmail =
                        DataStorageLayer::StorageFacade::settingsStorage()->value(
                            "application/email",
                            DataStorageLayer::SettingsStorage::ApplicationSettings);
            //
            // Если ранее были авторизованы
            //
            if (!userEmail.isEmpty()) {
                //
                // Запомним email
                //
                m_userEmail = userEmail;

                //
                // Загрузим всю требуемую информацию из кэша (имя пользователя,
                // активность и дату подписки)
                //
                const QString userName =
                            DataStorageLayer::StorageFacade::settingsStorage()->value(
                                "application/username",
                                DataStorageLayer::SettingsStorage::ApplicationSettings);
                m_isSubscriptionActive =
                            DataStorageLayer::StorageFacade::settingsStorage()->value(
                                "application/subscriptionIsActive",
                                DataStorageLayer::SettingsStorage::ApplicationSettings).toInt();
                const QString date =
                            DataStorageLayer::StorageFacade::settingsStorage()->value(
                                "application/subscriptionExpiredDate",
                                DataStorageLayer::SettingsStorage::ApplicationSettings);
                int paymentMonth =
                            DataStorageLayer::StorageFacade::settingsStorage()->value(
                                "application/subscriptionPaymentMonth",
                                DataStorageLayer::SettingsStorage::ApplicationSettings).toInt();

                //
                // Уведомим об этом
                //
                emit subscriptionInfoLoaded(m_isSubscriptionActive, dateTransform(date));
                emit loginAccepted(userName, m_userEmail, paymentMonth);

                //
                // Хоть как то авторизовались, тепер нас интересует статус интернета
                //
                checkNetworkState();
            }
        }
        return;
    }

    QString userName;
    QString date;
    int paymentMonth = -1;

    //
    // Найдем наш ключ сессии, имя пользователя, информацию о подписке
    //
    m_sessionKey.clear();
    bool isActiveFind = false;
    while (!responseReader.atEnd()) {
        responseReader.readNext();
        if (responseReader.name().toString() == "session_key") {
            responseReader.readNext();
            m_sessionKey = responseReader.text().toString();
            responseReader.readNext();
        } else if (responseReader.name().toString() == "user_name") {
            responseReader.readNext();
            userName = responseReader.text().toString();
            responseReader.readNext();
        } else if (responseReader.name().toString() == "subscribe_is_active") {
            isActiveFind = true;
            responseReader.readNext();
            m_isSubscriptionActive = responseReader.text().toString() == "true";
            responseReader.readNext();
        } else if (responseReader.name().toString() == "subscribe_end") {
            responseReader.readNext();
            date = responseReader.text().toString();
            responseReader.readNext();
        } else if (responseReader.name().toString() == "payment_month") {
            responseReader.readNext();
            paymentMonth = responseReader.text().toInt();
            responseReader.readNext();
        }
    }

    if (!isActiveFind || (isActiveFind && m_isSubscriptionActive && date.isEmpty())
            || paymentMonth < 0) {
        handleError(Sync::UnknownError);
        m_sessionKey.clear();
        return;
    }

    //
    // Не нашли ключ сессии
    //
    if (m_sessionKey.isEmpty()) {
        handleError(Sync::NoSessionKeyError);
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
    StorageFacade::settingsStorage()->setValue(
                "application/username",
                userName,
                SettingsStorage::ApplicationSettings);

    //
    // ... и о подписке
    //
    StorageFacade::settingsStorage()->setValue(
                "application/subscriptionIsActive",
                QString::number(m_isSubscriptionActive),
                SettingsStorage::ApplicationSettings);

    StorageFacade::settingsStorage()->setValue(
                "application/subscriptionExpiredDate",
                dateTransform(date),
                SettingsStorage::ApplicationSettings);

    StorageFacade::settingsStorage()->setValue(
                "application/subscriptionPaymentMonth",
                QString::number(paymentMonth),
                SettingsStorage::ApplicationSettings);

    //
    // Запомним email
    //
    m_userEmail = _email;

    emit subscriptionInfoLoaded(m_isSubscriptionActive, dateTransform(date));
    emit loginAccepted(userName, m_userEmail, paymentMonth);

    //
    // Авторизовались, тепер нас интересует статус интернета
    //
    checkNetworkState();
}

void SynchronizationManager::signUp(const QString& _email, const QString& _password)
{
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_EMAIL, _email);
    loader.addRequestAttribute(KEY_PASSWORD, _password);
    QByteArray response = loader.loadSync(URL_SIGNUP);

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
            responseReader.readNext();
            break;
        }
    }

    //
    // Не нашли ключ сессии
    //
    if (m_sessionKey.isEmpty()) {
        handleError(Sync::NoSessionKeyError);
        return;
    }

    //
    // Задаём умолчальное имя пользователя
    //
    changeUserName(_email.split("@").first());

    //
    // Обновим информацию о подписке
    //
    loadSubscriptionInfo();

    //
    // Если авторизация прошла
    //
    emit signUpFinished();
}

void SynchronizationManager::verify(const QString& _code)
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
        handleError(Sync::IncorrectValidationCodeError);
    }

    if (success) {
        emit verified();
    }
}

void SynchronizationManager::restorePassword(const QString &_email)
{
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_EMAIL, _email);
    QByteArray response = loader.loadSync(URL_RESTORE);

    //
    // Считываем результат
    //
    QXmlStreamReader responseReader(response);
    //
    // Успешно ли завершилась операция
    //
    if (!isOperationSucceed(responseReader)) {
        return;
    }

    //
    // Найдем статус
    //
    while (!responseReader.atEnd()) {
        responseReader.readNext();
        if (responseReader.name().toString() == "send_mail_result") {
            responseReader.readNext();
            QString status = responseReader.text().toString();
            responseReader.readNext();
            if (status != "success") {
                handleError(status, Sync::UnknownError);
                return;
            }
            break;
        }
    }

    //
    // Если успешно отправили письмо
    //
    emit passwordRestored();
}

void SynchronizationManager::logout()
{
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
    QByteArray response = loader.loadSync(URL_LOGOUT);

    m_sessionKey.clear();
    m_userEmail.clear();

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
                "application/username",
                QString::null,
                SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                "application/remote-projects",
                QString::null,
                SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                "application/subscriptionIsActive",
                QString::null,
                SettingsStorage::ApplicationSettings);
    StorageFacade::settingsStorage()->setValue(
                "application/subscriptionExpiredDate",
                QString::null,
                SettingsStorage::ApplicationSettings);

    //
    // Если деавторизация прошла
    //
    emit logoutFinished();

    //
    // Теперь статус интернета не отслеживается, а значит неизвестен
    //
    m_internetConnectionStatus = Undefined;
}

void SynchronizationManager::renewSubscription(unsigned _duration,
                                                 unsigned _type)
{
    QDesktopServices::openUrl(QUrl(QString("http://kitscenarist.ru/api/account/subscribe/?"
                                           "user=%1&month=%2&payment_type=%3").
                                   arg(m_userEmail).arg(_duration).
                                   arg(_type == 0 ? "AC" : "PC")));
}

void SynchronizationManager::changeUserName(const QString &_newUserName)
{
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
    loader.addRequestAttribute(KEY_USERNAME, _newUserName);
    QByteArray response = loader.loadSync(URL_UPDATE);

    //
    // Считываем результат авторизации
    //
    QXmlStreamReader responseReader(response);

    if (!isOperationSucceed(responseReader)) {
        return;
    }

    //
    // Сохраним измененную информацию
    //
    StorageFacade::settingsStorage()->setValue(
                "application/username",
                _newUserName,
                SettingsStorage::ApplicationSettings);

    emit userNameChanged();
}

void SynchronizationManager::loadSubscriptionInfo()
{
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
    QByteArray response = loader.loadSync(URL_SUBSCRIBE_STATE);

    //
    // Считываем результат
    //
    QXmlStreamReader responseReader(response);
    if (!isOperationSucceed(responseReader)) {
        return;
    }

    //
    // Распарсим результат
    //
    QString date;
    bool isActiveFind = false;
    while (!responseReader.atEnd()) {
        responseReader.readNext();
        if (responseReader.name().toString() == "subscribe_is_active") {
            isActiveFind = true;
            responseReader.readNext();
            m_isSubscriptionActive = responseReader.text().toString() == "true";
            responseReader.readNext();
        } else if (responseReader.name().toString() == "subscribe_end") {
            responseReader.readNext();
            date = responseReader.text().toString();
            responseReader.readNext();
        }
    }

    if (!isActiveFind || (isActiveFind && m_isSubscriptionActive && date.isEmpty())) {
        handleError(Sync::UnknownError);
        return;
    }

    StorageFacade::settingsStorage()->setValue(
                "application/subscriptionIsActive",
                QString::number(m_isSubscriptionActive),
                SettingsStorage::ApplicationSettings);

    StorageFacade::settingsStorage()->setValue(
                "application/subscriptionExpiredDate",
                dateTransform(date),
                SettingsStorage::ApplicationSettings);

    emit subscriptionInfoLoaded(m_isSubscriptionActive, dateTransform(date));
}

void SynchronizationManager::changePassword(const QString& _password,
                                              const QString& _newPassword)
{
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
    loader.addRequestAttribute(KEY_PASSWORD, _password);
    loader.addRequestAttribute(KEY_NEW_PASSWORD, _newPassword);
    QByteArray response = loader.loadSync(URL_UPDATE);

    //
    // Считываем результат авторизации
    //
    QXmlStreamReader responseReader(response);

    if (!isOperationSucceed(responseReader)) {
        return;
    }
    emit passwordChanged();
}

void SynchronizationManager::loadProjects()
{
    //
    // Получаем список проектов
    //
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
    const QByteArray response = loader.loadSync(URL_PROJECTS);

    //
    // Считываем результат
    //
    QXmlStreamReader responseReader(response);
    if (!isOperationSucceed(responseReader)) {
        //
        // Если работает в автономном режиме, то загрузим из кэша
        //
        if (m_sessionKey == INCORRECT_SESSION_KEY) {
            QByteArray cachedProjectsXml =
                    QByteArray::fromBase64(
                        DataStorageLayer::StorageFacade::settingsStorage()->value(
                            "application/remote-projects",
                            DataStorageLayer::SettingsStorage::ApplicationSettings).toUtf8()
                        );
            emit projectsLoaded(cachedProjectsXml);
        }
        return;
    }

    //
    // Сохраняем список проектов для работы в автономном режиме
    //
    StorageFacade::settingsStorage()->setValue("application/remote-projects",
        response.toBase64(), SettingsStorage::ApplicationSettings);

    //
    // Уведомляем о получении проектов
    //
    emit projectsLoaded(response);
}

int SynchronizationManager::createProject(const QString& _projectName)
{
    const int INVALID_PROJECT_ID = -1;

    //
    // Создаём новый проект
    //
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
    loader.addRequestAttribute(KEY_PROJECT_NAME, _projectName);
    const QByteArray response = loader.loadSync(URL_CREATE_PROJECT);

    //
    // Считываем результат
    //
    QXmlStreamReader responseReader(response);
    if (!isOperationSucceed(responseReader)) {
        return INVALID_PROJECT_ID;
    }

    //
    // Считываем идентификатор проекта
    //
    int newProjectId = INVALID_PROJECT_ID;
    while (!responseReader.atEnd()) {
        responseReader.readNext();
        if (responseReader.name().toString() == "project") {
            newProjectId = responseReader.attributes().value("id").toInt();
            break;
        }
    }

    if (newProjectId == INVALID_PROJECT_ID) {
        handleError(Sync::UnknownError);
        return INVALID_PROJECT_ID;
    }

    //
    // Если проект успешно добавился перечитаем список проектов
    //
    loadProjects();

    return newProjectId;
}

void SynchronizationManager::updateProjectName(int _projectId, const QString& _newProjectName)
{
    //
    // Обновляем проект
    //
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
    loader.addRequestAttribute(KEY_PROJECT_ID, _projectId);
    loader.addRequestAttribute(KEY_PROJECT_NAME, _newProjectName);
    const QByteArray response = loader.loadSync(URL_UPDATE_PROJECT);

    //
    // Считываем результат
    //
    QXmlStreamReader responseReader(response);
    if (!isOperationSucceed(responseReader)) {
        return;
    }

    //
    // Если проект успешно обновился перечитаем список проектов
    //
    loadProjects();
}

void SynchronizationManager::removeProject(int _projectId)
{
    //
    // Удаляем проект
    //
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
    loader.addRequestAttribute(KEY_PROJECT_ID, _projectId);
    const QByteArray response = loader.loadSync(URL_REMOVE_PROJECT);

    //
    // Считываем результат
    //
    QXmlStreamReader responseReader(response);
    if (!isOperationSucceed(responseReader)) {
        return;
    }

    //
    // Если проект успешно удалён перечитаем список проектов
    //
    loadProjects();
}

void SynchronizationManager::shareProject(int _projectId, const QString& _userEmail, int _role)
{
    const QString userRole = _role == 0 ? "redactor" : "commentator";

    //
    // ДОбавляем подписчика в проект
    //
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
    loader.addRequestAttribute(KEY_PROJECT_ID, _projectId);
    loader.addRequestAttribute(KEY_EMAIL, _userEmail);
    loader.addRequestAttribute(KEY_ROLE, userRole);
    const QByteArray response = loader.loadSync(URL_CREATE_PROJECT_SUBSCRIPTION);

    //
    // Считываем результат
    //
    QXmlStreamReader responseReader(response);
    if (!isOperationSucceed(responseReader)) {
        return;
    }

    //
    // Если подписчик добавлен, перечитаем список проектов
    //
    loadProjects();
}

void SynchronizationManager::unshareProject(int _projectId, const QString& _userEmail)
{
    //
    // Убираем подписчика из проекта
    //
    NetworkRequest loader;
    loader.setRequestMethod(NetworkRequest::Post);
    loader.clearRequestAttributes();
    loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
    loader.addRequestAttribute(KEY_PROJECT_ID, _projectId);
    loader.addRequestAttribute(KEY_EMAIL, _userEmail.trimmed());
    const QByteArray response = loader.loadSync(URL_REMOVE_PROJECT_SUBSCRIPTION);

    //
    // Считываем результат
    //
    QXmlStreamReader responseReader(response);
    if (!isOperationSucceed(responseReader)) {
        return;
    }

    //
    // Если подписчик удалён перечитаем список проектов
    //
    loadProjects();
}

void SynchronizationManager::aboutFullSyncScenario()
{
    if (isCanSync()) {
        //
        // Запоминаем время синхронизации изменений сценария, в дальнейшем будем отправлять
        // изменения произведённые с данного момента
        //
        m_lastChangesSyncDatetime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");

        //
        // Получить список патчей проекта
        //
        NetworkRequest loader;
        loader.setRequestMethod(NetworkRequest::Post);
        loader.clearRequestAttributes();
        loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
        loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
        QByteArray response = loader.loadSync(URL_SCENARIO_CHANGE_LIST);


        QXmlStreamReader changesReader(response);
        if (!isOperationSucceed(changesReader)) {
            return;
        }


        //
        // ... считываем изменения (uuid)
        //
        QList<QString> remoteChanges;
        while (!changesReader.atEnd()) {
            changesReader.readNextStartElement();
            if (changesReader.name() == "change") {
                const QString changeUuid = changesReader.attributes().value("id").toString();
                if (!changeUuid.isEmpty()) {
                    remoteChanges.append(changeUuid);
                }
            }
        }


        //
        // Сформируем список изменений сценария хранящихся локально
        //
        QList<QString> localChanges = StorageFacade::scenarioChangeStorage()->uuids();


        //
        // Отправить на сайт все изменения, которых там нет
        //
        {
            QList<QString> changesForUpload;
            foreach (const QString& changeUuid, localChanges) {
                //
                // ... отправлять нужно, если такого изменения нет на сайте
                //
                const bool needUpload = !remoteChanges.contains(changeUuid);

                if (needUpload) {
                    changesForUpload.append(changeUuid);
                }
            }
            //
            // ... отправляем
            //
            uploadScenarioChanges(changesForUpload);
        }

        //
        // Скачать все изменения, которых ещё нет
        //
        {
            QStringList changesForDownload;
            foreach (const QString& changeUuid, remoteChanges) {
                //
                // ... сохранять нужно, если такого изменения нет в локальной БД
                //
                const bool needDownload = !localChanges.contains(changeUuid);

                if (needDownload) {
                    changesForDownload.append(changeUuid);
                }
            }
            //
            // ... скачиваем
            //
            const QList<QHash<QString, QString> > changes =
                    downloadScenarioChanges(changesForDownload.join(";"));
            //
            // ... применять будет пачками
            //
            QList<QString> cleanPatches;
            QList<QString> draftPatches;
            QHash<QString, QString> change;
            foreach (change, changes) {
                if (!change.isEmpty()) {
                    //
                    // ... добавляем
                    //
                    auto* addedChange =
                            DataStorageLayer::StorageFacade::scenarioChangeStorage()->append(
                                change.value(SCENARIO_CHANGE_ID), change.value(SCENARIO_CHANGE_DATETIME),
                                change.value(SCENARIO_CHANGE_USERNAME), change.value(SCENARIO_CHANGE_UNDO_PATCH),
                                change.value(SCENARIO_CHANGE_REDO_PATCH), change.value(SCENARIO_CHANGE_IS_DRAFT).toInt());
                    if (addedChange != nullptr) {
                        if (change.value(SCENARIO_CHANGE_IS_DRAFT).toInt()) {
                            draftPatches.append(change.value(SCENARIO_CHANGE_REDO_PATCH));
                        } else {
                            cleanPatches.append(change.value(SCENARIO_CHANGE_REDO_PATCH));
                        }
                    }
                }
            }
            //
            // ... применяем
            //
            if (!cleanPatches.isEmpty()) {
                emit applyPatchesRequested(cleanPatches, IS_CLEAN);
            }
            if (!draftPatches.isEmpty()) {
                emit applyPatchesRequested(draftPatches, IS_DRAFT);
            }

            //
            // ... сохраняем
            //
            DataStorageLayer::StorageFacade::scenarioChangeStorage()->store();
        }
    }
}

void SynchronizationManager::aboutWorkSyncScenario()
{
    if (isCanSync()) {
        //
        // Защитимся от множественных выховов
        //
        static bool s_isInWorkSync = false;
        if (s_isInWorkSync) {
            return;
        }

        s_isInWorkSync = true;

        //
        // Отправляем новые изменения
        //
        {
            //
            // Запоминаем время синхронизации изменений сценария
            //
            const QString prevChangesSyncDatetime = m_lastChangesSyncDatetime;
            m_lastChangesSyncDatetime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");

            //
            // Отправляем
            //
            QList<QString> newChanges =
                    StorageFacade::scenarioChangeStorage()->newUuids(prevChangesSyncDatetime);
            const bool changesUploaded = uploadScenarioChanges(newChanges);

            //
            // Не обновляем время последней синхронизации, если изменения не были отправлены
            //
            if (changesUploaded == false) {
                m_lastChangesSyncDatetime = prevChangesSyncDatetime;
            }
        }

        //
        // Загружаем и применяем изменения от других пользователей за последние LAST_MINUTES минут
        //
        {
            const int LAST_MINUTES = 2;

            NetworkRequest loader;
            loader.setRequestMethod(NetworkRequest::Post);
            loader.clearRequestAttributes();
            loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
            loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
            loader.addRequestAttribute(KEY_FROM_LAST_MINUTES, LAST_MINUTES);
            QByteArray response = loader.loadSync(URL_SCENARIO_CHANGE_LIST);

            QXmlStreamReader changesReader(response);
            if (!isOperationSucceed(changesReader)) {
                return;
            }

            //
            // ... считываем uuid'ы новых изменений
            //
            QList<QString> remoteChanges;
            while (!changesReader.atEnd()) {
                changesReader.readNextStartElement();
                if (changesReader.name() == "change") {
                    const QString changeUuid = changesReader.attributes().value("id").toString();
                    if (!changeUuid.isEmpty()) {
                        remoteChanges.append(changeUuid);
                    }
                }
            }

            //
            // ... скачиваем все изменения, которых ещё нет
            //
            QStringList changesForDownload;
            foreach (const QString& changeUuid, remoteChanges) {
                //
                // ... сохранять нужно, если такого изменения нет
                //
                const bool needDownload =
                        !DataStorageLayer::StorageFacade::scenarioChangeStorage()->contains(changeUuid);

                if (needDownload) {
                    changesForDownload.append(changeUuid);
                }
            }
            //
            // ... скачиваем
            //
            const QList<QHash<QString, QString> > changes = downloadScenarioChanges(changesForDownload.join(";"));
            QHash<QString, QString> change;
            foreach (change, changes) {
                if (!change.isEmpty()) {
                    //
                    // ... сохраняем
                    //
                    auto* addedChange =
                            StorageFacade::scenarioChangeStorage()->append(
                                change.value(SCENARIO_CHANGE_ID), change.value(SCENARIO_CHANGE_DATETIME),
                                change.value(SCENARIO_CHANGE_USERNAME), change.value(SCENARIO_CHANGE_UNDO_PATCH),
                                change.value(SCENARIO_CHANGE_REDO_PATCH), change.value(SCENARIO_CHANGE_IS_DRAFT).toInt());
                    if (addedChange != nullptr) {
                        //
                        // ... применяем
                        //
//                        qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
//                        qDebug() << change.value(SCENARIO_CHANGE_ID);
                        emit applyPatchRequested(change.value(SCENARIO_CHANGE_REDO_PATCH),
                                                 change.value(SCENARIO_CHANGE_IS_DRAFT).toInt());
                    }
                }
            }
        }

        s_isInWorkSync = false;
    }
}

void SynchronizationManager::aboutFullSyncData()
{
    if (isCanSync()) {
        //
        // Запоминаем время синхронизации данных, в дальнейшем будем отправлять изменения
        // произведённые с данного момента
        //
        m_lastDataSyncDatetime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");

        //
        // Получить список всех изменений данных на сервере
        //
        NetworkRequest loader;
        loader.setRequestMethod(NetworkRequest::Post);
        loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
        loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
        QByteArray response = loader.loadSync(URL_SCENARIO_DATA_LIST);

        QXmlStreamReader changesReader(response);
        if (!isOperationSucceed(changesReader)) {
            return;
        }

        //
        // ... считываем изменения (uuid)
        //
        QList<QString> remoteChanges;
        changesReader.readNextStartElement();
        changesReader.readNextStartElement(); // changes
        while (!changesReader.atEnd()) {
            changesReader.readNextStartElement();
            if (changesReader.name() == "change") {
                const QString changeUuid = changesReader.attributes().value("id").toString();
                if (!changeUuid.isEmpty()) {
                    remoteChanges.append(changeUuid);
                }
            }
        }

        //
        // Сформируем список изменений сценария хранящихся локально
        //
        QList<QString> localChanges = StorageFacade::databaseHistoryStorage()->history(QString::null);

        //
        // Отправить на сайт все версии, которых на сайте нет
        //
        {
            QList<QString> changesForUpload;
            foreach (const QString& changeUuid, localChanges) {
                //
                // ... отправлять нужно, если такого изменения нет на сайте
                //
                const bool needUpload = !remoteChanges.contains(changeUuid);

                if (needUpload) {
                    changesForUpload.append(changeUuid);
                }
            }
            //
            // ... отправляем
            //
            uploadScenarioData(changesForUpload);
        }


        //
        // Сохранить в локальной БД все изменения, которых в ней нет
        //
        {
            QStringList changesForDownloadAndSave;
            foreach (const QString& changeUuid, remoteChanges) {
                //
                // ... сохранять нужно, если такого изменения нет в локальной БД
                //
                bool needSave = !localChanges.contains(changeUuid);

                if (needSave) {
                    changesForDownloadAndSave.append(changeUuid);
                }
            }
            //
            // ... скачиваем и сохраняем
            //
            downloadAndSaveScenarioData(changesForDownloadAndSave.join(";"));
        }
    }
}

void SynchronizationManager::aboutWorkSyncData()
{
    static bool s_inWorkSyncData = false;
    if (isCanSync() && !s_inWorkSyncData) {
        s_inWorkSyncData = true;

        //
        // Отправляем новые изменения
        //
        {
            //
            // Запоминаем время синхронизации изменений данных сценария
            //
            const QString prevDataSyncDatetime = m_lastDataSyncDatetime;
            m_lastDataSyncDatetime = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");

            //
            // Отправляем
            //
            QList<QString> newChanges =
                    StorageFacade::databaseHistoryStorage()->history(prevDataSyncDatetime);
            const bool dataUploaded = uploadScenarioData(newChanges);

            //
            // Не обновляем время последней синхронизации, если данные не были отправлены
            //
            if (dataUploaded == false) {
                m_lastDataSyncDatetime = prevDataSyncDatetime;
            }
        }

        //
        // Загружаем и применяем изменения от других пользователей за последние LAST_MINUTES минут
        //
        {
            const int LAST_MINUTES = 2;

            NetworkRequest loader;
            loader.setRequestMethod(NetworkRequest::Post);
            loader.clearRequestAttributes();
            loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
            loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
            loader.addRequestAttribute(KEY_FROM_LAST_MINUTES, LAST_MINUTES);
            QByteArray response = loader.loadSync(URL_SCENARIO_DATA_LIST);

            QXmlStreamReader changesReader(response);
            if (!isOperationSucceed(changesReader)) {
                s_inWorkSyncData = false;
                return;
            }

            //
            // ... считываем uuid'ы новых изменений
            //
            QList<QString> remoteChanges;
            while (!changesReader.atEnd()) {
                changesReader.readNextStartElement();
                if (changesReader.name() == "change") {
                    const QString changeUuid = changesReader.attributes().value("id").toString();
                    if (!changeUuid.isEmpty()) {
                        remoteChanges.append(changeUuid);
                    }
                }
            }

            //
            // ... скачиваем все изменения, которых ещё нет
            //
            QStringList changesForDownload;
            foreach (const QString& changeUuid, remoteChanges) {
                //
                // ... сохранять нужно, если такого изменения нет
                //
                const bool needDownload =
                        !DataStorageLayer::StorageFacade::databaseHistoryStorage()->contains(changeUuid);

                if (needDownload) {
                    changesForDownload.append(changeUuid);
                }
            }
            //
            // ... скачиваем и сохраняем
            //
            downloadAndSaveScenarioData(changesForDownload.join(";"));
        }

        s_inWorkSyncData = false;
    }
}

void SynchronizationManager::aboutUpdateCursors(int _cursorPosition, bool _isDraft)
{
    if (isCanSync()) {
        //
        // Загрузим позиции курсоров
        //
        NetworkRequest loader;
        loader.setRequestMethod(NetworkRequest::Post);
        loader.clearRequestAttributes();
        loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
        loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
        loader.addRequestAttribute(KEY_CURSOR_POSITION, _cursorPosition);
        loader.addRequestAttribute(KEY_SCENARIO_IS_DRAFT, _isDraft ? "1" : "0");
        QByteArray response = loader.loadSync(URL_SCENARIO_CURSORS);

        QXmlStreamReader cursorsReader(response);
        if (!isOperationSucceed(cursorsReader)) {
            return;
        }

        //
        // ... считываем данные о курсорах
        //
        QMap<QString, int> cleanCursors;
        QMap<QString, int> draftCursors;
        cursorsReader.readNextStartElement();
        while (!cursorsReader.atEnd()
               && cursorsReader.readNextStartElement()) {
            //
            // Курсоры
            //
            while (cursorsReader.name() == "cursors"
                   && cursorsReader.readNextStartElement()) {
                //
                // Считываем каждый курсор
                //
                while (cursorsReader.name() == "cursor") {
                    const QString username = cursorsReader.attributes().value("username").toString();
                    const int cursorPosition = cursorsReader.attributes().value("position").toInt();
                    const bool isDraft = cursorsReader.attributes().value("is_draft").toInt();

                    QMap<QString, int>& cursors = isDraft ? draftCursors : cleanCursors;
                    cursors.insert(username, cursorPosition);

                    //
                    // ... переход к следующему курсору
                    //
                    cursorsReader.readNextStartElement();
                    cursorsReader.readNextStartElement();
                }
            }
        }

        //
        // Уведомляем об обновлении курсоров
        //
        emit cursorsUpdated(cleanCursors);
        emit cursorsUpdated(draftCursors, IS_DRAFT);
    }
}

void SynchronizationManager::restartSession()
{
    //
    // Переавторизуемся
    //
    autoLogin();

    //
    // А если текущий проект - удаленный, то синхронизуем и его
    //
    if (ProjectsManager::currentProject().isRemote()) {
        aboutFullSyncScenario();
        aboutFullSyncData();
    }
}

bool SynchronizationManager::isOperationSucceed(QXmlStreamReader& _responseReader)
{
    while (!_responseReader.atEnd()) {
        _responseReader.readNext();
        if (_responseReader.name().toString() == "status") {
            //
            // Операция успешна
            //
            if (_responseReader.attributes().value("result").toString() == "true") {
                return true;
            } else {
                //
                // Попытаемся извлечь код ошибки
                //
                int errorCode = Sync::UnknownError;
                if (_responseReader.attributes().hasAttribute("errorCode")) {
                    errorCode = _responseReader.attributes().value("errorCode").toInt();
                }

                //
                // Попытаемся извлечь текст ошибки
                //
                QString errorText = Sync::errorText(errorCode);
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
    // Ничего не нашли про статус. Скорее всего пропал интернет
    //
    setInternetConnectionStatus(Inactive);
    handleError(Sync::NetworkError);

    return false;
}

void SynchronizationManager::handleError(int _code)
{
    handleError(Sync::errorText(_code), _code);
}

void SynchronizationManager::handleError(const QString &_error, int _code)
{
    switch (_code) {
        case Sync::NetworkError:
        case Sync::NoSessionKeyError:
        case Sync::SessionClosedError:
        case Sync::UnknownError: {
            m_sessionKey = ::INCORRECT_SESSION_KEY;
            emit cursorsUpdated(QMap<QString, int>());
            emit cursorsUpdated(QMap<QString, int>(), IS_DRAFT);
            break;
        }

        default: break;
    }

    emit syncClosedWithError(_code, _error);
}

bool SynchronizationManager::isCanSync() const
{
    return
            m_internetConnectionStatus
            && ProjectsManager::currentProject().isRemote()
            && ProjectsManager::currentProject().isSyncAvailable()
            && !m_sessionKey.isEmpty()
            && m_sessionKey != INCORRECT_SESSION_KEY;
}

bool SynchronizationManager::uploadScenarioChanges(const QList<QString>& _changesUuids)
{
    bool changesUploaded = false;

    if (isCanSync()
        && !_changesUuids.isEmpty()) {
        //
        // Сформировать xml для отправки
        //
        QString changesXml;
        QXmlStreamWriter xmlWriter(&changesXml);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("changes");
        foreach (const QString& changeUuid, _changesUuids) {
            const ScenarioChange change = StorageFacade::scenarioChangeStorage()->change(changeUuid);

            xmlWriter.writeStartElement("change");

            xmlWriter.writeTextElement(SCENARIO_CHANGE_ID, change.uuid().toString());

            xmlWriter.writeTextElement(SCENARIO_CHANGE_DATETIME, change.datetime().toString("yyyy-MM-dd hh:mm:ss"));

            xmlWriter.writeStartElement(SCENARIO_CHANGE_UNDO_PATCH);
            xmlWriter.writeCDATA(change.undoPatch());
            xmlWriter.writeEndElement();

            xmlWriter.writeStartElement(SCENARIO_CHANGE_REDO_PATCH);
            xmlWriter.writeCDATA(change.redoPatch());
            xmlWriter.writeEndElement();

            xmlWriter.writeTextElement(SCENARIO_CHANGE_IS_DRAFT, change.isDraft() ? "1" : "0");

            xmlWriter.writeEndElement(); // change
        }
        xmlWriter.writeEndElement(); // changes
        xmlWriter.writeEndDocument();

        //
        // Отправить данные
        //
        NetworkRequest loader;
        loader.setRequestMethod(NetworkRequest::Post);
        loader.clearRequestAttributes();
        loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
        loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
        loader.addRequestAttribute(KEY_CHANGES, changesXml);
        const QByteArray response = loader.loadSync(URL_SCENARIO_CHANGE_SAVE);

        //
        // Изменения отправлены, если сервер это подтвердил
        //
        changesUploaded = !response.isEmpty();
    }

    return changesUploaded;
}

QList<QHash<QString, QString> > SynchronizationManager::downloadScenarioChanges(const QString& _changesUuids)
{
    QList<QHash<QString, QString> > changes;

    if (isCanSync()
        && !_changesUuids.isEmpty()) {
        //
        // ... загружаем изменения
        //
        NetworkRequest loader;
        loader.setRequestMethod(NetworkRequest::Post);
        loader.clearRequestAttributes();
        loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
        loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
        loader.addRequestAttribute(KEY_CHANGES_IDS, _changesUuids);
        QByteArray response = loader.loadSync(URL_SCENARIO_CHANGE_LOAD);

        QXmlStreamReader changesReader(response);
        if (!isOperationSucceed(changesReader)) {
            return changes;
        }

        //
        // ... считываем данные об изменениях
        //
        changesReader.readNextStartElement();
        while (!changesReader.atEnd()
               && changesReader.readNextStartElement()) {
            //
            // Изменения
            //
            while (changesReader.name() == "changes"
                   && changesReader.readNextStartElement()) {
                //
                // Считываем каждое изменение
                //
                while (changesReader.name() == "change"
                       && changesReader.readNextStartElement()) {
                    //
                    // Данные изменения
                    //
                    QHash<QString, QString> change;
                    while (changesReader.name() != "change") {
                        const QString key = changesReader.name().toString();
                        const QString value = changesReader.readElementText();
                        if (!value.isEmpty()) {
                            change.insert(key, value);
                        }

                        //
                        // ... переходим к следующему элементу
                        //
                        changesReader.readNextStartElement();
                    }

                    if (!change.isEmpty()) {
                        changes.append(change);
                    }
                }
            }
        }
    }

    return changes;
}

bool SynchronizationManager::uploadScenarioData(const QList<QString>& _dataUuids)
{
    bool dataUploaded = false;

    if (isCanSync()
        && !_dataUuids.isEmpty()) {
        //
        // Сформировать xml для отправки
        //
        QString dataChangesXml;
        QXmlStreamWriter xmlWriter(&dataChangesXml);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("changes");
        int order = 0;
        foreach (const QString& dataUuid, _dataUuids) {
            const QMap<QString, QString> historyRecord =
                    StorageFacade::databaseHistoryStorage()->historyRecord(dataUuid);

            //
            // NOTE: Вынесено на уровень AbstractMapper::executeSql
            // Нас интересуют изменения из всех таблиц, кроме сценария и истории изменений сценария,
            // они синхронизируются самостоятельно
            //

            xmlWriter.writeStartElement("change");
            //
            xmlWriter.writeTextElement(DBH_ID_KEY, historyRecord.value(DBH_ID_KEY));
            //
            xmlWriter.writeStartElement(DBH_QUERY_KEY);
            xmlWriter.writeCDATA(historyRecord.value(DBH_QUERY_KEY));
            xmlWriter.writeEndElement();
            //
            xmlWriter.writeStartElement(DBH_QUERY_VALUES_KEY);
            xmlWriter.writeCDATA(historyRecord.value(DBH_QUERY_VALUES_KEY));
            xmlWriter.writeEndElement();
            //
            xmlWriter.writeTextElement(DBH_USERNAME_KEY, historyRecord.value(DBH_USERNAME_KEY));
            //
            xmlWriter.writeTextElement(DBH_DATETIME_KEY, historyRecord.value(DBH_DATETIME_KEY));
            //
            xmlWriter.writeTextElement(DBH_ORDER_KEY, QString::number(order++));
            //
            xmlWriter.writeEndElement(); // change
        }
        xmlWriter.writeEndElement(); // changes
        xmlWriter.writeEndDocument();

        //
        // Отправить данные
        //
        NetworkRequest loader;
        loader.setRequestMethod(NetworkRequest::Post);
        loader.clearRequestAttributes();
        loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
        loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
        loader.addRequestAttribute(KEY_CHANGES, dataChangesXml);
        const QByteArray response = loader.loadSync(URL_SCENARIO_DATA_SAVE);

        //
        // Данные отправлены, если сервер это подтвердил
        //
        dataUploaded = !response.isEmpty();
    }

    return dataUploaded;
}

void SynchronizationManager::downloadAndSaveScenarioData(const QString& _dataUuids)
{
    if (isCanSync()
        && !_dataUuids.isEmpty()) {
        //
        // ... загружаем изменения
        //
        NetworkRequest loader;
        loader.setRequestMethod(NetworkRequest::Post);
        loader.clearRequestAttributes();
        loader.addRequestAttribute(KEY_SESSION_KEY, m_sessionKey);
        loader.addRequestAttribute(KEY_PROJECT, ProjectsManager::currentProject().id());
        loader.addRequestAttribute(KEY_CHANGES_IDS, _dataUuids);
        QByteArray response = loader.loadSync(URL_SCENARIO_DATA_LOAD);

        QXmlStreamReader changesReader(response);
        if (!isOperationSucceed(changesReader)) {
            return;
        }

        //
        // ... считываем данные об изменениях
        //
        QList<QHash<QString, QString>> changes;
        changesReader.readNextStartElement();
        while (!changesReader.atEnd()
               && changesReader.readNextStartElement()) {
            //
            // Изменения
            //
            while (changesReader.name() == "changes"
                   && changesReader.readNextStartElement()) {
                //
                // Считываем каждое изменение
                //
                while (changesReader.name() == "change"
                       && changesReader.readNextStartElement()) {
                    //
                    // Данные изменения
                    //
                    QHash<QString, QString> change;
                    while (changesReader.name() != "change") {
                        const QString key = changesReader.name().toString();
                        const QString value = changesReader.readElementText();
                        if (!value.isEmpty()) {
                            change.insert(key, value);
                        }

                        //
                        // ... переходим к следующему элементу
                        //
                        changesReader.readNextStartElement();
                    }

                    if (!change.isEmpty()) {
                        changes.append(change);
                    }
                }
            }
        }


        //
        // Применяем изменения
        //
        QHash<QString, QString> changeValues;
        DatabaseLayer::Database::transaction();
        foreach (changeValues, changes) {
            DataStorageLayer::StorageFacade::databaseHistoryStorage()->storeAndApplyHistoryRecord(
                changeValues.value(DBH_ID_KEY), changeValues.value(DBH_QUERY_KEY),
                changeValues.value(DBH_QUERY_VALUES_KEY), changeValues.value(DBH_USERNAME_KEY), changeValues.value(DBH_DATETIME_KEY));
        }
        DatabaseLayer::Database::commit();


        //
        // Обновляем данные
        //
        DataStorageLayer::StorageFacade::refreshStorages();
    }
}

void SynchronizationManager::checkNetworkState()
{
    //
    // Если пользователь не авторизовался, незачем проверять статус интернета
    //
    if (m_sessionKey.isEmpty()) {
        return;
    }

    //
    // Защитимся от множественных вызовов
    //
    static bool s_isInCheckNetworkState = false;
    if (s_isInCheckNetworkState) {
        return;
    }

#ifdef Q_OS_MAC
    //
    // FIXME: Если есть открытый диалог сохранения, или открытия, то он закрывается
    // при загрузке страницы, поэтому делаем отсрочку на выполнение события
    //
    if (QApplication::activeModalWidget() != 0) {
        QTimer::singleShot(1000, this, &SynchronizationManager::checkNetworkState);
        return;
    }
#endif

    s_isInCheckNetworkState = true;

    //
    // Делаем три попытки запроса тестовую страницу
    //
    NetworkRequest loader;
    loader.setLoadingTimeout(2000);
    int leavedTries = 3;
    InternetStatus newState;
    while (leavedTries-- > 0) {
        QByteArray response = loader.loadSync(URL_CHECK_NETWORK_STATE);

        //
        // Запомним состояние интернета и кинем соответствующий сигнал
        //
        if (response == "ok") {
            newState = Active;
            break;
        } else {
            newState = Inactive;
        }
    }

    setInternetConnectionStatus(newState);

    //
    // Если интернет активен, запрашиваем каждые 5 секунд
    // Неактивен - каждую секунду
    //
    const int checkTimeout = m_internetConnectionStatus == Active ? 5000 : 1000;
    QTimer::singleShot(checkTimeout, this, &SynchronizationManager::checkNetworkState);

    s_isInCheckNetworkState = false;
}

void SynchronizationManager::setInternetConnectionStatus(SynchronizationManager::InternetStatus _newStatus)
{
    if (m_internetConnectionStatus != _newStatus) {
        m_internetConnectionStatus = _newStatus;
        if (m_internetConnectionStatus == Active) {
            restartSession();
        }
        emit networkStatusChanged(m_internetConnectionStatus);
    }
}

void SynchronizationManager::initConnections()
{
    connect(this, &SynchronizationManager::loginAccepted, this, &SynchronizationManager::loadProjects);
}
