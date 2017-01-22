#include "FdxExporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <QFile>
#include <QTextBlock>
#include <QTextCursor>
#include <QXmlStreamWriter>

using namespace BusinessLogic;

namespace {
	/**
	 * @brief Стиль экспорта
	 */
	static ScenarioTemplate exportStyle() {
		return ScenarioTemplateFacade::getTemplate(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"export/style",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					);
	}
}


FdxExporter::FdxExporter() :
	AbstractExporter()
{

}

void FdxExporter::exportTo(ScenarioDocument* _scenario, const ExportParameters& _exportParameters) const
{
	//
	// Открываем документ на запись
	//
	QFile docxFile(_exportParameters.filePath);
	if (docxFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		QXmlStreamWriter writer(&docxFile);
		//
		// Начало документа
		//
		writer.writeStartDocument();
		writer.writeStartElement("FinalDraft");
		writer.writeAttribute("DocumentType", "Script");
		writer.writeAttribute("Template", "No");
		writer.writeAttribute("Version", "3");
		//
		// Текст сценария
		//
		writeContent(writer, _scenario, _exportParameters);
		//
		// Параметры сценария
		//
		writeSettings(writer);
		//
		// Конец документа
		//
		writer.writeEndDocument();

		docxFile.close();
	}
}

void FdxExporter::writeContent(QXmlStreamWriter& _writer, ScenarioDocument* _scenario, const ExportParameters& _exportParameters) const
{
	_writer.writeStartElement("Content");

	//
	// Используем ненастоящие параметры экспорта, если надо, то обрабатываем их вручную
	//
	ExportParameters fakeParameters;

	//
	// Сформируем документ
	//
	QTextDocument* preparedDocument = prepareDocument(_scenario, fakeParameters);

	//
	// Данные считываются из исходного документа, определяется тип блока
	// и записываются прямо в файл
	//
	QTextCursor documentCursor(preparedDocument);
	while (!documentCursor.atEnd()) {
		if (!documentCursor.block().text().isEmpty()) {
			QString paragraphType;
			QString sceneNumber;
			switch (ScenarioBlockStyle::forBlock(documentCursor.block())) {
				case ScenarioBlockStyle::SceneHeading: {
					paragraphType = "Scene Heading";

					//
					// ... если надо, то выводим номера сцен
					//
					if (_exportParameters.printScenesNumbers) {
						if (ScenarioTextBlockInfo* sceneInfo = dynamic_cast<ScenarioTextBlockInfo*>(documentCursor.block().userData())) {
							sceneNumber = QString::number(sceneInfo->sceneNumber());
						}
					}

					break;
				}

				case ScenarioBlockStyle::SceneDescription:
				case ScenarioBlockStyle::Action:
                case ScenarioBlockStyle::TitleHeader:{
					paragraphType = "Action";
					break;
				}

				case ScenarioBlockStyle::Character: {
					paragraphType = "Character";
					break;
				}

				case ScenarioBlockStyle::Parenthetical:
				case ScenarioBlockStyle::Title: {
					paragraphType = "Parenthetical";
					break;
				}

				case ScenarioBlockStyle::Dialogue: {
					paragraphType = "Dialogue";
					break;
				}

				case ScenarioBlockStyle::Transition: {
					paragraphType = "Transition";
					break;
				}

				case ScenarioBlockStyle::Note: {
					paragraphType = "Shot";
					break;
				}

				case ScenarioBlockStyle::SceneCharacters: {
					paragraphType = "Cast List";
					break;
				}

				default: {
					paragraphType = "General";
					break;
				}
			}

			_writer.writeStartElement("Paragraph");
			if (!sceneNumber.isEmpty()) {
				_writer.writeAttribute("Number", sceneNumber);
			}
			_writer.writeAttribute("Type", paragraphType);
			_writer.writeTextElement("Text", documentCursor.block().text());
			_writer.writeEndElement(); // Paragraph
		}

		//
		// Переходим к следующему параграфу
		//
		documentCursor.movePosition(QTextCursor::EndOfBlock);
		documentCursor.movePosition(QTextCursor::NextBlock);
	}

	_writer.writeEndElement(); // Content
}

void FdxExporter::writeSettings(QXmlStreamWriter& _writer) const
{
	//
	// Информация о параметрах страницы
	//
	ScenarioTemplate exportStyle = ::exportStyle();
	QString pageHeight, pageWidth;
	switch (exportStyle.pageSizeId()) {
		case QPageSize::A4: {
			pageHeight = "11.69";
			pageWidth = "8.26";
			break;
		}

		case QPageSize::Letter: {
			pageHeight = "11.00";
			pageWidth = "8.50";
			break;
		}

		default: {
			qWarning("Unknown page size");
			break;
		}
	}

	//
	// Пишем параметры страницы
	//
	_writer.writeStartElement("PageLayout");
	_writer.writeAttribute("BackgroundColor", "#FFFFFFFFFFFF");
	_writer.writeAttribute("BottomMargin", "72"); // 1.000"
	_writer.writeAttribute("BreakDialogueAndActionAtSentences", "Yes");
	_writer.writeAttribute("DocumentLeading", "Normal");
	_writer.writeAttribute("FooterMargin", "36"); // 0.500"
	_writer.writeAttribute("ForegroundColor", "#000000000000");
	_writer.writeAttribute("HeaderMargin", "36"); // 0.500"
	_writer.writeAttribute("InvisiblesColor", "#808080808080");
	_writer.writeAttribute("TopMargin", "72"); // 1.000"
	_writer.writeAttribute("UsesSmartQuotes", "Yes");
	//
	_writer.writeEmptyElement("PageSize");
	_writer.writeAttribute("Height", pageHeight);
	_writer.writeAttribute("Width", pageWidth);
	//
	_writer.writeEmptyElement("AutoCastList");
	_writer.writeAttribute("AddParentheses", "Yes");
	_writer.writeAttribute("AutomaticallyGenerate", "Yes");
	_writer.writeAttribute("CastListElement", "Cast List");
	//
	_writer.writeEndElement(); // PageLayout
}
