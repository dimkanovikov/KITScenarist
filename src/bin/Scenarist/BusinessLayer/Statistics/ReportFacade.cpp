#include "ReportFacade.h"

#include "AbstractReport.h"
#include "SummaryReport.h"
#include "SceneReport.h"
#include "LocationReport.h"
#include "CastReport.h"
#include "CharacterReport.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioDataStorage.h>
#include <DataLayer/Database/Database.h>

#include <QApplication>
#include <QDateTime>
#include <QFileInfo>


QString BusinessLogic::ReportFacade::makeReport(QTextDocument* _scenario, const BusinessLogic::ReportParameters& _parameters)
{
	QString result;
	switch (_parameters.type) {
		default:
		case ReportParameters::Report: {
			//
			// Определим отчёт
			//
			AbstractReport* report = 0;
			switch (_parameters.reportType) {
				default:
				case ReportParameters::SummaryReport: {
					report = new SummaryReport;
					break;
				}

				case ReportParameters::SceneReport: {
					report = new SceneReport;
					break;
				}

				case ReportParameters::LocationReport: {
					report = new LocationReport;
					break;
				}

				case ReportParameters::CastReport: {
					report = new CastReport;
					break;
				}

				case ReportParameters::CharacterReport: {
					report = new CharacterReport;
					break;
				}
			}

			//
			// Формируем отчёт
			//
			result.append("<div style=\"margin-left: 10px; margin-top: 10px; margin-right: 10px; margin-bottom: 10px;\">");
			QString scenarioName = DataStorageLayer::StorageFacade::scenarioDataStorage()->name();
			if (scenarioName.isEmpty()) {
				QFileInfo fileInfo(DatabaseLayer::Database::currentFile());
				scenarioName = fileInfo.completeBaseName();
			}
			result.append(
				QString("<table width=\"100%\"><tr><td><b>%1</b><br/><b>%2</b></td>"
						"<td valign=\"top\" align=\"right\"><small>%3 %4</small></td></tr></table>")
						.arg(scenarioName)
						.arg(report->reportName(_parameters))
						.arg(QApplication::translate("BusinessLayer::ReportFacade", "generated"))
						.arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss t"))
						);
			result.append("<hr width=\"100%\"></hr>");
			result.append(report->makeReport(_scenario, _parameters));
			result.append("</div>");

			break;
		}
	}

	return result;
}
