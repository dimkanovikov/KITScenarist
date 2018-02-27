#ifndef STARTUPVIEW_H
#define STARTUPVIEW_H

#include <QWidget>

class QAbstractItemModel;

namespace Ui {
    class StartUpView;
}


namespace UserInterface
{
    /**
     * @brief Класс формы приветствия программы
     */
    class StartUpView : public QWidget
    {
        Q_OBJECT

    public:
        explicit StartUpView(QWidget *parent = 0);
        ~StartUpView();

        /**
         * @brief Открыт ли в данный момент список локальных проектов (true), или облачных (false)
         */
        bool isOnLocalProjectsTab() const;

        /**
         * @brief Установить недавно использованные файлы
         */
        void setRecentProjects(QAbstractItemModel* _recentProjectsModel);

        /**
         * @brief Установить имя проекта из списка недавних
         */
        void setRecentProjectName(int _index, const QString& _name);

        /**
         * @brief Обновить информацию о доступности обновлений
         */
        void setUpdateInfo(const QString& _updateInfo);

        /**
         * @brief Установить видимость списка облачных проектов
         */
        void setRemoteProjectsVisible(bool _visible);

        /**
         * @brief Установить список доступных проектов
         */
        void setRemoteProjects(QAbstractItemModel* _remoteProjectsModel);

        /**
         * @brief Установить имя проекта из списка облачных
         */
        void setRemoteProjectName(int _index, const QString& _name);

    signals:
        /**
         * @brief Нажата кнопка создать проект
         */
        void createProjectClicked();

        /**
         * @brief Нажата кнопка открыть проект
         */
        void openProjectClicked();

        /**
         * @brief Нажата кнопка помощь
         */
        void helpClicked();

        /**
         * @brief Выбран один из недавно используемых проектов для открытия
         */
        void openRecentProjectClicked(const QModelIndex& _projectIndex);

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
        void openRemoteProjectClicked(const QModelIndex& _projectIndex);

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
         * @brief Нажата кнопка обновления недавних файлов
         */
        void refreshProjects();

        /**
         * @brief Пользователь хочет обновиться, нажав на кнопку обновления
         */
        void updateRequested();

    protected:
        /**
         * @brief Переопределяем для обновления цвета иконок, при смене палитры
         */
        bool event(QEvent* _event);

    private slots:
        /**
         * @brief Пользователь сменил источник отображаемых проектов
         */
        void aboutFilesSourceChanged();

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить соединения для формы
         */
        void initConnections();

        /**
         * @brief Настроить внешний вид
         */
        void initStyleSheet();

        /**
         * @brief Настроить цвет иконок
         */
        void initIconsColor();

    private:
        /**
         * @brief Интерфейс
         */
        Ui::StartUpView* m_ui = nullptr;
    };
}

#endif // STARTUPVIEW_H
