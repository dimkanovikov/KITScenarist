#ifndef STATISTICSSETTINGS_H
#define STATISTICSSETTINGS_H

#include <BusinessLayer/Statistics/AbstractReport.h>

#include <QStackedWidget>

class QAbstractItemModel;

namespace Ui {
	class StatisticsSettings;
}


namespace UserInterface
{
	/**
	 * @brief Класс виджета с настройками отчётов/графиков
	 */
	class StatisticsSettings : public QStackedWidget
	{
		Q_OBJECT

	public:
		explicit StatisticsSettings(QWidget *parent = 0);
		~StatisticsSettings();

		/**
		 * @brief Задать персонажей
		 */
		void setCharacters(QAbstractItemModel* _characters);

		/**
		 * @brief Задать элементы сценария
		 */
		void setScriptElements(QAbstractItemModel* _elements);

		/**
		 * @brief Получить параметры отчётов
		 */
		const BusinessLogic::ReportParameters& settings() const;

	signals:
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

	private:
		Ui::StatisticsSettings *ui;

		/**
		 * @brief Параметры отчёта
		 */
		mutable BusinessLogic::ReportParameters m_settings;
	};
}

#endif // STATISTICSSETTINGS_H
