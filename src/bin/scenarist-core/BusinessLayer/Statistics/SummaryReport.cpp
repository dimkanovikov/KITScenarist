#include "SummaryReport.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>
#include <BusinessLayer/Chronometry/ChronometerFacade.h>
#include <BusinessLayer/Counters/CountersFacade.h>
#include <BusinessLayer/Counters/Counter.h>

#include <Domain/Character.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/CharacterStorage.h>

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

	/**
	 * @brief Сформировать линию графика
	 */
	static QString makeBar(int _count, int _max, int _index) {
		QString color;
		while (_index > 8) {
			_index = _index % 8;
		}
		switch (_index) {
			default:
			case 0: color = "#b1e186"; break;
			case 1: color = "#2ba121"; break;
			case 2: color = "#fc9998"; break;
			case 3: color = "#d84d4b"; break;
			case 4: color = "#fdc069"; break;
			case 5: color = "#fe8200"; break;
			case 6: color = "#69c6ff"; break;
			case 7: color = "#4cb3d2"; break;
			case 8: color = "#88abad"; break;
		}

		const float widthPercent = _count == 0 ? 0.1 : ((float)_count * 100. / (float)_max);
		const QString percentText =
				QString("%1%")
				.arg(_count == 0
					 ? "0"
					 : QString::number(widthPercent, 'f', (widthPercent < 1) ? 1 : 0));

		return QString("<table width=\"100%\"><tr><td width=\"%1%\" bgcolor=\"%2\"></td><td>&nbsp;%3</td></tr></table>")
				.arg(widthPercent).arg(color).arg(percentText);
	}
}


QString SummaryReport::reportName(const BusinessLogic::ReportParameters&) const
{
	return QApplication::translate("BusinessLogic::SummaryReport", "Summary report");
}

QString SummaryReport::makeReport(QTextDocument* _scenario, const BusinessLogic::ReportParameters& _parameters) const
{
	//
	// Бежим по документу и собираем статистику
	//
	// - блок - вхождений - слов
	QStringList blockNames;
	const bool BEAUTIFY_NAME = true;
	blockNames << ScenarioBlockStyle::typeName(ScenarioBlockStyle::SceneHeading, BEAUTIFY_NAME)
			   << ScenarioBlockStyle::typeName(ScenarioBlockStyle::SceneCharacters, BEAUTIFY_NAME)
			   << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Action, BEAUTIFY_NAME)
			   << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Character, BEAUTIFY_NAME)
			   << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Parenthetical, BEAUTIFY_NAME)
			   << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Dialogue, BEAUTIFY_NAME)
			   << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Transition, BEAUTIFY_NAME)
			   << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Note, BEAUTIFY_NAME)
			   << ScenarioBlockStyle::typeName(ScenarioBlockStyle::Title, BEAUTIFY_NAME);
	QMap<QString, QPair<int, int> > blockCounters;
	foreach (const QString& blockName, blockNames) {
		blockCounters.insert(blockName, QPair<int, int>(0, 0));
	}
	// - список сцен
	QStringList scenes;
	// - персонаж - кол-во реплик
	QMap<QString, int> characters;
	foreach (DomainObject* characterObject, DataStorageLayer::StorageFacade::characterStorage()->all()->toList()) {
		Character* character = dynamic_cast<Character*>(characterObject);
		characters.insert(character->name(), 0);
	}
	//
	// ... побежали
	//
	QTextBlock block = _scenario->begin();
	QString lastCharacter;
	while (block.isValid()) {
		if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::SceneHeading) {
			scenes.append(block.text().toUpper());
		} else if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Character) {
			lastCharacter = CharacterParser::name(block.text().toUpper());
		} else if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::Dialogue) {
			if (!characters.contains(lastCharacter)) {
				characters.insert(lastCharacter, 0);
			}
			characters[lastCharacter] += 1;
		}
		//
		const QString blockName = ScenarioBlockStyle::typeName(ScenarioBlockStyle::forBlock(block), BEAUTIFY_NAME);
		if (blockNames.contains(blockName)) {
			blockCounters[blockName].first += 1;
			blockCounters[blockName].second += CountersFacade::calculateFull(block).words();
		}

		block = block.next();
	}

	//
	// Формируем отчёт
	//
	QString html;

	if (_parameters.summaryText) {
		//
		// Статистика по текстовой состовляющей
		//
		PagesTextEdit edit;
		edit.setUsePageMode(true);
		edit.setPageFormat(::editorStyle().pageSizeId());
		edit.setPageMargins(::editorStyle().pageMargins());
		edit.setDocument(_scenario->clone());

		const int chron = ChronometerFacade::calculate(_scenario);
		const int pageCount = edit.document()->pageCount();
		const Counter counter = CountersFacade::calculateFull(_scenario);

		html.append("<table width=\"100%\">");
		html.append("<tr>");
		html.append(QString("<td align=\"center\" width=\"22%\"><h2>%1</h2></td>")
					.arg(ChronometerFacade::secondsToTime(chron)));
		html.append(QString("<td align=\"center\" width=\"20%\"><h2>%1</h2></td>").arg(pageCount));
		html.append(QString("<td align=\"center\" width=\"22%\"><h2>%1</h2></td>").arg(counter.words()));
		html.append(QString("<td align=\"center\" width=\"36%\"><h2>%1/%2</h2></td>")
					.arg(counter.charactersWithSpaces()).arg(counter.charactersWithoutSpaces()));
		html.append("</tr>");
		html.append("<tr>");
		html.append(QString("<td align=\"center\">%1</td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Chronometry")));
		html.append(QString("<td align=\"center\">%1</td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Pages")));
		html.append(QString("<td align=\"center\">%1</td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Words")));
		html.append(QString("<td align=\"center\">%1</td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Characters with/without spaces")));
		html.append("</tr>");
		html.append("</table>");
		html.append("<hr width=\"100%\"></hr>");

		//
		// Соотношение блоков
		//
		// ... заголовок
		//
		html.append(QString("<h3>%1</h3>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Text")));
		html.append("<table width=\"100%\">");
		html.append("<tr>");
		html.append(QString("<td><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Paragraph")));
		html.append(QString("<td align=\"center\"><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Size")));
		html.append(QString("<td align=\"center\"><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Words")));
		html.append(QString("<td><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Percents")));
		html.append("</tr>");
		int wordsMax = 0;
		foreach (const QString& blockName, blockNames) {
			wordsMax += blockCounters.value(blockName).second;
		}
		//
		// ... тело
		//
		foreach (const QString& blockName, blockNames) {
			html.append("<tr>");
			html.append(QString("<td>%1</td><td align=\"center\">%2</td><td align=\"center\">%3</td><td width=\"50%\">%4</td>")
						.arg(blockName)
						.arg(blockCounters.value(blockName).first)
						.arg(blockCounters.value(blockName).second)
						.arg(::makeBar(blockCounters.value(blockName).second, wordsMax, blockNames.indexOf(blockName))));

			html.append("</tr>");
		}
		html.append("</table>");
	}

	if (_parameters.summaryScenes) {
		//
		// Соотношение сцен по времени действия
		//
		html.append(QString("<h3>%1</h3>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Scenes")));
		QMap<QString, int> sceneTimes;
		foreach (const QString& scene, scenes) {
			const QString time = SceneHeadingParser::timeName(scene);
			if (!sceneTimes.contains(time)) {
				sceneTimes.insert(time, 0);
			}
			sceneTimes[time] += 1;
		}

		//
		// Сортируем по наибольшему вхождению и удаляем дубликаты
		//
		QList<int> counts = QSet<int>::fromList(sceneTimes.values()).toList();
		qSort(counts.begin(), counts.end(), qGreater<int>());

		//
		// Формируем таблицу
		//
		// ... заголовок
		//
		html.append("<table width=\"100%\">");
		html.append("<tr>");
		html.append(QString("<td><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Time")));
		html.append(QString("<td align=\"center\"><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Size")));
		html.append(QString("<td><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Percents")));
		html.append("</tr>");
		//
		// ... тело
		//
		int maxCount = 0;
		foreach (const int count, sceneTimes.values()) {
			maxCount += count;
		}
		int index = 0;
		foreach (int count, counts) {
			foreach (const QString& time, sceneTimes.keys(count)) {
				QString timeName = time.isEmpty()
								   ? QApplication::translate("BusinessLogic::SummaryReport", "[UNDEFINED]")
								   : time;
				html.append("<tr>");
				html.append(QString("<td>%1</td><td align=\"center\" width=\"10%\">%2</td><td width=\"60%\">%3</td>")
							.arg(timeName)
							.arg(count)
							.arg(::makeBar(count, maxCount, index++)));

				html.append("</tr>");
			}
		}
		html.append("</table>");
	}

	if (_parameters.summaryLocations) {
		//
		// Соотношение локаций по типу
		//
		html.append(QString("<h3>%1</h3>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Locations")));
		QMap<QString, int> locationPlaces;
		foreach (const QString& scene, scenes) {
			const QString place = SceneHeadingParser::placeName(scene);
			if (!locationPlaces.contains(place)) {
				locationPlaces.insert(place, 0);
			}
			locationPlaces[place] += 1;
		}

		//
		// Сортируем по наибольшему вхождению и удаляем дубликаты
		//
		QList<int> counts = QSet<int>::fromList(locationPlaces.values()).toList();
		qSort(counts.begin(), counts.end(), qGreater<int>());

		//
		// Формируем таблицу
		//
		// ... заголовок
		//
		html.append("<table width=\"100%\">");
		html.append("<tr>");
		html.append(QString("<td><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Place")));
		html.append(QString("<td align=\"center\"><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Size")));
		html.append(QString("<td><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Percents")));
		html.append("</tr>");
		//
		// ... тело
		//
		int maxCount = 0;
		foreach (const int count, locationPlaces.values()) {
			maxCount += count;
		}
		int index = 4;
		foreach (int count, counts) {
			foreach (const QString& time, locationPlaces.keys(count)) {
				html.append("<tr>");
				html.append(QString("<td>%1</td><td align=\"center\" width=\"10%\">%2</td><td width=\"60%\">%3</td>")
							.arg(time)
							.arg(count)
							.arg(::makeBar(count, maxCount, index++)));

				html.append("</tr>");
			}
		}
		html.append("</table>");
	}

	if (_parameters.summaryCharacters) {
		//
		// Соотношение персонажей по кол-ву реплик
		//
		html.append(QString("<h3>%1</h3>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Characters")));

		//
		// Сортируем по наибольшему вхождению и удаляем дубликаты
		//
		QList<int> counts = QSet<int>::fromList(characters.values()).toList();
		qSort(counts.begin(), counts.end(), qGreater<int>());

		//
		// Группируем персонажей
		//
		int nonspeaking = 0, speakAbout10 = 0, speakMore10 = 0;
		foreach (const QString& character, characters.keys()) {
			const int speaks = characters.value(character);
			if (speaks == 0) {
				nonspeaking += 1;
			} else if (speaks <= 10) {
				speakAbout10 += 1;
			} else {
				speakMore10 += 1;
			}
		}

		//
		// Формируем таблицу
		//
		// ... заголовок
		//
		html.append("<table width=\"100%\">");
		html.append("<tr>");
		html.append(QString("<td><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Dialogues count")));
		html.append(QString("<td align=\"center\"><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Size")));
		html.append(QString("<td><u>%1</u></td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Percents")));
		html.append("</tr>");
		//
		// ... тело
		//
		int maxCount = nonspeaking + speakAbout10 + speakMore10;
		int index = 0;
		// ... болтуны
		html.append("<tr>");
		html.append(QString("<td>%1</td><td align=\"center\" width=\"10%\">%2</td><td width=\"60%\">%3</td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "More 10 dialogues"))
					.arg(speakMore10)
					.arg(::makeBar(speakMore10, maxCount, index++)));
		html.append("</tr>");
		// ... мало говорящие
		html.append("<tr>");
		html.append(QString("<td>%1</td><td align=\"center\" width=\"10%\">%2</td><td width=\"60%\">%3</td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "About 10 dialogues"))
					.arg(speakAbout10)
					.arg(::makeBar(speakAbout10, maxCount, index++)));
		html.append("</tr>");
		// ... молчаливые
		html.append("<tr>");
		html.append(QString("<td>%1</td><td align=\"center\" width=\"10%\">%2</td><td width=\"60%\">%3</td>")
					.arg(QApplication::translate("BusinessLogic::SummaryReport", "Nonspeaking"))
					.arg(nonspeaking)
					.arg(::makeBar(nonspeaking, maxCount, index++)));
		html.append("</tr>");
		//
		html.append("</table>");
	}

	return html;
}
