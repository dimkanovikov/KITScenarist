#ifndef REPORTFACADE_H
#define REPORTFACADE_H

#include <QString>

class QTextDocument;

namespace BusinessLogic
{
	class ReportParameters;


	/**
	 * @brief Фасад для доступа к отчётам
	 */
	class ReportFacade
	{
	public:
		/**
		 * @brief Сформировать отчёт
		 */
		static QString makeReport(QTextDocument* _scenario, const ReportParameters& _parameters);
	};
}

#endif // REPORTFACADE_H
