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

        /**
         * @brief Закрыть окно предварительной настройки
         */
        void close();

    signals:
        /**
         * @brief Пользователь сменил язык
         */
        void translationUpdateRequested();

        /**
         * @brief Первоначальная настройка завершена
         */
        void finished();

    private:
        /**
         * @brief Настроить соединения
         */
        void initConnections();

        /**
         * @brief Обновить используемые шаблоны, удаляем старый default.kitss
         */
        void updateUsedTemplates();

        /**
         * @brief Установить язык приложения
         */
        void setLanguage(int _language);

        /**
         * @brief Включить/выключить использование тёмной темы
         */
        void setUseDarkTheme(bool _use);

        /**
         * @brief Пропустить первоначальную настройку и использовать парамеры по умолчанию
         */
        void skip();

        /**
         * @brief Сохранить все настройки выбранные пользователем и перейти к приложению
         */
        void finalize();

    private:
        /**
         * @brief Представление диалога стартовой настройки приложения
         */
        UserInterface::OnboardingView* m_view = nullptr;
    };
}

#endif // ONBOARDINGMANAGER_H
