#include "CastReport.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ResearchStorage.h>

#include <Domain/Research.h>

#include <QApplication>
#include <QTextBlock>
#include <QTextDocument>
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


QString CastReport::reportName(const StatisticsParameters&) const
{
	return QApplication::translate("BusinessLogic::CastReport", "Cast Report");
}

QString CastReport::makeReport(QTextDocument* _scenario,
	const BusinessLogic::StatisticsParameters& _parameters) const
{
	//
	// Сформируем регулярное выражение для выуживания молчаливых персонажей
	//
	QString rxPattern;
	foreach (DomainObject* characterObject,
             DataStorageLayer::StorageFacade::researchStorage()->characters()->toList()) {
        Research* character = dynamic_cast<Research*>(characterObject);
		if (rxPattern.isEmpty()) {
			rxPattern.append(character->name());
		} else {
			rxPattern.append("|" + character->name());
		}
	}
	rxPattern.prepend("(^|\\W)(");
	rxPattern.append(")($|\\W)");
	QRegularExpression rxCharacterFinder(rxPattern,
		QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption);


	//
	// Бежим по документу и собираем информацию о сценах и персонажах в них
	//
	QTextBlock block = _scenario->begin();
	QList<CharacterData*> reportCharactersDataList;
	QStringList characters;
	QStringList sceneSpeakingCharacters;
	QStringList sceneNonspeakingCharacters;
	while (block.isValid()) {
		//
		// Сцена, всё очищаем
		//
		if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::SceneHeading) {
			sceneSpeakingCharacters.clear();
			sceneNonspeakingCharacters.clear();
		}
		//
		// Список персонажей, всех в молчаливых
		//
		else if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::SceneCharacters) {
			const QStringList sceneCharacters = SceneCharactersParser::characters(block.text().toUpper());
			foreach (const QString& character, sceneCharacters) {
				if (!characters.contains(character)) {
					characters.append(character);
					sceneNonspeakingCharacters.append(character);
					reportCharactersDataList.append(new CharacterData(character));
					reportCharactersDataList.last()->nonspeakingScenesCount += 1;
				} else {
					//
					// Если он ещё не добавлен в текущую сцену
					//
					if (!sceneSpeakingCharacters.contains(character)
						&& !sceneNonspeakingCharacters.contains(character)) {
						sceneNonspeakingCharacters.append(character);
						reportCharactersDataList[characters.indexOf(character)]->nonspeakingScenesCount += 1;
					}
				}
			}
		}
		//
		// Персонаж +1 реплика и в список говорящих
		//
		else if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Character) {
			const QString character = CharacterParser::name(block.text().toUpper());
			if (!characters.contains(character)) {
				characters.append(character);
				sceneSpeakingCharacters.append(character);
				reportCharactersDataList.append(new CharacterData(character));
				reportCharactersDataList.last()->speakingScenesCount += 1;
				reportCharactersDataList.last()->dialogsCount += 1;
			} else {
				//
				// Если он уже был и в говорящих
				//
				if (sceneSpeakingCharacters.contains(character)) {
					//
					// Просто увеличиваем число реплик
					//
					reportCharactersDataList[characters.indexOf(character)]->dialogsCount += 1;
				}
				//
				// Если был в молчаливых
				//
				else if (sceneNonspeakingCharacters.contains(character)) {
					//
					// То перемещаем в говорящих
					//
					sceneNonspeakingCharacters.removeAll(character);
					sceneSpeakingCharacters.append(character);
					reportCharactersDataList[characters.indexOf(character)]->nonspeakingScenesCount -= 1;
					reportCharactersDataList[characters.indexOf(character)]->speakingScenesCount += 1;
					reportCharactersDataList[characters.indexOf(character)]->dialogsCount += 1;
				}
				//
				// Если не было в сцене
				//
				else {
					sceneSpeakingCharacters.append(character);
					reportCharactersDataList[characters.indexOf(character)]->speakingScenesCount += 1;
					reportCharactersDataList[characters.indexOf(character)]->dialogsCount += 1;
				}
			}
		}
		//
		// Описание действия, в молчаливые, если ещё не встречался
		//
		else if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Action
				 && !block.text().isEmpty()) {
			QRegularExpressionMatch match = rxCharacterFinder.match(block.text());
			while (match.hasMatch()) {
				const QString character = match.captured(2).toUpper();
				if (!characters.contains(character)) {
					characters.append(character);
					sceneNonspeakingCharacters.append(character);
					reportCharactersDataList.append(new CharacterData(character));
					reportCharactersDataList.last()->nonspeakingScenesCount += 1;
				} else {
					//
					// Если он ещё не добавлен в текущую сцену
					//
					if (!sceneSpeakingCharacters.contains(character)
						&& !sceneNonspeakingCharacters.contains(character)) {
						sceneNonspeakingCharacters.append(character);
						reportCharactersDataList[characters.indexOf(character)]->nonspeakingScenesCount += 1;
					}
				}

				//
				// Ищем дальше
				//
				match = rxCharacterFinder.match(block.text(), match.capturedEnd());
			}
		}

		block = block.next();
	}

	//
	// Сортируем
	//
	switch (_parameters.castSortByColumn) {
		default:
		case 0: {
			break;
		}

		case 1: {
			qSort(reportCharactersDataList.begin(),
				  reportCharactersDataList.end(),
				  CharacterData::sortAlphabetical);
			break;
		}

		case 2: {
			qSort(reportCharactersDataList.begin(),
				  reportCharactersDataList.end(),
				  CharacterData::sortFromMostToLeastScenes);
			break;
		}

		case 3: {
			qSort(reportCharactersDataList.begin(),
				  reportCharactersDataList.end(),
				  CharacterData::sortFromLeastToMostScenes);
			break;
		}

		case 4: {
			qSort(reportCharactersDataList.begin(),
				  reportCharactersDataList.end(),
				  CharacterData::sortFromMostToLeastDialogs);
			break;
		}

		case 5: {
			qSort(reportCharactersDataList.begin(),
				  reportCharactersDataList.end(),
				  CharacterData::sortFromLeastToMostDialogs);
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
				.arg(QApplication::translate("BusinessLogic::CastReport", "Character")));
	html.append(QString("<td align=\"center\"><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::CastReport", "Total Dialogues")));
	if (_parameters.castShowSpeakingAndNonspeakingScenes) {
		html.append(QString("<td align=\"center\"><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::CastReport", "Speaking Scenes")));
		html.append(QString("<td align=\"center\"><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::CastReport", "Non-Sp. Scenes")));
	}
	html.append(QString("<td align=\"center\"><u>%1</u></td>")
				.arg(QApplication::translate("BusinessLogic::CastReport", "Total Scenes")));
	html.append("</tr>");
	//
	// ... данные
	//
	foreach (CharacterData* data, reportCharactersDataList) {
		html.append("<tr>");
		html.append(QString("<td>%1</td>").arg(data->name));
		html.append(QString("<td align=\"center\">%1</td>").arg(data->dialogsCount));
		if (_parameters.castShowSpeakingAndNonspeakingScenes) {
			html.append(QString("<td align=\"center\">%1</td>").arg(data->speakingScenesCount));
			html.append(QString("<td align=\"center\">%1</td>").arg(data->nonspeakingScenesCount));
		}
		html.append(QString("<td align=\"center\">%1</td>").arg(data->scenesCount()));
		html.append("</tr>");
	}

	html.append("</table>");

	//
	// Очищаем память
	//
	qDeleteAll(reportCharactersDataList);

	return html;
}


