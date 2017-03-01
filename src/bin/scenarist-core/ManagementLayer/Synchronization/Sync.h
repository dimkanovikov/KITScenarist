#ifndef SYNCHRONIZATIONERRORCODES_H
#define SYNCHRONIZATIONERRORCODES_H

#include <QApplication>


namespace ManagementLayer
{
    class Sync
    {
    public:
        /**
         * @brief Список ошибок синхронизации
         */
        enum SynchronizationErrorCode {
            // ****
            //
            // Нет связи с интернетом
            //
            NetworkError = 0,

            // ****
            //
            // Проблемы с вводом логина
            //
            IncorrectLoginError = 100,
            //
            // Проблемы с вводом пароля
            //
            IncorrectPasswordError = 101,
            //
            // Закончилась подписка
            //
            SubscriptionEndedError = 102,
            //
            // Не задан ключ сессии
            // NOTE: Такая проблема может возникать при проблемах с провайдером,
            //		 когда данные портятся на каком-либо из узлов связи
            //
            NoSessionKeyError = 103,
            //
            // Сессия закрыта
            //
            SessionClosedError = 104,

            // ****
            // Попытка открыть доступ самому себе
            //
            DisallowToShareSelf = 130,

            // ****
            //
            // Проект недоступен (у хостера закончилась подписка)
            //
            ProjectUnavailableError = 201,
            //
            // Доступ к проекту закрыт
            //
            AccessToProjectClosed = 202,
            //
            // Закончилось место на сервере
            //
            StorageSizeFinished = 203,

            // ****
            //
            // Такой email уже зарегистрирован
            //
            EmailAlreadyRegisteredError = 404,
            //
            // Слишком слабый пароль
            //
            WeakPasswordError = 405,
            //
            // Старый пароль некорректен
            //
            IncorrectOldPasswordError = 406,
            //
            // Неверный код валидации
            //
            IncorrectValidationCodeError = 505,
            //
            // Неверный email для восстановления пароля
            //
            EmailNotRegisteredError = 606,

            // ****
            //
            // Неизвестная ошибка
            //
            UnknownError = 1000
        };

        /**
         * @brief Получить текст ошибки по-умолчанию
         */
        /** @{ */
        static QString errorText(SynchronizationErrorCode _code) {
            switch (_code) {
                case NetworkError: {
                    return QApplication::translate("ManagementLayer::Sync", "Can't estabilish network connection.");
                }

                case NoSessionKeyError: {
                    return QApplication::translate("ManagementLayer::Sync", "Session key not found.");
                }

                case IncorrectValidationCodeError: {
                    return QApplication::translate("ManagementLayer::Sync", "Wrong validation code.");
                }

                case UnknownError: {
                    return QApplication::translate("ManagementLayer::Sync", "Got wrong response from server.");
                }

                default: break;
            }

            return QString::null;
        }
        static QString errorText(int _code) { return errorText((SynchronizationErrorCode)_code); }
        /** @} */
    };
}

#endif // SYNCHRONIZATIONERRORCODES_H
