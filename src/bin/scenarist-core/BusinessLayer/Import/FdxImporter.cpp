#include "FdxImporter.h"

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


FdxImporter::FdxImporter() :
	AbstractImporter()
{

}

QString FdxImporter::importScenario(const ImportParameters& _importParameters) const
{
	QString scenarioXml;

	//
	// Открываем файл
	//
	QFile fdxFile(_importParameters.filePath);
	if (fdxFile.open(QIODevice::ReadOnly)) {
		//
		// Читаем XML
		//
		QDomDocument fdxDocument;
		fdxDocument.setContent(&fdxFile);
		//
		// ... и пишем в сценарий
		//
		QXmlStreamWriter writer(&scenarioXml);
		writer.writeStartDocument();
		writer.writeStartElement(NODE_SCENARIO);
		writer.writeAttribute(ATTRIBUTE_VERSION, "1.0");

		//
		// Content - текст сценария
		//
		QDomElement rootElement = fdxDocument.documentElement();
		QDomElement content = rootElement.firstChildElement("Content");
		QDomNode paragraph = content.firstChild();
		while (!paragraph.isNull()) {
			//
			// Определим тип блока
			//
			const QString paragraphType = paragraph.attributes().namedItem("Type").nodeValue();
			ScenarioBlockStyle::Type blockType = ScenarioBlockStyle::Action;
			if (paragraphType == "Scene Heading") {
				blockType = ScenarioBlockStyle::SceneHeading;
			} else if (paragraphType == "Action") {
				blockType = ScenarioBlockStyle::Action;
			} else if (paragraphType == "Character") {
				blockType = ScenarioBlockStyle::Character;
			} else if (paragraphType == "Parenthetical") {
				blockType = ScenarioBlockStyle::Parenthetical;
			} else if (paragraphType == "Dialogue") {
				blockType = ScenarioBlockStyle::Dialogue;
			} else if (paragraphType == "Transition") {
				blockType = ScenarioBlockStyle::Transition;
			} else if (paragraphType == "Shot") {
				blockType = ScenarioBlockStyle::Note;
			} else if (paragraphType == "Cast List") {
				blockType = ScenarioBlockStyle::SceneCharacters;
			}

			//
			// Получим текст блока
			//
			const QString paragraphText = paragraph.firstChildElement("Text").text();

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
			// Переходим к следующему
			//
			paragraph = paragraph.nextSibling();
		}
	}

	return scenarioXml;
}
