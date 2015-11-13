#ifndef ABSTRACTPLOT
#define ABSTRACTPLOT

#include "../StatisticsParameters.h"

#include <QColor>
#include <QMap>
#include <QStringList>
#include <QVector>

class QTextDocument;

namespace BusinessLogic
{
	/**
	 * @brief Данные графика
	 */
	class PlotData
	{
	public:
		PlotData() {}

		/**
		 * @brief Название
		 */
		QString name;

		/**
		 * @brief Цвет
		 */
		QColor color;

		/**
		 * @brief Координаты
		 */
		/** @{ */
		QVector<double> x;
		QVector<double> y;
		/** @} */
	};

	/**
	 * @brief Дополнительная информация о графике
	 */
	class Plot
	{
	public:
		Plot() {}

		/**
		 * @brief Дополнительная информация о графике
		 * @note Используется в качестве подсказки в QCustomPlotExtended
		 */
		QMap<double, QStringList> info;

		/**
		 * @brief Данные о графике
		 */
		QVector<PlotData> data;
	};

	/**
	 * @brief Базовый класс для графика
	 */
	class AbstractPlot
	{
	public:
		virtual ~AbstractPlot() {}

		/**
		 * @brief Получить название графика
		 */
		virtual QString plotName(const StatisticsParameters& _parameters) const = 0;

		/**
		 * @brief Сформировать график по заданному сценарию с установленными параметрами
		 */
		virtual Plot makePlot(QTextDocument* _scenario,
			const StatisticsParameters& _parameters) const = 0;
	};
}

#endif // ABSTRACTPLOT

