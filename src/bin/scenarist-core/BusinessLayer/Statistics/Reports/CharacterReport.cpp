#include "CharacterReport.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>
#include <3rd_party/Widgets/PagesTextEdit/PageTextEdit.h>

#include <QApplication>
#include <QPalette>
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
    name.append(_parameters.characterNames.join(","));
	return name;
}

QString CharacterReport::makeReport(QTextDocument* _scenario,
	const BusinessLogic::StatisticsParameters& _parameters) const
{
    if (_parameters.characterNames.isEmpty()) {
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
    QString characterName;
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
                characterName = CharacterParser::name(block.text().toUpper());
                if (_parameters.characterNames.contains(characterName)) {
					saveDialogues = true;
                    if (!currentData->dialogues.isEmpty()) {
                        currentData->dialogues.append({ "", "", 0 });
					}
				} else {
					saveDialogues = false;
				}
			} else if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Dialogue
					   || ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Parenthetical) {
				if (saveDialogues) {
                    currentData->dialogues.append({ characterName, block.text(), block.position() });
				}
			}
		}

		block = block.next();
	}


	//
	// Формируем отчёт
	//
	QString html;
    html.append(
        QString("<style>a { color: %1; text-decoration: none; }</style>")
                .arg(QApplication::palette().text().color().name()));
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
            //
            // Если персонажей несколько, то в отчёте показываем только пересекающиеся сцены
            //
            if (_parameters.characterNames.size() > 1) {
                QStringList characters = _parameters.characterNames;
                foreach (const QVariantList dialogueData, data->dialogues) {
                    characters.removeOne(dialogueData.first().toString());
                }
                //
                // Если не все персонажи беседуют в этой сцене - пропускаем её
                //
                if (!characters.isEmpty()) {
                    continue;
                }
            }

			html.append("<tr style=\"background-color: #ababab;\">");
			const QString sceneName =
					data->scene.isEmpty()
					? QApplication::translate("BusinessLogic::CharacterReport", "[UNDEFINED]")
					: data->scene;
			html.append(QString("<td><b>%1</b></td>").arg(sceneName));
			html.append(QString("<td width=\"8%\" align=\"center\"><b>%1</b></td>").arg(data->number));
			html.append(QString("<td width=\"8%\" align=\"center\"><b>%1</b></td>").arg(data->page));
			html.append("</tr>");

            html.append("<tr><td colspan=\"3\">");
            QString dialogueText;
            QString lastCharacter;
            foreach (const QVariantList dialogueData, data->dialogues) {
                if (!dialogueData.first().toString().isEmpty()) {
                    //
                    // Вид отчёта для одного персонажа
                    //
                    if (_parameters.characterNames.size() == 1) {
                        dialogueText.append(
                                    QString("<tr><td><a href=\"inapp://scenario?position=%2\">%1</a></td></tr>")
                                    .arg(dialogueData.value(1).toString())
                                    .arg(dialogueData.value(2).toInt()));
                    }
                    //
                    // Вид отчёта для нескольких персонажей
                    //
                    else {
                        QString currentCharacter = dialogueData.value(0).toString() + ":";
                        if (currentCharacter == lastCharacter) {
                            currentCharacter.clear();
                        }
                        dialogueText.append(
                                    QString("<tr><td>%1</td><td width=\"100%\"><a href=\"inapp://scenario?position=%3\">%2</a></td></tr>")
                                    .arg(currentCharacter)
                                    .arg(dialogueData.value(1).toString())
                                    .arg(dialogueData.value(2).toInt()));
                        if (!currentCharacter.isEmpty()) {
                            lastCharacter = currentCharacter;
                        }
                    }
                }
            }
            html.append("<table width=\"100%\" cellspacing=\"0\" cellpadding=\"3\">");
            html.append(dialogueText);
            html.append("</table>");
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
