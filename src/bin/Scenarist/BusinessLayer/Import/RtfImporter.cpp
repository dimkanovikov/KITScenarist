#include "RtfImporter.h"

#include <format_manager.h>
#include <format_reader.h>

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>

#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QXmlStreamWriter>

using namespace BusinessLogic;

namespace {
	/**
	 * @brief Регулярное выражение для определения блока "Время и место" по наличию слов места
	 */
	const QRegularExpression PLACE_CONTAINS_CHECKER("(^|[^\\S])(INT|EXT|ИНТ|НАТ|ПАВ|ЭКСТ)[. ]");

	/**
	 * @brief Регулярное выражение для определения блока "Время и место" по началу с номера
	 */
	const QRegularExpression START_FROM_NUMBER_CHECKER("^([\\d]{1,}[\\d\\S]{0,})[.](([\\d\\S]{1,})[.]|) ");

	/**
	 * @brief Допущение для блоков, которые по идее вообще не должны иметь отступа (миллиметры)
	 */
	const int LEFT_MARGIN_DELTA = 20;

	/**
	 * @brief Определить тип блока в текущей позиции курсора
	 *		  с указанием предыдущего типа и количества предшествующих пустых строк
	 */
	static ScenarioBlockStyle::Type typeForTextCursor(const QTextCursor& _cursor,
		ScenarioBlockStyle::Type _lastBlockType, int _prevEmptyLines, int _minLeftMargin) {
		//
		// Определим текст блока
		//
		const QString blockText = _cursor.block().text();
		const QString blockTextUppercase = blockText.toUpper();

		//
		// Для всех нераспознаных блоков ставим тип "Описание действия"
		//
		ScenarioBlockStyle::Type blockType = ScenarioBlockStyle::Action;

		//
		// Определим некоторые характеристики исследуемого текста
		//
		// ... стили блока
		const QTextBlockFormat blockFormat = _cursor.blockFormat();
		const QTextCharFormat charFormat = _cursor.charFormat();
		// ... текст в верхнем регистре (FIXME: такие строки, как "Я.")
		bool textIsUppercase =
				charFormat.fontCapitalization() == QFont::AllUppercase
				|| blockText == blockText.toUpper();
		// ... блоки находящиеся в центре
		bool isCentered = blockFormat.leftMargin() > LEFT_MARGIN_DELTA + _minLeftMargin;

		//
		// Собственно определение типа
		//
		{
			//
			// Блоки текста посередине
			//
			if (isCentered) {
				//
				// Персонаж
				// 1. В верхнем регистре
				//
				if (textIsUppercase && _lastBlockType != ScenarioBlockStyle::Character) {
					blockType = ScenarioBlockStyle::Character;
				}
				//
				// Ремарка
				// 1. начинается со скобки
				//
				else if (blockText.startsWith("(")) {
					blockType = ScenarioBlockStyle::Parenthetical;
				}
				//
				// Реплика
				// 1. всё что осталось
				//
				else {
					blockType = ScenarioBlockStyle::Dialog;
				}

			}
			//
			// Не посередине
			//
			else {
				//
				// Блоки текста в верхнем регистре
				//
				if (textIsUppercase) {
					//
					// Время и место
					// 1. текст в верхнем регистре
					// 2. содержит ключевые сокращения места действия или начинается с номера сцены
					//
					if (blockTextUppercase.contains(PLACE_CONTAINS_CHECKER)
						|| blockTextUppercase.contains(START_FROM_NUMBER_CHECKER)) {
						blockType = ScenarioBlockStyle::TimeAndPlace;
					}
					//
					// Участника сцены
					// 1. в верхнем регистре
					// 2. идут сразу же после времени и места
					//
					else if (_lastBlockType == ScenarioBlockStyle::TimeAndPlace
							 && _prevEmptyLines == 0) {
						blockType = ScenarioBlockStyle::SceneCharacters;
					}
					//
					// Примечание
					// 1. всё что осталось и не имеет отступов
					// 2. выровнено по левому краю
					//
					else if (blockFormat.alignment() == Qt::AlignLeft
							 && !isCentered) {
						blockType = ScenarioBlockStyle::Note;
					}
					//
					// Переход
					// 1. всё что осталось и выровнено по правому краю
					//
					else if (blockFormat.alignment() == Qt::AlignRight) {
						blockType = ScenarioBlockStyle::Transition;
					}
				}
			}
		}

		return blockType;
	}
}


RtfImporter::RtfImporter()
{
}

QString RtfImporter::importScenario(const ImportParameters& _importParameters) const
{
	//
	// Преобразовать заданный документ в QTextDocument
	//
	QTextDocument rtfDocument;
	QFile rtfFile(_importParameters.filePath);
	rtfFile.open(QIODevice::ReadOnly);
	FormatReader* reader = FormatManager::createReader(&rtfFile);
	reader->read(&rtfFile, &rtfDocument);

	//
	// Найти минимальный отступ слева для всех блоков
	// ЗАЧЕМ: во многих программах (Final Draft, Screeviner) сделано так, что поля
	//		  задаются за счёт оступов. Получается что и заглавие сцены и описание действия
	//		  имеют отступы. Так вот это и будет минимальным отступом, который не будем считать
	//
	int minLeftMargin = 1000;
	{
		QTextCursor cursor(&rtfDocument);
		while (!cursor.atEnd()) {
			if (minLeftMargin > cursor.blockFormat().leftMargin()) {
				minLeftMargin = cursor.blockFormat().leftMargin();
			}

			cursor.movePosition(QTextCursor::NextBlock);
			cursor.movePosition(QTextCursor::EndOfBlock);
		}
	}

	//
	// Преобразовать его в xml-строку
	//
	QString scenarioXml;
	QTextCursor cursor(&rtfDocument);

	QXmlStreamWriter writer(&scenarioXml);
	writer.writeStartDocument();
	writer.writeStartElement("scenario");

	//
	// Для каждого блока текста определяем тип
	//
	// ... последний стиль блока
	ScenarioBlockStyle::Type lastBlockType = ScenarioBlockStyle::Undefined;
	// ... количество пустых строк
	int emptyLines = 0;
	do {
		cursor.movePosition(QTextCursor::EndOfBlock);

		//
		// Если в блоке есть текст
		//
		if (!cursor.block().text().simplified().isEmpty()) {
			//
			// ... определяем тип
			//
			const ScenarioBlockStyle::Type blockType =
					::typeForTextCursor(cursor, lastBlockType, emptyLines, minLeftMargin);
			const QString blockTypeName = ScenarioBlockStyle::typeName(blockType);
			QString blockText = cursor.block().text();

			//
			// ... запишем данные в строку
			//
			writer.writeStartElement(blockTypeName);

			//
			// Если текущий тип "Время и место" и нужно удалить номер сцены, то делаем это
			//
			if (blockType == ScenarioBlockStyle::TimeAndPlace
				&& _importParameters.removeScenesNumbers){
				blockText = blockText.toUpper();
				QRegularExpressionMatch match = START_FROM_NUMBER_CHECKER.match(blockText);
				if (match.hasMatch()) {
					blockText = blockText.mid(match.capturedEnd());
				}
			}

			writer.writeCDATA(blockText);
			writer.writeEndElement();

			//
			// Запомним последний стиль блока и обнулим счётчик пустых строк
			//
			lastBlockType = blockType;
			emptyLines = 0;
		}
		//
		// Если в блоке нет текста, то увеличиваем счётчик пустых строк
		//
		else {
			++emptyLines;
		}

		cursor.movePosition(QTextCursor::Right);
	} while (!cursor.atEnd());

	writer.writeEndElement(); // scenario
	writer.writeEndDocument();

	return scenarioXml;
}
