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
	 * @brief Регулярное выражение для определения блока "Время и место"
	 */
	const QRegularExpression TIME_AND_PLACE_CHECKER("(INT(.|) |EXT(.|) |ИНТ(.|) |НАТ(.|) |ПАВ(.|) |ЭКСТ(.|) )");

	/**
	 * @brief Определить тип блока в текущей позиции курсора
	 */
	static ScenarioBlockStyle::Type typeForTextCursor(const QTextCursor& _cursor) {
		const QString blockText = _cursor.block().text();

		//
		// Для всех нераспознаных блоков ставим тип "Описание действия"
		//
		ScenarioBlockStyle::Type blockType = ScenarioBlockStyle::Action;

		//
		// Блок время и место определяем по словам места действия
		//
		if (blockText.contains(TIME_AND_PLACE_CHECKER)) {
			blockType = ScenarioBlockStyle::TimeAndPlace;
		}
		//
		// Остальные блоки определяем более общим способом
		//
		else {
			const QTextBlockFormat blockFormat = _cursor.blockFormat();
			const QTextCharFormat charFormat = _cursor.charFormat();

			//
			// ... текст в верхнем регистре
			//
			bool uppercase =
					charFormat.fontCapitalization() == QFont::AllUppercase
					|| blockText == blockText.toUpper();

			if (uppercase) {
				//
				// Примечание
				//
				if (blockFormat.leftMargin() == 0) {
					blockType = ScenarioBlockStyle::Note;
				}
				//
				// Персонаж
				//
				else {
					blockType = ScenarioBlockStyle::Character;
				}
			} else {
				if (blockFormat.leftMargin() > 0) {
					//
					// Ремарка
					//
					if (blockText.startsWith("(")) {
						blockType = ScenarioBlockStyle::Parenthetical;
					}
					//
					// Реплика
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
	do {
		cursor.movePosition(QTextCursor::EndOfBlock);

		//
		// Если в блоке есть текст
		//
		if (!cursor.block().text().simplified().isEmpty()) {
			//
			// ... определяем тип
			//
			const ScenarioBlockStyle::Type blockType = ::typeForTextCursor(cursor);
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
				QRegularExpressionMatch match = TIME_AND_PLACE_CHECKER.match(blockText);
				if (match.hasMatch()) {
					blockText = blockText.mid(match.capturedStart());
				}
			}

			writer.writeCDATA(blockText);
			writer.writeEndElement();
		}


		cursor.movePosition(QTextCursor::Right);
	} while (!cursor.atEnd());

	writer.writeEndElement(); // scenario
	writer.writeEndDocument();

	return scenarioXml;
}
