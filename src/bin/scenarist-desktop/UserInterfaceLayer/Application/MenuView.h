#ifndef MENUVIEW_H
#define MENUVIEW_H

#include <QPushButton>

class QLabel;
class QMenu;

class MenuButton : public QPushButton {
public:
    explicit MenuButton(QWidget* _parent = nullptr);

    /**
     * @brief Установить текст с комбинацией клавиш активирующих кнопку
     */
    void setShortcutText(const QString& _text);

private:
    /**
     * @brief Текст с информацией о горячих клавишах кнопки
     */
    QLabel* m_shortcutText = nullptr;
};

namespace Ui {
    class MenuView;
}


namespace UserInterface
{
    /**
     * @brief Меню приложения
     */
    class MenuView : public QWidget
    {
        Q_OBJECT

    public:
        explicit MenuView(QWidget* _parent = nullptr);
        ~MenuView();

        /**
         * @brief Установить меню для оборачивания
         */
        void setMenu(QMenu* _menu);

        /**
         * @brief Получить обёрнутое меню
         */
        QMenu* menu() const;

        /**
         * @brief Настроить доступность заданного пункта меню
         */
        void setMenuItemEnabled(int _index, bool _enabled);

        /**
         * @brief Деактивировать доступность действий над проектом
         */
        void disableProjectActions();

        /**
         * @brief Активировать доступность действий над проектом
         */
        void enableProjectActions();

        /**
         * @brief Установим текст о прогресса авторизации
         */
        void enableProgressLoginLabel(int _dots, bool _firstUpdate = false);

        /**
         * @brief Отключить текст о прогрессе авторизации
         */
        void disableProgressLoginLabel();

        /**
         * @brief Задать авторизован ли пользователь
         */
        void setUserLogged(bool _isLogged, const QString& _userName = QString(), const QString& _email = QString());

        /**
         * @brief Установить информацию о подписке
         */
        void setSubscriptionInfo(bool _isActive, const QString& _expDate, quint64 _usedSpace, quint64 _availableSpace);

        /**
         * @brief Отобразить страницу с кнопками меню
         */
        void showMenuPage(bool _animate = true);

        /**
         * @brief Отобразить страницу с личным кабинетом
         */
        void showAccountPage();

    signals:
        /**
         * @brief Пользователь хочет авторизоваться
         */
        void loginPressed();

        /**
         * @brief Пользователь хочет открыть личный кабинет
         */
        void accountPressed();

        /**
         * @brief Пользователь решил сменить свое имя
         */
        void userNameChanged(const QString& _newUserName);

        /**
         * @brief Пользователь нажал кнопку смены пароля
         */
        void passwordChangeClicked();

        /**
         * @brief Пользователь нажал кнопку обновления информации о подписке
         */
        void getSubscriptionInfoClicked();

        /**
         * @brief Пользователь нажал кнопку продления подписки
         */
        void renewSubscriptionClicked();

        /**
         * @brief Нажата кнопка выйти
         */
        void logoutClicked();

        /**
         * @brief Пользователь хочет посмотреть информацию о приложении
         */
        void aboutAppPressed();

        /**
         * @brief Запрос на скрытие меню
         */
        void hideRequested();

    protected:
        /**
         * @brief Переопределяем для обработки события смены палитры
         */
        bool event(QEvent* _event) override;

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить кнопки меню
         */
        void initMenuButtons();

        /**
         * @brief Настроить соединения
         */
        void initConnections();

        /**
         * @brief Настроить стили
         */
        void initStyleSheet();

    private:
        /**
         * @brief Интерфейс
         */
        Ui::MenuView* m_ui = nullptr;

        /**
         * @brief Меню, которое нужно обернуть
         */
        QMenu* m_menu = nullptr;

        /**
         * @brief Список кнопок соответствующий пунктам меню
         */
        QVector<MenuButton*> m_menuButtons;

        /**
         * @brief Находимся ли в данный момент в процессе авторизации
         */
        bool m_isProcessLogin = false;
    };
}

#endif // MENUVIEW_H
