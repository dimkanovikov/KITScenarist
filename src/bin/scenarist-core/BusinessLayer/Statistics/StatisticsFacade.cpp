#include "StatisticsFacade.h"

#include "Reports/AbstractReport.h"
#include "Reports/SummaryReport.h"
#include "Reports/SceneReport.h"
#include "Reports/LocationReport.h"
#include "Reports/CastReport.h"
#include "Reports/CharacterReport.h"

#include "Plots/AbstractPlot.h"
#include "Plots/StoryStructureAnalisysPlot.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioDataStorage.h>
#include <DataLayer/Database/Database.h>

#include <QApplication>
#include <QDateTime>
#include <QFileInfo>


QString BusinessLogic::StatisticsFacade::makeReport(QTextDocument* _scenario, const BusinessLogic::StatisticsParameters& _parameters)
{
	QString result;
	switch (_parameters.type) {
		default:
		case StatisticsParameters::Report: {
			//
			// Определим отчёт
			//
			AbstractReport* report = 0;
			switch (_parameters.reportType) {
				default:
				case StatisticsParameters::SummaryReport: {
					report = new SummaryReport;
					break;
				}

				case StatisticsParameters::SceneReport: {
					report = new SceneReport;
					break;
				}

				case StatisticsParameters::LocationReport: {
					report = new LocationReport;
					break;
				}

				case StatisticsParameters::CastReport: {
					report = new CastReport;
					break;
				}

				case StatisticsParameters::CharacterReport: {
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
						.arg(QApplication::translate("BusinessLogic::ReportFacade", "generated"))
						.arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss t"))
						);
			result.append("<hr width=\"100%\"></hr>");
			result.append(report->makeReport(_scenario, _parameters));
			result.append("</div>");

			delete report;
			report = 0;

			break;
		}
	}

	return result;
}

BusinessLogic::Plot BusinessLogic::StatisticsFacade::makePlot(
	QTextDocument* _scenario, const BusinessLogic::StatisticsParameters& _parameters)
{
	BusinessLogic::Plot result;
	switch (_parameters.type) {
		default:
		case StatisticsParameters::Plot: {
			//
			// Определим отчёт
			//
			AbstractPlot* plot = 0;
			switch (_parameters.plotType) {
				default:
				case StatisticsParameters::StoryStructureAnalisysPlot: {
					plot = new StoryStructureAnalisysPlot;
					break;
				}
			}

			result = plot->makePlot(_scenario, _parameters);

			delete plot;
			plot = 0;

			break;
		}
	}

	return result;
}
