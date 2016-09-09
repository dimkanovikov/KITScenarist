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
		 * @brief Пользователь с заданным именем успешно авторизован на сервере
		 */
        void userLogged();

        /**
         * @brief Пользователь успешно отправил данные для регистрации
         */
        void userSignUp();

        /**
         * @brief Пользователь успешно ввел проверочный код
         *        и окончательно зарегистрировался
         */
        void userVerified();

        /**
         * @brief Пользователю отправлен пароль на email
         */
        void userPassRestored();

		/**
		 * @brief Пользователь закрыл авторизацию
		 */
		void aboutUserUnlogged();

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

	signals:
		/**
		 * @brief Пользователь хочет авторизоваться
		 */
        void loginRequested(const QString& _email, const QString& _password);

        /**
         * @brief Пользователь хочет зарегистрироваться
         */
        void signUpRequested(const QString& _email, const QString& _password,
                               const QString& _type);

        /**
         * @brief Пользователь хочет отправить проверочный код
         */
        void verifyRequested(const QString& _code);

        /**
         * @brief Пользователь хочет восстановить пароль
         */
        void restoreRequested(const QString& _email);

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
		 * @brief Выбран один из проектов из облака для открытия
		 */
		void openRemoteProjectRequested(const QModelIndex& _remoteProjectIndex);

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
		 * @brief Настроить соединения
		 */
		void initConnections();

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
		QString m_userName;

		/**
		 * @brief Пароль введённый при авторизации
		 */
		QString m_password;

        /**
         * @brief Окно авторизации/регистрации
         */
        UserInterface::LoginDialog* m_loginDialog;
	};
}

#endif // STARTUPMANAGER_H
