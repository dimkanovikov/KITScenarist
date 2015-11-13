#ifndef STATISTICSFACADE_H
#define STATISTICSFACADE_H

#include "Plots/AbstractPlot.h"

class QTextDocument;

namespace BusinessLogic
{
	class StatisticsParameters;


	/**
	 * @brief Фасад для доступа к отчётам
	 */
	class StatisticsFacade
	{
	public:
		/**
		 * @brief Сформировать отчёт
		 */
		static QString makeReport(QTextDocument* _scenario, const StatisticsParameters& _parameters);

		/**
		 * @brief Сформировать график
		 */
		static Plot makePlot(QTextDocument* _scenario, const StatisticsParameters& _parameters);
	};
}

#endif // STATISTICSFACADE_H
