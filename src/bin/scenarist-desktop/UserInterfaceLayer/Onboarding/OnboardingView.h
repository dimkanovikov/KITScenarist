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

        /**
         * @brief Нуобходимо ли использвоать тёмную тему (true) или светлую (false)
         */
        bool isUseDarkTheme() const;

        /**
         * @brief Использовать ли модуль разработки
         */
        bool isUseResearch() const;

        /**
         * @brief Использовать ли модуль карточек
         */
        bool isUseCards() const;

        /**
         * @brief Использовать ли модуль сценария
         */
        bool isUseScript() const;

        /**
         * @brief Выбранный пользователем шаблон сценария
         */
        QString scriptTemplate() const;

        /**
         * @brief Использовать ли модуль статистики
         */
        bool isUseStatistics() const;

    signals:
        /**
         * @brief Пользователь выбрал язык
         */
        void languageChanged(int _language);

        /**
         * @brief Пользователь включил/выключил использование тёмной темы
         */
        void useDarkThemeToggled(bool _checked);

        /**
         * @brief Была нажата кнопка пропуска первоначальной настройки
         */
        void skipClicked();

        /**
         * @brief Нажата кнопка завершения
         */
        void finishClicked();

    protected:
        /**
         * @brief Переопределяем для перезагрузки перевода
         */
        void changeEvent(QEvent* _event);

    private:
        /**
         * @brief Настроить внешний вид
         */
        void initView();

        /**
         * @brief Настроить соединения
         */
        void initConnections();

        /**
         * @brief Уведомить о выборе языка
         */
        void notifyLanguageChange();

        /**
         * @brief Перейти к предыдущей странице
         */
        void goToPreviousPage();

        /**
         * @brief Перейти к следующей странице
         */
        void goToNextPage();

        /**
         * @brief Показать последнюю страницу
         */
        void showFinalPage(bool _isSkipped);

    private:
        /**
         * @brief Интерфейс представления
         */
        Ui::OnboardingView* m_ui = nullptr;
    };
}

#endif // ONBOARDINGVIEW_H
