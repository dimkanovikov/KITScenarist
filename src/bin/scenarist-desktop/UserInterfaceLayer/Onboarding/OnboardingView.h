#ifndef ONBOARDINGVIEW_H
#define ONBOARDINGVIEW_H

#include <QWidget>

namespace Ui {
    class OnboardingView;
}


namespace UserInterface
{
    /**
     * @brief Представление диалога первоначальной настройки приложения
     */
    class OnboardingView : public QWidget
    {
        Q_OBJECT

    public:
        explicit OnboardingView(QWidget *parent = 0);
        ~OnboardingView();

    signals:
        /**
         * @brief Нажата кнопка завершения
         */
        void finishClicked();

    private:
        /**
         * @brief Настроить соединения
         */
        void initConnections();

    private:
        /**
         * @brief Интерфейс представления
         */
        Ui::OnboardingView* m_ui = nullptr;
    };
}

#endif // ONBOARDINGVIEW_H
