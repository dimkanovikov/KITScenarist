#include "LocationReport.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <3rd_party/Widgets/PagesTextEdit/PagesTextEdit.h>

#include <QApplication>
#include <QTextBlock>
#include <QTextDocument>

using namespace BusinessLogic;

namespace {
	/**
	 * @brief Стиль документа
	 */
	static ScenarioTemplate editorStyle() {
		return ScenarioTemplateFacade::getTemplate();
	}
}


QString LocationReport::reportName(const StatisticsParameters&) const
{
	return QApplication::translate("BusinessLogic::LocationReport", "Location Report");
}

QString LocationReport::makeReport(QTextDocument* _scenario,
	const BusinessLogic::StatisticsParameters& _parameters) const
{

	PagesTextEdit edit;
	edit.setUsePageMode(true);
	edit.setPageFormat(::editorStyle().pageSizeId());
	edit.setPageMargins(::editorStyle().pageMargins());
	edit.setDocument(_scenario->clone());

	//
	// Бежим по документу и собираем информацию о сценах
	//
	QTextBlock block = _scenario->begin();
	QTextCursor cursor = edit.textCursor();
	QList<ReportData*> reportScenesDataList;
	ReportData* currentData = 0;
	while (block.isValid()) {
		if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::SceneHeading) {
			currentData = new ReportData;
			reportScenesDataList.append(currentData);
			//
			currentData->name = block.text().toUpper();
			//
			cursor.setPosition(block.position());
			currentData->page = edit.cursorPage(cursor);
			//
			if (ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(block.userData())) {
				currentData->number = info->sceneNumber();
			}
		}
		//
		if (currentData != 0) {
			currentData->chron += ChronometerFacade::calculate(block);
		}

		block = block.next();
	}

	//
	// Формируем группы по локациям
	//
	QList<ReportData*> reportLocationsDataList;
	QList<QString> locations;
	foreach (ReportData* data, reportScenesDataList) {
		const QString location = SceneHeadingParser::locationName(data->name);
		if (!locations.contains(location)) {
			locations.append(location);
			reportLocationsDataList.append(new ReportData);
		}

		ReportData* locationData = reportLocationsDataList.at(locations.indexOf(location));
		//
		locationData->name = location;
		//
		locationData->chron += data->chron;
		//
		locationData->childs.append(data);
	}

	//
	// Сортируем
	//
	switch (_parameters.locationSortByColumn) {
		default:
		case 0: {
			break;
		}

		case 1: {
			qSort(reportLocationsDataList.begin(),
				  reportLocationsDataList.end(),
				  ReportData::sortAlphabetical);
			break;
		}

		case 2: {
			qSort(reportLocationsDataList.begin(),
				  reportLocationsDataList.end(),
				  ReportData::sortFromMostToLeast);
			break;
		}

		case 3: {
			qSort(reportLocationsDataList.begin(),
				  reportLocationsDataList.end(),
				  ReportData::sortFromLeastToMost);
			break;
		}

		case 4: {
			qSort(reportLocationsDataList.begin(),
				  reportLocationsDataList.end(),
				  ReportData::sortFromLongestToShortest);
			break;
		}

		case 5: {
			qSort(reportLocationsDataList.begin(),
				  reportLocationsDataList.end(),
				  ReportData::sortFromShortestToLongest);
			break;
		}
	}

	//
	// Формируем отчёт
	//
	QString html;
	html.append("<table width=\"100%\" cellspacing=\"0\" cellpadding=\"3\">");
	//
	// ... заголовок
	//
	html.append("<tr>");
	html.append(QString("<td><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::LocationReport", "Location/Scene")));
	html.append(QString("<td width=\"8%\" align=\"center\"><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::LocationReport", "Number")));
	html.append(QString("<td width=\"8%\" align=\"center\"><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::LocationReport", "Page")));
	html.append(QString("<td width=\"8%\" align=\"center\"><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::LocationReport", "Scenes")));
	html.append(QString("<td width=\"8%\" align=\"center\"><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::LocationReport", "Chron")));
	html.append("</tr>");
	//
	// ... данные
	//
	foreach (ReportData* data, reportLocationsDataList) {
		html.append("<tr style=\"background-color: #ababab;\">");
		const QString locationName =
				data->name.isEmpty()
				? QApplication::translate("BusinessLogic::LocationReport", "[UNDEFINED]")
				: data->name;
		html.append(QString("<td><b>%1</b></td>").arg(locationName));
		html.append(QString("<td width=\"8%\" align=\"center\">%1</td>").arg(QString::null));
		html.append(QString("<td width=\"8%\" align=\"center\">%1</td>").arg(QString::null));
		html.append(QString("<td width=\"8%\" align=\"center\"><b>%1</b></td>").arg(data->childs.size()));
		html.append(QString("<td width=\"8%\" align=\"center\"><b>%1</b></td>").arg(ChronometerFacade::secondsToTime(data->chron)));
		html.append("</tr>");

		//
		// Формируем список времён действия
		//
		QList<ReportData*> reportLocationTimesDataList;
		QList<QString> locationTimes;
		foreach (ReportData* locationData, data->childs) {
			const QString time = SceneHeadingParser::timeName(locationData->name);
			if (!locationTimes.contains(time)) {
				locationTimes.append(time);
				reportLocationTimesDataList.append(new ReportData);
			}

			ReportData* locationTimeData = reportLocationTimesDataList.at(locationTimes.indexOf(time));
			//
			locationTimeData->name = time;
			//
			locationTimeData->chron += locationData->chron;
			//
			locationTimeData->childs.append(locationData);
		}

		//
		// Сортируем их
		//
		qSort(reportLocationTimesDataList.begin(),
			  reportLocationTimesDataList.end(),
			  ReportData::sortAlphabetical);

		//
		// Выводим их в отчёт
		//
		foreach (ReportData* locationTimeData, reportLocationTimesDataList) {
			html.append("<tr>");
			const QString locationTimeName =
					locationTimeData->name.isEmpty()
					? QApplication::translate("BusinessLogic::LocationReport", "[UNDEFINED]")
					: locationTimeData->name;
			html.append(QString("<td><b>%1</b></td>").arg(locationTimeName));
			html.append(QString("<td width=\"8%\" align=\"center\">%1</td>").arg(QString::null));
			html.append(QString("<td width=\"8%\" align=\"center\">%1</td>").arg(QString::null));
			html.append(QString("<td width=\"8%\" align=\"center\"><b>%1</b></td>").arg(locationTimeData->childs.size()));
			html.append(QString("<td width=\"8%\" align=\"center\"><b>%1</b></td>").arg(ChronometerFacade::secondsToTime(locationTimeData->chron)));
			html.append("</tr>");

			//
			// Если нужно использовать расширенный режим, выведем все сцены
			//
			if (_parameters.locationExtendedView) {
				foreach (ReportData* sceneData, locationTimeData->childs) {
					html.append("<tr>");
					html.append(QString("<td>%1</td>").arg(sceneData->name));
					html.append(QString("<td width=\"8%\" align=\"center\">%1</td>").arg(sceneData->number));
					html.append(QString("<td width=\"8%\" align=\"center\">%1</td>").arg(sceneData->page));
					html.append(QString("<td width=\"8%\" align=\"center\">%1</td>").arg(QString::null));
					html.append(QString("<td width=\"8%\" align=\"center\">%1</td>").arg(ChronometerFacade::secondsToTime(sceneData->chron)));
					html.append("</tr>");
				}
			}
		}

		//
		// Очищаем память
		//
		qDeleteAll(reportLocationTimesDataList);

		//
		// И добавляем пустую строку для отступа перед следующим элементом
		//
		html.append("<tr><td></td></tr>");
	}

	html.append("</table>");

	//
	// Очищаем память
	//
	qDeleteAll(reportLocationsDataList);
	qDeleteAll(reportScenesDataList);

	return html;
}
