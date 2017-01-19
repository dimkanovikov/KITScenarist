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

#ifndef STARTUPMANAGER_H
#define STARTUPMANAGER_H

#include <QObject>

#include <QMap>

namespace UserInterface {
    class StartUpView;
    class LoginDialog;
    class ChangePasswordDialog;
    class RenewSubscriptionDialog;
}

class QAbstractItemModel;
class QNetworkReply;

namespace ManagementLayer
{

    /**
     * @brief Управляющий стартовой страницей
     */
    class StartUpManager : public QObject
    {
        Q_OBJECT

    public:
        explicit StartUpManager(QObject* _parent, QWidget* _parentWidget);

        QWidget* view() const;

    public slots:
        /**
         * @brief Открыт ли в данный момент диалог авторизации/регистрации
         */
        bool isOnLoginDialog() const;

        /**
         * @brief Открыт ли в данный момент список локальных проектов (true), или облачных (false)
         */
        bool isOnLocalProjectsTab() const;

        /**
         * @brief Пользователь с заданным именем успешно авторизован на сервере
         */
        void completeLogin(const QString& _userName, const QString& _userEmail);

        /**
         * @brief Пользователь успешно отправил данные для регистрации
         */
        void verifyUser();

        /**
         * @brief Пользователь успешно ввел проверочный код
         *        и окончательно зарегистрировался
         */
        void userAfterSignUp();

        /**
         * @brief Пользователю отправлен пароль на email
         */
        void userPassRestored();

        /**
         * @brief Пользователь закрыл авторизацию
         */
        void completeLogout();

        /**
         * @brief Пароль успешно сменен
         */
        void passwordChanged();

        /**
         * @brief passwordNotChanged
         */
        void showPasswordError(const QString& _error);

        /**
         * @brief Получена информация о подписке
         */
        void setSubscriptionInfo(bool _isActive, const QString& _expiredDate);

        /**
         * @brief Установить список недавно используемых проектов
         */
        void setRecentProjects(QAbstractItemModel* _model);

        /**
         * @brief Установить список проектов из облака
         */
        void setRemoteProjects(QAbstractItemModel* _model);

        /**
         * @brief Попробовать повторно авторизоваться, после неудачной попытки
         */
        void retryLogin(const QString& _error);

        /**
         * @brief Попробовать повторно зарегистрироваться
         */
        void retrySignUp(const QString& _error);

        /**
         * @brief Попробовать повторно ввести проверочный код
         */
        void retryVerify(const QString& _error);

        /**
         * @brief Возникла неизвестная ошибка. Покажем на последнем окне
         */
        void retryLastAction(const QString& _error);

    signals:
        /**
         * @brief Пользователь хочет авторизоваться
         */
        void loginRequested(const QString& _email, const QString& _password);

        /**
         * @brief Пользователь хочет зарегистрироваться
         */
        void signUpRequested(const QString& _email, const QString& _password);

        /**
         * @brief Пользователь хочет отправить проверочный код
         */
        void verifyRequested(const QString& _code);

        /**
         * @brief Пользователь хочет восстановить пароль
         */
        void restoreRequested(const QString& _email);

        /**
         * @brief Пользователь хочет сменить имя
         */
        void userNameChangeRequested(const QString& _userName);

        /**
         * @brief Пользователь хочет запросить информацию о подписке
         */
        void getSubscriptionInfoRequested();

        /**
         * @brief Пользователь хочет продлить подписку
         */
        void renewSubscriptionRequested(unsigned _duration, unsigned _type);

        /**
         * @brief Пользователь хочет сменить пароль
         */
        void passwordChangeRequested(const QString& _password,
                                     const QString& _newPassword);

        /**
         * @brief Пользователь хочет выйти
         */
        void logoutRequested();

        /**
         * @brief Создать проект
         */
        void createProjectRequested();

        /**
         * @brief Открыть проект
         */
        void openProjectRequested();

        /**
         * @brief Помощь
         */
        void helpRequested();

        /**
         * @brief Обновить список проектов из облака
         */
        void refreshProjectsRequested();

        /**
         * @brief Выбран один из недавних файлов для открытия
         */
        void openRecentProjectRequested(const QModelIndex& _recentProjectIndex);

        /**
         * @brief Требуется скрыть один из недавно используемых проектов
         */
        void hideRecentProjectRequested(const QModelIndex& _projectIndex);

        /**
         * @brief Выбран один из проектов из облака для открытия
         */
        void openRemoteProjectRequested(const QModelIndex& _remoteProjectIndex);

        /**
         * @brief Требуется изменить название проекта из облака
         */
        void editRemoteProjectRequested(const QModelIndex& _remoteProjectIndex);

        /**
         * @brief Пользователь хочет удалить проект из облака
         */
        void removeRemoteProjectRequested(const QModelIndex& _remoteProjectIndex);

        /**
         * @brief Пользователь хочет открыть доступ к проекту из облака
         */
        void shareRemoteProjectRequested(const QModelIndex& _remoteProjectIndex);

        /**
         * @brief Пользователь хочет закрыть доступ к проекту из облака
         */
        void unshareRemoteProjectRequested(const QModelIndex& _remoteProjectIndex, const QString& _userEmail);

    private slots:

        /**
         * @brief Загрузилась страница с информацией об обновлениях
         */
        void aboutLoadUpdatesInfo(QNetworkReply* _reply);

    private:
        /**
         * @brief Настроить данные
         */
        void initData();

        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить соединения
         */
        void initConnections();

        /**
         * @brief Проверить наличие отчётов об ошибке
         */
        void checkCrashReports();

        /**
         * @brief Проверить наличие новой версии
         */
        void checkNewVersion();

    private:
        /**
         * @brief Представление для стартовой страницы
         */
        UserInterface::StartUpView* m_view;

        /**
         * @brief Логин введённый при авторизации
         */
        QString m_userEmail;

        /**
         * @brief Дата окончания подпискиы
         */
        QString m_subscriptionEndDate;

        /**
         * @brief Окно авторизации/регистрации
         */
        UserInterface::LoginDialog* m_loginDialog;

        /**
         * @brief Окно смены пароля
         */
        UserInterface::ChangePasswordDialog* m_changePasswordDialog;

        /**
         * @brief Окно продления подписки
         */
        UserInterface::RenewSubscriptionDialog* m_renewSubscriptionDialog;


    };
}

#endif // STARTUPMANAGER_H
