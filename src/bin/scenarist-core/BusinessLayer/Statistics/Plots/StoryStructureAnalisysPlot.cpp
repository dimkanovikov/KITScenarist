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

	/**
	 * @brief Названия графиков
	 */
	/** @{ */
	static QString sceneChronLabel() {
		return QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Scene Duration");
	}
	static QString actionChronLabel() {
		return QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Action Duration");
	}
	static QString dialoguesChronLabel() {
		return QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Dialogues Duration");
	}
	static QString charactersCountLabel() {
		return QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Characters Count");
	}
	static QString dialoguesCountLabel() {
		return QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Dialogs Count");
	}
	/** @} */
}


QString StoryStructureAnalisysPlot::plotName(const BusinessLogic::StatisticsParameters& _parameters) const
{
	Q_UNUSED(_parameters);
	return QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Story Structure Analisys Plot");
}

Plot StoryStructureAnalisysPlot::makePlot(QTextDocument* _scenario, const BusinessLogic::StatisticsParameters& _parameters) const
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
	QStringList sceneCharacters;
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
			//
			sceneCharacters.clear();
		}
		//
		if (currentData != 0) {
			//
			// Участники сцены
			//
			if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::SceneCharacters) {
				const QStringList sceneCharacters = SceneCharactersParser::characters(block.text().toUpper());
				foreach (const QString& character, sceneCharacters) {
					if (!sceneCharacters.contains(character)) {
						currentData->charactersCount += 1;
					}
				}
			}
			//
			// Персонаж
			//
			else if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Character) {
				const QString character = CharacterParser::name(block.text().toUpper());
				if (!sceneCharacters.contains(character)) {
					currentData->charactersCount += 1;
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
					if (!sceneCharacters.contains(character)) {
						currentData->charactersCount += 1;
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
			// Реплика
			//
			else if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Dialogue) {
				currentData->dialoguesChron += ChronometerFacade::calculate(block);
				currentData->dialoguesCount += 1;
			}

			currentData->chron += ChronometerFacade::calculate(block);
		}

		block = block.next();
	}

	//
	// Формируем данные для визуализации
	//
	QVector<double> initializedVector = QVector<double>() << 0;
	// ... х - общий для всех
	QVector<double> x = initializedVector;
	// ... y
	QVector<double> sceneChronY = initializedVector;
	QVector<double> actionChronY = initializedVector;
	QVector<double> dialogsChronY = initializedVector;
	QVector<double> charactersCountY = initializedVector;
	QVector<double> dialogsCountY = initializedVector;
	//
	const int SECONDS_IN_MINUTE = 60;
	double lastX = 0;
	QMap<double, QStringList> info;
	info.insert(lastX, QStringList());
	foreach (SceneData* data, reportScenesDataList) {
		//
		// Информация
		//
		QString infoTitle =
			QString("%1 %2")
					.arg(QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Scene"))
					.arg(data->number);
		QString infoText;
		{
			if (_parameters.storyStructureAnalisysSceneChron) {
				if (!infoText.isEmpty()) {
					infoText.append("\n");
				}
				infoText.append(QString("%1: %2").arg(::sceneChronLabel()).arg(ChronometerFacade::secondsToTime(data->chron)));
			}
			if (_parameters.storyStructureAnalisysActionChron) {
				if (!infoText.isEmpty()) {
					infoText.append("\n");
				}
				infoText.append(QString("%1: %2").arg(::actionChronLabel()).arg(ChronometerFacade::secondsToTime(data->actionChron)));
			}
			if (_parameters.storyStructureAnalisysDialoguesChron) {
				if (!infoText.isEmpty()) {
					infoText.append("\n");
				}
				infoText.append(QString("%1: %2").arg(::dialoguesChronLabel()).arg(ChronometerFacade::secondsToTime(data->dialoguesChron)));
			}
			if (_parameters.storyStructureAnalisysCharactersCount) {
				if (!infoText.isEmpty()) {
					infoText.append("\n");
				}
				infoText.append(QString("%1: %2").arg(::charactersCountLabel()).arg(data->charactersCount));
			}
			if (_parameters.storyStructureAnalisysDialoguesCount) {
				if (!infoText.isEmpty()) {
					infoText.append("\n");
				}
				infoText.append(QString("%1: %2").arg(::dialoguesCountLabel()).arg(data->dialoguesCount));
			}
		}
		info.insert(lastX, QStringList() << infoTitle << infoText);

		//
		// По иксу откладываем длительность
		//
		x << lastX + (double)data->chron / SECONDS_IN_MINUTE;
		lastX = x.last();
		//
		// Хронометраж считаем в минутах
		//
		sceneChronY << (double)data->chron / SECONDS_IN_MINUTE;
		actionChronY << (double)data->actionChron / SECONDS_IN_MINUTE;
		dialogsChronY << (double)data->dialoguesChron / SECONDS_IN_MINUTE;
		//
		// Количества как есть
		//
		charactersCountY << data->charactersCount;
		dialogsCountY << data->dialoguesCount;
	}
	info.insert(lastX, QStringList());
	//
	Plot resultPlot;
	resultPlot.info = info;
	//
	// ... хронометраж сцены
	//
	if (_parameters.storyStructureAnalisysSceneChron) {
		BusinessLogic::PlotData data;
		data.name = QApplication::translate("BusinessLogic::StoryStructureAnalisysPlot", "Scene Duration");
		data.color = QColor("#FF3030");
		data.x = x;
		data.y = sceneChronY;
		resultPlot.data.append(data);
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
		resultPlot.data.append(data);
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
		resultPlot.data.append(data);
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
		resultPlot.data.append(data);
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
		resultPlot.data.append(data);
	}

	return resultPlot;
}
