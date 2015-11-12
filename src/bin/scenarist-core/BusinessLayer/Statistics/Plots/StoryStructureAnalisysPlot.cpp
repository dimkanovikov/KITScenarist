#include "StoryStructureAnalisysPlot.h"

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


QString StoryStructureAnalisysPlot::plotName(const BusinessLogic::StatisticsParameters& _parameters) const
{
	Q_UNUSED(_parameters);
	return QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Story Structure Analisys Plot");
}

QVector<BusinessLogic::PlotData> StoryStructureAnalisysPlot::makePlot(QTextDocument* _scenario, const BusinessLogic::StatisticsParameters& _parameters) const
{
	PagesTextEdit edit;
	edit.setUsePageMode(true);
	edit.setPageFormat(::editorStyle().pageSizeId());
	edit.setPageMargins(::editorStyle().pageMargins());
	edit.setDocument(_scenario->clone());

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
	rxPattern.prepend("(^|\\W)(");
	rxPattern.append(")($|\\W)");
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
			// Описание действия, выуживаем молчаливых и сохраняем хронометраж
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
				//
				currentData->actionChron += ChronometerFacade::calculate(block);
			}
			//
			// Реплика, просто считываем хронометраж
			//
			else if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Dialogue) {
				currentData->dialogsChron += ChronometerFacade::calculate(block);
			}

			currentData->chron += ChronometerFacade::calculate(block);
		}

		block = block.next();
	}

	//
	// Формируем данные для визуализации
	//
	// ... х - общий для всех
	QVector<double> x;
	// ... y
	QVector<double> sceneChronY;
	QVector<double> actionChronY;
	QVector<double> dialogsChronY;
	QVector<double> charactersCountY;
	QVector<double> dialogsCountY;
	foreach (SceneData* data, reportScenesDataList) {
		x << data->number;
		sceneChronY << data->chron;
		actionChronY << data->actionChron;
		dialogsChronY << data->dialogsChron;
		charactersCountY << data->characters.size();
		int dialogsCounter = 0;
		foreach (const SceneCharacter& character, data->characters) {
			dialogsCounter += character.dialogsCount;
		}
		dialogsCountY << dialogsCounter;
	}
	//
	QVector<BusinessLogic::PlotData> result;
	//
	// ... хронометраж сцены
	//
	if (_parameters.storyStructureAnalisysSceneChron) {
		BusinessLogic::PlotData data;
		data.name = QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Scene Duration");
		data.color = QColor("#FF3030");
		data.x = x;
		data.y = sceneChronY;
		result.append(data);
	}
	//
	// ... хронометраж действий
	//
	if (_parameters.storyStructureAnalisysActionChron) {
		BusinessLogic::PlotData data;
		data.name = QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Action Duration");
		data.color = QColor("#FFC600");
		data.x = x;
		data.y = actionChronY;
		result.append(data);
	}
	//
	// ... хронометраж реплик
	//
	if (_parameters.storyStructureAnalisysDialoguesChron) {
		BusinessLogic::PlotData data;
		data.name = QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Dialogues Duration");
		data.color = QColor("#BF6DE8");
		data.x = x;
		data.y = dialogsChronY;
		result.append(data);
	}
	//
	// ... количество персонажей
	//
	if (_parameters.storyStructureAnalisysCharactersCount) {
		BusinessLogic::PlotData data;
		data.name = QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Characters Count");
		data.color = QColor("#95D900");
		data.x = x;
		data.y = charactersCountY;
		result.append(data);
	}
	//
	// ... хронометраж действий
	//
	if (_parameters.storyStructureAnalisysDialoguesCount) {
		BusinessLogic::PlotData data;
		data.name = QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Dialogs Count");
		data.color = QColor("#00B6F7");
		data.x = x;
		data.y = dialogsCountY;
		result.append(data);
	}

	return result;
}
