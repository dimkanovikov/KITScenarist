#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

#include <QWidget>

class FlatButton;
class QAbstractItemModel;
class QCustomPlotExtended;
class QLabel;
class QLightBoxProgress;
class QPrinter;
class QStackedWidget;
class QTextBrowser;
class QTreeWidget;
class QTreeWidgetItem;

namespace BusinessLogic {
    class StatisticsParameters;
    class Plot;
}

namespace UserInterface
{
    class StatisticsSettings;
    class ReportButton;


    /**
     * @brief Класс формы со статистикой по сценарию
     */
    class StatisticsView : public QWidget
    {
        Q_OBJECT

    public:
        explicit StatisticsView(QWidget* _parent = 0);

        /**
         * @brief Задать персонажей
         */
        void setCharacters(QAbstractItemModel* _characters);

        /**
         * @brief Установить отчёт
         */
        void setReport(const QString& _html);

        /**
         * @brief Установить данные графиков
         */
        void setPlot(const BusinessLogic::Plot& _plot);

        /**
         * @brief Функции управленя индикатором информирования пользователя о подготовке отчёта
         */
        /** @{ */
        void showProgress();
        void hideProgress();
        /** @} */

    signals:
        /**
         * @brief Необходимо сформировать отчёт по заданным параметрам
         */
        void makeReportRequested(const BusinessLogic::StatisticsParameters& _parameters);

        /**
         * @brief В отчёте активирована ссылка
         */
        void linkActivated(const QUrl& _url);

    private:
        /**
         * @brief Активировать отчёт и настроить панель отображения статистических данных
         */
        void activateReport();

        /**
         * @brief Запрос на формирование отчёта
         */
        void makeReport();

        /**
         * @brief Печатать отчёт
         */
        /** @{ */
        void printReport();
        void print(QPrinter* _printer);
        /** @{ */

        /**
         * @brief Сохранить отчёт
         */
        void saveReport();

    private:
        /**
         * @brief Настроить представление
         */
        void initView();

        /**
         * @brief Настроить оформление графика
         */
        void initPlot();

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
         * @brief Заполнители верхней панели
         */
        /** @{ */
        QLabel* m_leftTopEmptyLabel = nullptr;
        QLabel* m_rightTopEmptyLabel = nullptr;
        /** @} */

        /**
         * @brief Кнопка "настройки отчёта"
         */
        FlatButton* m_settings = nullptr;

        /**
         * @brief Кнопка "печатать отчёт"
         */
        FlatButton* m_print = nullptr;

        /**
         * @brief Кнопка "сохранить отчёт"
         */
        FlatButton* m_save = nullptr;

        /**
         * @brief Кнопка "обновить отчёт"
         */
        FlatButton* m_update = nullptr;

        /**
         * @brief Панель навигации
         */
        QStackedWidget* m_navigation = nullptr;

        /**
         * @brief Панель с видами отчётов
         */
        QTreeWidget* m_statisticTypes = nullptr;

        /**
         * @brief Панель с параметрами отчётов
         */
        StatisticsSettings* m_statisticSettings = nullptr;

        /**
         * @brief Панель с данными отчёта
         */
        QStackedWidget* m_statisticData = nullptr;

        /**
         * @brief Данные отчёта
         */
        QTextBrowser* m_reportData = nullptr;

        /**
         * @brief График
         */
        QCustomPlotExtended* m_plotData = nullptr;

        /**
         * @brief Виджет перекрытие для отображения сообщения о формирующемся отчёте
         */
        QLightBoxProgress* m_progress = nullptr;
    };
}

#endif // STATISTICSVIEW_H
