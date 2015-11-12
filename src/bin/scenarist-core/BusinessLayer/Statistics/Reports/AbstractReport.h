#ifndef ABSTRACTREPORT
#define ABSTRACTREPORT

#include "../StatisticsParameters.h"

#include <QString>

class QTextDocument;


namespace BusinessLogic
{
	/**
	 * @brief Базовый класс для отчёта
	 */
	class AbstractReport
	{
	public:
		virtual ~AbstractReport() {}

		/**
		 * @brief Получить название отчёта
		 */
		virtual QString reportName(const StatisticsParameters& _parameters) const = 0;

		/**
		 * @brief Сформировать отчёт по заданному сценарию с установленными параметрами
		 */
		virtual QString makeReport(QTextDocument* _scenario,
			const StatisticsParameters& _parameters) const = 0;
	};
}

#endif // ABSTRACTREPORT

