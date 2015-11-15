#include "CharactersActivityPlot.h"

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
	 * @brief Цвет для графика по персонажу
	 *		  Пробуем получить неповторяющие пастельные цвета
	 */
	static QColor characterColor(int _index) {
		int h = _index * 50;
		while (h > 359) {
			h -= 359;
		}
		int s = 255 - _index;
		int v = 255;
		return QColor::fromHsv(h, s, v);
	}
}


QString CharactersActivityPlot::plotName(const BusinessLogic::StatisticsParameters& _parameters) const
{
	Q_UNUSED(_parameters);
	return QApplication::translate("BusinessLogic::CharactersActivityPlot", "Characters Activity Plot");
}

Plot CharactersActivityPlot::makePlot(QTextDocument* _scenario, const BusinessLogic::StatisticsParameters& _parameters) const
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
	QList<SceneData*> scenesDataList;
	SceneData* currentData = 0;
	QStringList characters;
	while (block.isValid()) {
		if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::SceneHeading) {
			currentData = new SceneData;
			scenesDataList.append(currentData);
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
					currentData->characters.last().dialoguesCount += 1;
				}
				//
				// Повторное появление
				//
				else {
					int characterIndex = currentData->characterIndex(character);
					if (characterIndex == -1) {
						currentData->characters.append(SceneCharacter(character));
						currentData->characters.last().isFirstOccurence = false;
						currentData->characters.last().dialoguesCount += 1;
					} else {
						currentData->characters[characterIndex].dialoguesCount += 1;
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
	// Формируем данные для визуализации
	//
	QVector<double> initializedVector = QVector<double>() << 0;
	// ... х - общий для всех
	QVector<double> x = initializedVector;
	// ... y
	QList<QVector<double> > charactersY;
	for (int characterIndex = 0; characterIndex < characters.size(); ++characterIndex) {
		charactersY.append(initializedVector);
	}
	//
	const int SECONDS_IN_MINUTE = 60;
	double lastX = 0;
	QMap<double, QStringList> info;
	info.insert(lastX, QStringList());
	foreach (SceneData* data, scenesDataList) {
		//
		// Информация
		//
		QString infoTitle =
			QString("%1 %2")
					.arg(QApplication::translate("BusinessLogic::CharactersActivityPlot", "Scene"))
					.arg(data->number);
		QString infoText;
		{
			if (!data->characters.isEmpty()) {
				infoText = QApplication::translate("BusinessLogic::CharactersActivityPlot", "Dialogues count:");
			}
			foreach (const SceneCharacter& character, data->characters) {
				if (_parameters.charactersActivityNames.contains(character.name)) {
					if (!infoText.isEmpty()) {
						infoText.append("\n");
					}
					infoText.append(QString("%1: %2").arg(character.name).arg(character.dialoguesCount));
				}
			}
		}
		info.insert(lastX, QStringList() << infoTitle << infoText);

		//
		// По иксу откладываем длительность
		//
		x << lastX + 1. / SECONDS_IN_MINUTE;
		x << lastX + (double)data->chron / SECONDS_IN_MINUTE;
		lastX = x.last();
		//
		// По игрику активность персонажей
		//
		// Позицию по игрику наращиваем, потому что мы будем строить график из горбов
		//
		int lastY = 0;
		for (int characterIndex = 0; characterIndex < characters.size(); ++characterIndex) {
			if (_parameters.charactersActivityNames.contains(characters.at(characterIndex))) {
				lastY += 1;
				double currentY = std::numeric_limits<double>::quiet_NaN();
				int dataCharacterIndex = data->characterIndex(characters.at(characterIndex));
				if (dataCharacterIndex != -1) {
					currentY = lastY;
				}
				charactersY[characterIndex] << currentY << currentY;
			}
		}
	}
	info.insert(lastX, QStringList());
	//
	Plot resultPlot;
	resultPlot.info = info;
	//
	// ... Формируем список графиков снизу вверх, чтоби они не закрашивались при выводе
	//
	for (int characterIndex = characters.size() - 1; characterIndex >= 0; --characterIndex) {
		if (_parameters.charactersActivityNames.contains(characters.at(characterIndex))) {
			BusinessLogic::PlotData data;
			data.name = characters.at(characterIndex);
			data.color = ::characterColor(characterIndex);
			data.x = x;
			data.y = charactersY.at(characterIndex);
			resultPlot.data.append(data);
		}
	}

	//
	// Очищаем память
	//
	qDeleteAll(scenesDataList);

	return resultPlot;
}
