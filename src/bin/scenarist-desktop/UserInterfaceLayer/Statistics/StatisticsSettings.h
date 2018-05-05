#ifndef STATISTICSSETTINGS_H
#define STATISTICSSETTINGS_H

#include <BusinessLayer/Statistics/Reports/AbstractReport.h>

#include <QWidget>

class QAbstractItemModel;
class QSortFilterProxyModel;

namespace Ui {
    class StatisticsSettings;
}


namespace UserInterface
{
    /**
     * @brief Класс виджета с настройками отчётов/графиков
     */
    class StatisticsSettings : public QWidget
    {
        Q_OBJECT

    public:
        explicit StatisticsSettings(QWidget* parent = nullptr);
        ~StatisticsSettings();

        /**
         * @brief Установить заголовок
         */
        void setTitle(const QString& _title);

        /**
         * @brief Отобразить заданные параметры
         */
        void setCurrentType(int _index);

        /**
         * @brief Задать персонажей
         */
        void setCharacters(QAbstractItemModel* _characters);

        /**
         * @brief Получить параметры отчётов
         */
        const BusinessLogic::StatisticsParameters& settings() const;

    signals:
        /**
         * @brief Пользователь нажал кнопку выйти из настроек инструментов
         */
        void backPressed();

        /**
         * @brief Изменились настройки
         */
        void settingsChanged();

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

    private:
        /**
         * @brief Интерфейс
         */
        Ui::StatisticsSettings* m_ui = nullptr;

        /**
         * @brief Отсортированная модель персонажей
         */
        QSortFilterProxyModel* m_charactersModel = nullptr;

        /**
         * @brief Параметры отчёта
         */
        mutable BusinessLogic::StatisticsParameters m_settings;
    };
}

#endif // STATISTICSSETTINGS_H
