#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <QObject>

class QMenu;

namespace UserInterface {
    class ChangePasswordDialog;
    class LoginDialog;
    class MenuView;
    class RenewSubscriptionDialog;
}


namespace ManagementLayer
{
    /**
     * @brief Управляющий меню и личным кабинетом пользователя
     */
    class MenuManager : public QObject
    {
        Q_OBJECT

    public:
        explicit MenuManager(QObject* _parent, QWidget* _parentWidget);

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
         * @brief Показать меню
         */
        void showMenu();

        // Работа с личным кабинетом

        /**
         * @brief Для процесса автологина меняет строку "Авторизация" на "Подключение"
         */
        void setProgressLoginLabel(bool _enable);

        /**
         * @brief Отобразить диалог авторизации с заданными имейлом и сообщением об ошибке
         */
        void showLoginDialog(const QString& _email, const QString& _message);

        /**
         * @brief Пользователь с заданным именем успешно авторизован на сервере
         */
        void completeLogin(const QString& _userName, const QString& _userEmail,
                           int _paymentMonth);

        /**
         * @brief Имейл пользователя
         */
        QString userEmail() const;

        /**
         * @brief Установить информацию о подписке
         */
        void setSubscriptionInfo(bool _isActive, const QString& _expiredDate, quint64 _usedSpace, quint64 _availableSpace);

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

        /**
         * @brief Открыт ли в данный момент диалог авторизации/регистрации или смены пароля
         */
        bool isOnLoginDialog() const;

        /**
         * @brief Показать информацию о том, что доступна новая версия
         */
        void showUpdateButton(const QString& _newVersion);

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
         * @brief Пользователь хочет обновиться
         */
        void updateRequested();

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить соединения
         */
        void initConnections();

    private:
        /**
         * @brief Представление меню
         */
        UserInterface::MenuView* m_view = nullptr;

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
        UserInterface::LoginDialog* m_loginDialog = nullptr;

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

#endif // MENUMANAGER_H
