#include "SceneReport.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>

#include <Domain/Character.h>

#include <3rd_party/Widgets/PagesTextEdit/PagesTextEdit.h>

#include <QApplication>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextEdit>
#include <QRegularExpression>

using namespace BusinessLogic;

namespace {
	/**
	 * @brief Стиль документа
	 */
	static ScenarioTemplate editorStyle() {
		return ScenarioTemplateFacade::getTemplate();
	}
}


QString SceneReport::reportName(const ReportParameters&) const
{
	return QApplication::translate("BusinessLogic::SceneReport", "Scene Report");
}

QString SceneReport::makeReport(QTextDocument* _scenario,
	const BusinessLogic::ReportParameters& _parameters) const
{
	PagesTextEdit edit;
	edit.setUsePageMode(true);
	edit.setPageFormat(::editorStyle().pageSizeId());
	edit.setPageMargins(::editorStyle().pageMargins());
	edit.setDocument(_scenario);

	//
	// Сформируем регулярное выражение для выуживания молчаливых персонажей
	//
	QString rxPattern;
	foreach (DomainObject* characterObject,
			 DataStorageLayer::StorageFacade::characterStorage()->all()->toList()) {
		Character* character = dynamic_cast<Character*>(characterObject);
		if (rxPattern.isEmpty()) {
			rxPattern.append(character->name());
		} else {
			rxPattern.append("|" + character->name());
		}
	}
	rxPattern.prepend("(^|\\W)[(]");
	rxPattern.append("[)]($|\\W)");
	QRegularExpression rxCharacterFinder(rxPattern,
		QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption);


	//
	// Бежим по документу и собираем информацию о сценах и персонажах в них
	//
	QTextBlock block = _scenario->begin();
	QTextCursor cursor = edit.textCursor();
	QList<SceneData*> reportScenesDataList;
	SceneData* currentData = 0;
	QStringList characters;
	while (block.isValid()) {
		if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::SceneHeading) {
			currentData = new SceneData;
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
			//
			// Участники сцены
			//
			if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::SceneCharacters) {
				const QStringList sceneCharacters = SceneCharactersParser::characters(block.text().toUpper());
				foreach (const QString& character, sceneCharacters) {
					//
					// Первое появление
					//
					if (!characters.contains(character)) {
						characters.append(character);
						currentData->characters.append(SceneCharacter(character));
					}
					//
					// Повторное появление
					//
					else {
						int characterIndex = currentData->characterIndex(character);
						if (characterIndex == -1) {
							currentData->characters.append(SceneCharacter(character));
							currentData->characters.last().isFirstOccurence = false;
						}
					}
				}
			}
			//
			// Персонаж
			//
			else if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Character) {
				const QString character = CharacterParser::name(block.text().toUpper());
				//
				// Первое появление
				//
				if (!characters.contains(character)) {
					characters.append(character);
					currentData->characters.append(SceneCharacter(character));
					currentData->characters.last().dialogsCount += 1;
				}
				//
				// Повторное появление
				//
				else {
					int characterIndex = currentData->characterIndex(character);
					if (characterIndex == -1) {
						currentData->characters.append(SceneCharacter(character));
						currentData->characters.last().isFirstOccurence = false;
						currentData->characters.last().dialogsCount += 1;
					} else {
						currentData->characters[characterIndex].dialogsCount += 1;
					}
				}
			}
			//
			// Описание действия, выуживаем молчаливых
			//
			else if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Action) {
				QRegularExpressionMatch match = rxCharacterFinder.match(block.text());
				while (match.hasMatch()) {
					const QString character = match.captured(2).toUpper();
					//
					// Первое появление
					//
					if (!characters.contains(character)) {
						characters.append(character);
						currentData->characters.append(SceneCharacter(character));
					}
					//
					// Повторное появление
					//
					else {
						int characterIndex = currentData->characterIndex(character);
						if (characterIndex == -1) {
							currentData->characters.append(SceneCharacter(character));
							currentData->characters.last().isFirstOccurence = false;
						}
					}

					//
					// Ищем дальше
					//
					match = rxCharacterFinder.match(block.text(), match.capturedEnd());
				}
			}

			currentData->chron += ChronometerFacade::calculate(block);
		}

		block = block.next();
	}

	//
	// Сортируем
	//
	switch (_parameters.sceneSortByColumn) {
		default:
		case 0: {
			break;
		}

		case 1: {
			qSort(reportScenesDataList.begin(),
				  reportScenesDataList.end(),
				  SceneData::sortAlphabetical);
			break;
		}

		case 2: {
			qSort(reportScenesDataList.begin(),
				  reportScenesDataList.end(),
				  SceneData::sortFromLongestToShortest);
			break;
		}

		case 3: {
			qSort(reportScenesDataList.begin(),
				  reportScenesDataList.end(),
				  SceneData::sortFromShortestToLongest);
			break;
		}

		case 4: {
			qSort(reportScenesDataList.begin(),
				  reportScenesDataList.end(),
				  SceneData::sortFromMassiveToUnmanned);
			break;
		}

		case 5: {
			qSort(reportScenesDataList.begin(),
				  reportScenesDataList.end(),
				  SceneData::sortFromUnmannedToMassive);
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
				.arg(QApplication::translate("BusinessLogic::SceneReport", "Scene/Characters")));
	html.append(QString("<td width=\"8%\" align=\"center\"><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::SceneReport", "Number")));
	html.append(QString("<td width=\"8%\" align=\"center\"><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::SceneReport", "Page")));
	html.append(QString("<td width=\"8%\" align=\"center\"><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::SceneReport", "Characters")));
	html.append(QString("<td width=\"8%\" align=\"center\"><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::SceneReport", "Chron")));
	html.append("</tr>");
	//
	// ... данные
	//
	const bool styledHeaderFormat = _parameters.sceneShowCharacters;
	foreach (SceneData* data, reportScenesDataList) {
		if (styledHeaderFormat) {
			html.append("<tr style=\"background-color: #ababab;\">");
		} else {
			html.append("<tr>");
		}
		QString sceneName =
				data->name.isEmpty()
				? QApplication::translate("BusinessLogic::SceneReport", "[UNDEFINED]")
				: data->name;

		if (styledHeaderFormat) {
			html.append(QString("<td><b>%1</b></td>").arg(sceneName));
			html.append(QString("<td width=\"8%\" align=\"center\"><b>%1</b></td>").arg(data->number));
			html.append(QString("<td width=\"8%\" align=\"center\"><b>%1</b></td>").arg(data->page));
			html.append(QString("<td width=\"8%\" align=\"center\"><b>%1</b></td>").arg(data->characters.size()));
			html.append(QString("<td width=\"8%\" align=\"center\"><b>%1</b></td>").arg(ChronometerFacade::secondsToTime(data->chron)));
		} else {
			html.append(QString("<td>%1</td>").arg(sceneName));
			html.append(QString("<td width=\"8%\" align=\"center\">%1</td>").arg(data->number));
			html.append(QString("<td width=\"8%\" align=\"center\">%1</td>").arg(data->page));
			html.append(QString("<td width=\"8%\" align=\"center\">%1</td>").arg(data->characters.size()));
			html.append(QString("<td width=\"8%\" align=\"center\">%1</td>").arg(ChronometerFacade::secondsToTime(data->chron)));
		}
		html.append("</tr>");

		//
		// Если нужно выводим информацию о персонажах
		//
		if (_parameters.sceneShowCharacters) {
			if (!data->characters.isEmpty()) {
				QString charactersNames;
				foreach (const SceneCharacter& character, data->characters) {
					if (!charactersNames.isEmpty()) {
						charactersNames.append(", ");
					}

					if (character.isFirstOccurence) {
						charactersNames.append("<u>");
					}
					charactersNames.append(character.name);
					if (character.isFirstOccurence) {
						charactersNames.append("</u>");
					}
					charactersNames.append(QString("(%1)").arg(character.dialogsCount));
				}

				html.append("<tr>");
				html.append(QString("<td>%1</td>").arg(charactersNames));
				html.append("</tr>");
			}

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
