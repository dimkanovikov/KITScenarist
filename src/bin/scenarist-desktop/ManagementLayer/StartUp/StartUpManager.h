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
}

class QAbstractItemModel;
class QMenu;

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

        /**
         * @brief Проверить наличие отчётов об ошибке
         */
        void checkCrashReports();

        /**
         * @brief Проверить наличие новой версии
         */
        void checkNewVersion();

        /**
         * @brief Открыт ли в данный момент список локальных проектов (true), или облачных (false)
         */
        bool isOnLocalProjectsTab() const;

        /**
         * @brief Установить список недавно используемых проектов
         */
        void setRecentProjects(QAbstractItemModel* _model);

        /**
         * @brief Установить имя проекта из списка недавних
         */
        void setRecentProjectName(int _index, const QString& _name);

        /**
         * @brief Установить видимость списка облачных проектов
         */
        void setRemoteProjectsVisible(bool _visible);

        /**
         * @brief Установить список проектов из облака
         */
        void setRemoteProjects(QAbstractItemModel* _model);

        /**
         * @brief Установить имя проекта из списка облачных
         */
        void setRemoteProjectName(int _index, const QString& _name);

#ifdef Q_OS_MAC
        /**
         * @brief Сформировать меню "Правка" с фиктивным содержанием
         */
        void buildEditMenu(QMenu* _menu);
#endif

    signals:
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
         * @brief Требуется перенести проект в облако
         */
        void moveToCloudRecentProjectRequested(const QModelIndex& _projectIndex);

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

        /**
         * @brief Прогресс загрузки установочного файла новой версии
         */
        void downloadProgressForUpdate(int);

        /**
         * @brief Завершилась загрузка установочного файла новой версии
         */
        void downloadFinishedForUpdate();

        /**
         * @brief Не удалось загрузить файл с обновлением
         */
        void errorDownloadForUpdate();

        /**
         * @brief Прерываем загрузку установочного файла новой версии
         */
        void stopDownloadForUpdate();

    private slots:
        /**
         * @brief Покажем окно с информацией об обновлении
         */
        void showUpdateDialog();

        /**
         * @brief Загрузить файл с обновлением
         */
        void downloadUpdate(const QString& _fileTemplate);

    private:
        /**
         * @brief Настроить соединения
         */
        void initConnections();

    private:
        /**
         * @brief Представление для стартовой страницы
         */
        UserInterface::StartUpView* m_view;

        /**
         * @brief Путь до файла с обновлениями
         */
        QString m_updateFile;

        /**
         * @brief Версия обновления
         */
        QString m_updateVersion;

        /**
         * @brief Описание обновления
         */
        QString m_updateDescription;

        /**
         * @brief Шаблон файла обновления для скачивания
         */
        QString m_updateFileTemplate;

        /**
         * @brief Является ли обновление бета
         */
        bool m_updateIsBeta;

    };
}

#endif // STARTUPMANAGER_H
