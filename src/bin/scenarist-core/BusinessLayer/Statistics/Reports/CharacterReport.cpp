#include "CharacterReport.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>
#include <3rd_party/Widgets/PagesTextEdit/PageTextEdit.h>

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


QString CharacterReport::reportName(const StatisticsParameters& _parameters) const
{
	QString name = QApplication::translate("BusinessLogic::CharacterReport", "Character Report");
	name.append(" - ");
	name.append(_parameters.characterName);
	return name;
}

QString CharacterReport::makeReport(QTextDocument* _scenario,
	const BusinessLogic::StatisticsParameters& _parameters) const
{
	if (_parameters.characterName.isEmpty()) {
		return QString::null;
	}


	PageTextEdit edit;
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
	bool saveDialogues = false;
	while (block.isValid()) {
		if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::SceneHeading) {
			currentData = new ReportData;
			reportScenesDataList.append(currentData);
			//
			currentData->scene = block.text().toUpper();
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
			if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Character) {
				if (CharacterParser::name(block.text().toUpper()) == _parameters.characterName) {
					saveDialogues = true;
					if (!currentData->dialogues.isEmpty()) {
						currentData->dialogues.append(QString::null);
					}
				} else {
					saveDialogues = false;
				}
			} else if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Dialogue
					   || ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Parenthetical) {
				if (saveDialogues) {
					currentData->dialogues.append(block.text());
				}
			}
		}

		block = block.next();
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
				.arg(QApplication::translate("BusinessLogic::CharacterReport", "Scene/Dialogue")));
	html.append(QString("<td width=\"8%\" align=\"center\"><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::CharacterReport", "Number")));
	html.append(QString("<td width=\"8%\" align=\"center\"><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::CharacterReport", "Page")));
	html.append("</tr>");
	//
	// ... данные
	//
	foreach (ReportData* data, reportScenesDataList) {
		if (!data->dialogues.isEmpty()) {
			html.append("<tr style=\"background-color: #ababab;\">");
			const QString sceneName =
					data->scene.isEmpty()
					? QApplication::translate("BusinessLogic::CharacterReport", "[UNDEFINED]")
					: data->scene;
			html.append(QString("<td><b>%1</b></td>").arg(sceneName));
			html.append(QString("<td width=\"8%\" align=\"center\"><b>%1</b></td>").arg(data->number));
			html.append(QString("<td width=\"8%\" align=\"center\"><b>%1</b></td>").arg(data->page));
			html.append("</tr>");

			html.append("<tr><td colspan=\"3\">");// style=\"background-color: #ababab;\">");
			QString dialogueText;
			foreach (const QString& dialogue, data->dialogues) {
				if (!dialogueText.isEmpty()) {
					dialogueText.append("<br/>");
				}
				dialogueText.append(dialogue);
			}
			html.append(dialogueText);
			html.append("</td></tr>");

			//
			// И добавляем пустую строку для отступа перед следующим элементом
			//
			html.append("<tr><td></td></tr>");
		}
	}

	html.append("</table>");

	//
	// Очищаем память
	//
	qDeleteAll(reportScenesDataList);

	return html;
}
