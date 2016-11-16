#include "TrelbyImporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <QDomDocument>
#include <QFile>
#include <QXmlStreamWriter>

using namespace BusinessLogic;

namespace {
	/**
	 * @brief Ключи для формирования xml из импортируемого документа
	 */
	/** @{ */
	const QString NODE_SCENARIO = "scenario";
	const QString NODE_VALUE = "v";

	const QString ATTRIBUTE_VERSION = "version";
	/** @} */
}


TrelbyImporter::TrelbyImporter() :
	AbstractImporter()
{

}

QString TrelbyImporter::importScenario(const BusinessLogic::ImportParameters& _importParameters) const
{
	QString scenarioXml;

	//
	// Открываем файл
	//
	QFile trelbyFile(_importParameters.filePath);
	if (trelbyFile.open(QIODevice::ReadOnly)) {
		//
		// Читаем plain text
		//
		// ... и пишем в сценарий
		//
		QXmlStreamWriter writer(&scenarioXml);
		writer.writeStartDocument();
		writer.writeStartElement(NODE_SCENARIO);
		writer.writeAttribute(ATTRIBUTE_VERSION, "1.0");

		//
		// Текст сценария
		//
		const QStringList paragraphs = QString(trelbyFile.readAll()).split("\n");
		QString paragraphText;
		const QStringList blockChecker = {">", "&", "|", "." };
		for (const QString& paragraph : paragraphs) {
			const QString paragraphType = paragraph.left(2);

			//
			// Если строка пуста, или не является текстовым блоком, пропускаем её
			//
			if (paragraphType.isEmpty()
				|| !blockChecker.contains(paragraphType[0])) {
				continue;
			}

			//
			// Определим тип блока
			//
			ScenarioBlockStyle::Type blockType = ScenarioBlockStyle::Action;
			if (paragraphType.endsWith("\\")) {
				blockType = ScenarioBlockStyle::SceneHeading;
			} else if (paragraphType.endsWith(".")) {
				blockType = ScenarioBlockStyle::Action;
			} else if (paragraphType.endsWith("_")) {
				blockType = ScenarioBlockStyle::Character;
			} else if (paragraphType.endsWith("(")) {
				blockType = ScenarioBlockStyle::Parenthetical;
			} else if (paragraphType.endsWith(":")) {
				blockType = ScenarioBlockStyle::Dialogue;
			} else if (paragraphType.endsWith("/")) {
				blockType = ScenarioBlockStyle::Transition;
			} else if (paragraphType.endsWith("=")) {
				blockType = ScenarioBlockStyle::Note;
			} else if (paragraphType.endsWith("%")) {
				blockType = ScenarioBlockStyle::NoprintableText;
			}

			//
			// Получим текст блока
			//
			if (!paragraphText.isEmpty()) {
				paragraphText += " ";
			}
			paragraphText += paragraph.mid(2);

			//
			// Если дошли до последней строки блока
			//
			if (paragraphType.startsWith(".")) {
				//
				// Формируем блок сценария
				//
				const QString blockTypeName = ScenarioBlockStyle::typeName(blockType);
				writer.writeStartElement(blockTypeName);
				writer.writeStartElement(NODE_VALUE);
				writer.writeCDATA(paragraphText);
				writer.writeEndElement();
				writer.writeEndElement();

				//
				// И очищаем текст
				//
				paragraphText.clear();
			}
		}
	}

	return scenarioXml;
}
