#include "RtfImporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>

#include <rtfreader.h>
#include <TextDocumentRtfOutput.h>

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
	const QRegularExpression PLACE_CONTAINS_CHECKER("(INT(.| )|EXT(.| )|ИНТ(.| )|НАТ(.| )|ПАВ(.| )|ЭКСТ(.| ))");

	/**
	 * @brief Регулярное выражение для определения блока "Время и место" по началу с номера
	 */
	const QRegularExpression START_FROM_NUMBER_CHECKER("^(\\d{1,})[.] ");

	/**
	 * @brief Определить тип блока в текущей позиции курсора
	 *		  с указанием предыдущего типа и количества предшествующих пустых строк
	 */
	static ScenarioBlockStyle::Type typeForTextCursor(const QTextCursor& _cursor,
		ScenarioBlockStyle::Type _lastBlockType, int _prevEmptyLines) {
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
		// ... текст в верхнем регистре (5 - минимальное количество знаков в строке,
		//     иначе при таком сравнении в верхнем регистре будут считаться такие строки, как "Я."
		bool textIsUppercase =
				charFormat.fontCapitalization() == QFont::AllUppercase
				|| (blockText.size() > 5 && blockText == blockText.toUpper());

		//
		// Собственно определение типа
		//
		{
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
				else if (blockFormat.leftMargin() == 0
						 && blockFormat.alignment() == Qt::AlignLeft) {
					blockType = ScenarioBlockStyle::Note;
				}
				//
				// Переход
				// 1. всё что осталось и выровнено по правому краю
				//
				else if (blockFormat.alignment() == Qt::AlignRight) {
					blockType = ScenarioBlockStyle::Transition;
				}
				//
				// Персонаж
				// 1. всё что осталось
				//
				else {
					blockType = ScenarioBlockStyle::Character;
				}
			}
			//
			// Блоки текста не в верхнем регистре
			//
			else {
				if (blockFormat.leftMargin() > 0) {
					//
					// Ремарка
					// 1. имеет отступ слева
					// 2. начинается со скобки
					//
					if (blockText.startsWith("(")) {
						blockType = ScenarioBlockStyle::Parenthetical;
					}
					//
					// Реплика
					// 1. имеет отступ слева
					//
					else {
						blockType = ScenarioBlockStyle::Dialog;
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
	RtfReader::TextDocumentRtfOutput output(&rtfDocument);
	RtfReader::Reader reader;
	reader.open(_importParameters.filePath);
	reader.parseTo(&output);
	reader.close();

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
			const ScenarioBlockStyle::Type blockType = ::typeForTextCursor(cursor, lastBlockType, emptyLines);
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
