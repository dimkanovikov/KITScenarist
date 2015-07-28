#ifndef SUMMARYREPORT_H
#define SUMMARYREPORT_H

#include "AbstractReport.h"


namespace BusinessLogic
{
	/**
	 * @brief Отчёт по сценам
	 */
	class SummaryReport : public AbstractReport
	{
	public:
		SummaryReport() {}

		/**
		 * @brief Название
		 */
		QString reportName(const ReportParameters&) const;

		/**
		 * @brief Подготовить отчёт
		 */
		QString makeReport(QTextDocument *_scenario, const ReportParameters &_parameters) const;
	};
}

#endif // SUMMARYREPORT_H
