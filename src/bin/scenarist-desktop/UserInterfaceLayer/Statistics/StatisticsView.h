#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

#include <QWidget>

class FlatButton;
class ProgressWidget;
class QAbstractItemModel;
class QCustomPlot;
class QFrame;
class QLabel;
class QPrinter;
class QStackedWidget;
class QTextBrowser;

namespace BusinessLogic {
	class StatisticsParameters;
	class PlotData;
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
		void setPlot(const QVector<BusinessLogic::PlotData>& _plotData);

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
		void makeReport(const BusinessLogic::StatisticsParameters& _parameters);

	private slots:
		/**
		 * @brief Настроить панель отображения статистических данных
		 */
		void aboutInitDataPanel();

		/**
		 * @brief Запрос на формирование отчёта
		 */
		void aboutMakeReport();

		/**
		 * @brief Печатать отчёт
		 */
		/** @{ */
		void aboutPrintReport();
		void aboutPrint(QPrinter* _printer);
		/** @{ */

		/**
		 * @brief Сохранить отчёт
		 */
		void aboutSaveReport();

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
		 * @brief Заполнители верхней панели
		 */
		/** @{ */
		QLabel* m_leftTopEmptyLabel;
		QLabel* m_rightTopEmptyLabel;
		/** @} */

		/**
		 * @brief Кнопка "настройки отчёта"
		 */
		FlatButton* m_settings;

		/**
		 * @brief Кнопка "печатать отчёт"
		 */
		FlatButton* m_print;

		/**
		 * @brief Кнопка "сохранить отчёт"
		 */
		FlatButton* m_save;

		/**
		 * @brief Кнопка "обновить отчёт"
		 */
		FlatButton* m_update;

		/**
		 * @brief Панель с видами отчётов
		 */
		QFrame* m_statisticTypes;

		/**
		 * @brief Панель с параметрами отчётов
		 */
		StatisticsSettings* m_statisticSettings;

		/**
		 * @brief Панель с данными отчёта
		 */
		QStackedWidget* m_statisticData;

		/**
		 * @brief Кнопки отчётов
		 */
		QList<ReportButton*> m_reports;

		/**
		 * @brief Данные отчёта
		 */
		QTextBrowser* m_reportData;

		/**
		 * @brief График
		 */
		QCustomPlot* m_plotData;

		/**
		 * @brief Виджет перекрытие для отображения сообщения о формирующемся отчёте
		 */
		ProgressWidget* m_progress;
	};
}

#endif // STATISTICSVIEW_H
