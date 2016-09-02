#ifndef SYNCHRONIZATIONMANAGERV2_H
#define SYNCHRONIZATIONMANAGERV2_H

#include <QObject>

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
        void registration(const QString& _email, const QString& _password,
                          const QString& _type);

        /**
         * @brief Подтверждение регистрации при помощи проверочного кода
         */
        void verifyRegistration(const QString& _code);

        /**
         * @brief Восстановление пароля
         */
        void recoveryPassword(const QString& _email);

    signals:
        /**
         * @brief Авторизация пройдена успешно
         */
        void loginAccepted();

        /**
         * @brief Сервер успешно принял данные пользователя на регистрацию
         */
        void registered();

        /**
         * @brief Сервер подтвердил регистрацию
         */
        void registerVerified();

        /**
         * @brief Пароль отправлен на email
         */
        void passwordRecoveried();

        /**
         * @brief Ошибка
         */
        void syncClosedWithError(int, QString);

    private:
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
    };
}

#endif // SYNCHRONIZATIONMANAGERV2_H
