#ifndef ONBOARDINGMANAGER_H
#define ONBOARDINGMANAGER_H

#include <QObject>

namespace UserInterface {
    class OnboardingView;
}


namespace ManagementLayer
{
    /**
     * @brief Управляющий страницей первоначальной настройки приложения
     */
    class OnboardingManager : public QObject
    {
        Q_OBJECT

    public:
        explicit OnboardingManager(QObject* _parent = nullptr);
        ~OnboardingManager();

        /**
         * @brief Было ли сконфигурировано приложение (false) или не сконфигурировано (true)
         */
        bool needConfigureApp() const;

        /**
         * @brief Если настройка ещё ни разу не была выполнена, запустить соответствующий диалог
         */
        void exec();

    signals:
        /**
         * @brief Первоначальная настройка завершена
         */
        void finished();

    private:
        /**
         * @brief Настроить соединения
         */
        void initConnections();

    private:
        /**
         * @brief Представление диалога стартовой настройки приложения
         */
        UserInterface::OnboardingView* m_view = nullptr;
    };
}

#endif // ONBOARDINGMANAGER_H
