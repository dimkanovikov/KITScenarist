#include "DocxExporter.h"

#include "qtzip/QtZipWriter"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>

#include <Domain/Scenario.h>

#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QFile>
#include <QChar>

using namespace BusinessLogic;

namespace {
	/**
	 * @brief Стиль экспорта
	 */
	static ScenarioStyle exportStyle() {
		return ScenarioStyleFacade::style(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"export/style",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					);
	}

	/**
	 * @brief Перевести миллиметры в твипсы (мера длины в формате RTF)
	 */
	static int mmToTwips(qreal _mm) {
		return 56.692913386 * _mm;
	}

	/**
	 * @brief Пронумерованный список типов блоков
	 */
	static QMap<int, ScenarioBlockStyle::Type> blockTypes() {
		static QMap<int, ScenarioBlockStyle::Type> s_types;
		if (s_types.isEmpty()) {
			s_types.insert(0, ScenarioBlockStyle::Undefined);
			s_types.insert(1, ScenarioBlockStyle::TimeAndPlace);
			s_types.insert(2, ScenarioBlockStyle::SceneCharacters);
			s_types.insert(3, ScenarioBlockStyle::Action);
			s_types.insert(4, ScenarioBlockStyle::Character);
			s_types.insert(5, ScenarioBlockStyle::Parenthetical);
			s_types.insert(6, ScenarioBlockStyle::Dialog);
			s_types.insert(7, ScenarioBlockStyle::Transition);
			s_types.insert(8, ScenarioBlockStyle::Note);
			s_types.insert(9, ScenarioBlockStyle::TitleHeader);
			s_types.insert(10, ScenarioBlockStyle::Title);
			s_types.insert(11, ScenarioBlockStyle::SceneGroupHeader);
			s_types.insert(12, ScenarioBlockStyle::SceneGroupFooter);
		}
		return s_types;
	}

	/**
	 * @brief Сформировать строку DOCX-стиля из стиля блока
	 */
	static QString docxBlockStyle(const ScenarioBlockStyle& _style) {
		QString blockStyle;

		if (_style.type() != ScenarioBlockStyle::Undefined) {
			//
			// Стиль
			//
			blockStyle.append(
				QString("<w:style w:type=\"paragraph\" w:styleId=\"%1\">")
				.arg(ScenarioBlockStyle::typeName(_style.type()).toUpper().replace("_", ""))
				);

			//
			// Наименование
			//
			blockStyle.append(
				QString("<w:name w:val=\"%1\"/>")
				.arg(ScenarioBlockStyle::typeName(_style.type()).toUpper().replace("_", " "))
				);

			//
			// Свойства
			//
			blockStyle.append("<w:pPr>");
			//
			// ... разрешаем перенос абзацев между страницами и висячие строки
			//
			blockStyle.append("<w:widowControl w:val=\"0\"/><w:autoSpaceDE w:val=\"0\"/><w:autoSpaceDN w:val=\"0\"/><w:adjustRightInd w:val=\"0\"/><w:spacing w:after=\"0\" w:line=\"240\" w:lineRule=\"auto\"/>");
			//
			// ... отступы
			//
			blockStyle.append(
				QString("<w:ind w:left=\"%1\" w:right=\"%2\"/>")
				.arg(::mmToTwips(_style.leftMargin()))
				.arg(::mmToTwips(_style.rightMargin()))
				);
			//
			// ... выравнивание
			//
			blockStyle.append("<w:jc w:val=\"");
			switch (_style.blockFormat().alignment()) {
				case Qt::AlignRight: {
					blockStyle.append("right");
					break;
				}

				case Qt::AlignCenter:
				case Qt::AlignHCenter: {
					blockStyle.append("center");
					break;
				}

				case Qt::AlignJustify: {
					blockStyle.append("both");
					break;
				}

				default: {
					blockStyle.append("left");
					break;
				}
			}
			blockStyle.append("\"/>");
			//
			// ... конец свойств
			//
			blockStyle.append("</w:pPr>");

			//
			// Параметры шрифта
			//
			blockStyle.append("<w:rPr>");
			blockStyle.append(
				QString("<w:rFonts w:ascii=\"%1\" w:hAnsi=\"%1\" w:cs=\"%1\"/>")
				.arg(_style.font().family())
				);
			//
			// ... размер
			//
			blockStyle.append(
				QString("<w:sz w:val=\"%1\"/><w:szCs w:val=\"%1\"/>")
				.arg(_style.font().pointSize() * 2)
				);
			//
			// ... начертание
			//
			if (_style.font().bold()) {
				blockStyle.append("<w:b/><w:bCs/>");
			}
			if (_style.font().italic()) {
				blockStyle.append("<w:i/><w:iCs/>");
			}
			if (_style.font().underline()) {
				blockStyle.append("<w:u  w:val=\"single\"/>");
			}
			//
			// ... регистр
			//
			if (_style.font().capitalization() == QFont::AllUppercase) {
				blockStyle.append("<w:caps/>");
			}
			//
			// ... конец параметров шрифта
			//
			blockStyle.append("</w:rPr>");

			//
			// Конец стиля
			//
			blockStyle.append("</w:style>");
		} else {
			//
			// Для неопределённого стиля формируется простая заглушка
			//
			blockStyle =
					"<w:style w:type=\"paragraph\" w:styleId=\"Normal\">"
						"<w:name w:val=\"Normal\"/>"
						"<w:pPr>"
							"<w:widowControl w:val=\"0\"/><w:autoSpaceDE w:val=\"0\"/><w:autoSpaceDN w:val=\"0\"/><w:adjustRightInd w:val=\"0\"/><w:spacing w:after=\"0\" w:line=\"240\" w:lineRule=\"auto\"/>"
						"</w:pPr>"
						"<w:rPr>"
							"<w:rFonts w:ascii=\"Courier New\" w:hAnsi=\"Courier New\" w:cs=\"Courier New\"/>"
							"<w:sz w:val=\"24\"/>"
						"</w:rPr>"
					"</w:style>";
		}

		return blockStyle;
	}

	/**
	 * @brief Сформировать текст блока документа в зависимости от его стиля и оформления
	 */
	static QString docxText(const QTextCursor& _cursor) {
		QString documentXml;

		//
		// Получим стиль параграфа
		//
		ScenarioBlockStyle::Type currentBlockType =
				(ScenarioBlockStyle::Type)_cursor.blockFormat().property(ScenarioBlockStyle::PropertyType).toInt();

		//
		// Запишем параграф в документ
		//
		if (currentBlockType != ScenarioBlockStyle::Undefined) {
			documentXml =
				QString("<w:p><w:pPr><w:pStyle w:val=\"%1\"/><w:rPr/></w:pPr><w:r><w:rPr/><w:t>%2</w:t></w:r></w:p>")
				.arg(ScenarioBlockStyle::typeName(currentBlockType).toUpper().replace("_", ""))
				.arg(_cursor.block().text());
		} else {
			documentXml = "<w:p><w:pPr><w:pStyle w:val=\"Normal\"/>";
			switch (_cursor.blockFormat().alignment()) {
				case Qt::AlignCenter:
				case Qt::AlignHCenter: {
					documentXml.append("<w:jc w:val=\"center\"/>");
					break;
				}

				case Qt::AlignRight: {
					documentXml.append("<w:jc w:val=\"right\"/>");
					break;
				}

				case Qt::AlignJustify: {
					documentXml.append("<w:jc w:val=\"both\"/>");
					break;
				}

				default: {
					break;
				}
			}
			documentXml.append(
				QString("<w:rPr/></w:pPr><w:r><w:rPr/><w:t>%2</w:t></w:r></w:p>")
				.arg(_cursor.block().text())
				);
		}

		return documentXml;
	}
}

DocxExporter::DocxExporter()
{
}

void DocxExporter::exportTo(ScenarioDocument* _scenario, const ExportParameters& _exportParameters) const
{
	//
	// Открываем документ на запись
	//
	QFile docxFile(_exportParameters.filePath);
	if (docxFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		//
		// Пишем все статичные данные в файл
		//
		QtZipWriter zip(&docxFile);
		if (zip.status() == QtZipWriter::NoError) {
			//
			// Конвертируем документ в DOCX
			//
			// ... статичные данные
			//
			writeStaticData(&zip, _exportParameters);
			//
			// ... стили
			//
			writeStyles(&zip);
			//
			// ... колонтитулы
			//
			writeHeader(&zip, _exportParameters);
			writeFooter(&zip, _exportParameters);
			//
			// ... документ
			//
			writeDocument(&zip, _scenario, _exportParameters);
		}
		zip.close();
		docxFile.close();
	}
}

void DocxExporter::writeStaticData(QtZipWriter* _zip, const ExportParameters& _exportParameters) const
{
	//
	// Перечисление всех компонентов архива
	//
	QString contentTypesXml =
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
			"<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
			"<Override PartName=\"/_rels/.rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
			"<Override PartName=\"/word/_rels/document.xml.rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
			"<Override PartName=\"/word/styles.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml\"/>";
	//
	// ... необходимы ли колонтитулы
	//
	if (_exportParameters.printPagesNumbers) {
		if (::exportStyle().numberingAlignment().testFlag(Qt::AlignTop)) {
			contentTypesXml.append("<Override PartName=\"/word/header1.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.header+xml\"/>");
		} else {
			contentTypesXml.append("<Override PartName=\"/word/footer1.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.footer+xml\"/>");
		}
	}
	contentTypesXml.append(
			"<Override PartName=\"/word/document.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml\"/>"
			"<Override PartName=\"/word/settings.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.wordprocessingml.settings+xml\"/>"
			"</Types>");
	_zip->addFile(QString::fromLatin1("[Content_Types].xml"), contentTypesXml.toUtf8());

	//
	// Связи пакета
	//
	_zip->addFile(QString::fromLatin1("_rels/.rels"),
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
		"<Relationship Target=\"word/document.xml\" Id=\"pkgRId0\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\"/>"
		"</Relationships>");

	//
	// Связи документа
	//
	QString documentXmlRels =
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
			"<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
			"<Relationship Id=\"docRId0\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/>";
	//
	// ... необходимы ли колонтитулы
	//
	if (_exportParameters.printPagesNumbers) {
		if (::exportStyle().numberingAlignment().testFlag(Qt::AlignTop)) {
			documentXmlRels.append("<Relationship Id=\"docRId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/header\" Target=\"header1.xml\"/>");
		} else {
			documentXmlRels.append("<Relationship Id=\"docRId2\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/footer\" Target=\"footer1.xml\"/>");
		}
	}
	documentXmlRels.append(
		"<Relationship Id=\"docRId3\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/settings\" Target=\"settings.xml\"/>"
		"</Relationships>");
	_zip->addFile(QString::fromLatin1("word/_rels/document.xml.rels"), documentXmlRels.toUtf8());

	//
	// Настройки документа
	//
	_zip->addFile(QString::fromLatin1("word/settings.xml"),
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
		"<w:settings xmlns:o=\"urn:schemas-microsoft-com:office:office\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:m=\"http://schemas.openxmlformats.org/officeDocument/2006/math\" xmlns:v=\"urn:schemas-microsoft-com:vml\" xmlns:w10=\"urn:schemas-microsoft-com:office:word\" xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" xmlns:sl=\"http://schemas.openxmlformats.org/schemaLibrary/2006/main\"><w:characterSpacingControl w:val=\"compressPunctuation\"/></w:settings>");
}

void DocxExporter::writeStyles(QtZipWriter* _zip) const
{
	//
	// Сформируем xml стилей
	//
	QString styleXml =
			"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
			"<w:styles xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\">";

	//
	// Настройки в соответсвии со стилем
	//
	ScenarioStyle style = ::exportStyle();
	foreach (int blockNumber, ::blockTypes().keys()) {
		ScenarioBlockStyle blockStyle = style.blockStyle(::blockTypes().value(blockNumber));
		styleXml.append(::docxBlockStyle(blockStyle));
	}

	styleXml.append("</w:styles>");

	//
	// Запишем стили в архив
	//
	_zip->addFile(QString::fromLatin1("word/styles.xml"), styleXml.toUtf8());
}

void DocxExporter::writeHeader(QtZipWriter* _zip, const ExportParameters& _exportParameters) const
{
	//
	// Если нужна нумерация вверху
	//
	if (_exportParameters.printPagesNumbers
		&& ::exportStyle().numberingAlignment().testFlag(Qt::AlignTop)) {
		QString headerXml =
				"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
				"<w:hdr xmlns:o=\"urn:schemas-microsoft-com:office:office\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:v=\"urn:schemas-microsoft-com:vml\" xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" xmlns:w10=\"urn:schemas-microsoft-com:office:word\" xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\">";


		headerXml.append("<w:p><w:pPr><w:jc w:val=\"");
		if (::exportStyle().numberingAlignment().testFlag(Qt::AlignLeft)) {
			headerXml.append("left");
		} else if (::exportStyle().numberingAlignment().testFlag(Qt::AlignCenter)) {
			headerXml.append("center");
		} else {
			headerXml.append("right");
		}
		headerXml.append("\"/><w:rPr/></w:pPr><w:r><w:rPr/><w:fldChar w:fldCharType=\"begin\"/></w:r><w:r><w:instrText> PAGE </w:instrText></w:r><w:r><w:fldChar w:fldCharType=\"separate\"/></w:r><w:r><w:t>0</w:t></w:r><w:r><w:fldChar w:fldCharType=\"end\"/></w:r></w:p>");
		headerXml.append("</w:hdr>");

		//
		// Запишем верхний колонтитул в архив
		//
		_zip->addFile(QString::fromLatin1("word/header1.xml"), headerXml.toUtf8());
	}
}

void DocxExporter::writeFooter(QtZipWriter* _zip, const ExportParameters& _exportParameters) const
{
	//
	// Если нужна нумерация внизу
	//
	if (_exportParameters.printPagesNumbers
		&& ::exportStyle().numberingAlignment().testFlag(Qt::AlignBottom)) {
		QString footerXml =
				"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
				"<w:ftr xmlns:o=\"urn:schemas-microsoft-com:office:office\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:v=\"urn:schemas-microsoft-com:vml\" xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" xmlns:w10=\"urn:schemas-microsoft-com:office:word\" xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\">";

		footerXml.append("<w:p><w:pPr><w:jc w:val=\"");
		if (::exportStyle().numberingAlignment().testFlag(Qt::AlignLeft)) {
			footerXml.append("left");
		} else if (::exportStyle().numberingAlignment().testFlag(Qt::AlignCenter)) {
			footerXml.append("center");
		} else {
			footerXml.append("right");
		}
		footerXml.append("\"/><w:rPr/></w:pPr><w:r><w:rPr/><w:fldChar w:fldCharType=\"begin\"/></w:r><w:r><w:instrText> PAGE </w:instrText></w:r><w:r><w:fldChar w:fldCharType=\"separate\"/></w:r><w:r><w:t>0</w:t></w:r><w:r><w:fldChar w:fldCharType=\"end\"/></w:r></w:p>");
		footerXml.append("</w:ftr>");

		//
		// Запишем верхний колонтитул в архив
		//
		_zip->addFile(QString::fromLatin1("word/footer1.xml"), footerXml.toUtf8());
	}
}

void DocxExporter::writeDocument(QtZipWriter* _zip, ScenarioDocument* _scenario,
	const ExportParameters& _exportParameters) const
{
	QString documentXml =
			"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
			"<w:document xmlns:o=\"urn:schemas-microsoft-com:office:office\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:v=\"urn:schemas-microsoft-com:vml\" xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" xmlns:w10=\"urn:schemas-microsoft-com:office:word\" xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\">"
			"<w:body>";

	//
	// Сформируем документ
	//
	QTextDocument* preparedDocument = prepareDocument(_scenario, _exportParameters);

	//
	// Данные считываются из исходного документа, определяется тип блока
	// и записываются прямо в файл
	//
	QTextCursor documentCursor(preparedDocument);
	while (!documentCursor.atEnd()) {
		documentXml.append(::docxText(documentCursor));

		//
		// Переходим к следующему параграфу
		//
		documentCursor.movePosition(QTextCursor::EndOfBlock);
		documentCursor.movePosition(QTextCursor::NextBlock);
	}

	//
	// В конце идёт блок настроек страницы
	//
	ScenarioStyle style = ::exportStyle();
	documentXml.append("<w:sectPr>");
	//
	// ... колонтитулы
	//
	if (_exportParameters.printPagesNumbers) {
		if (style.numberingAlignment().testFlag(Qt::AlignTop)) {
			documentXml.append("<w:headerReference w:type=\"default\" r:id=\"docRId1\"/>");
		} else {
			documentXml.append("<w:footerReference w:type=\"default\" r:id=\"docRId2\"/>");
		}
	}
	//
	// ... размер страницы
	//
	QSizeF paperSize = QPageSize(style.pageSizeId()).size(QPageSize::Millimeter);
	documentXml.append(
		QString("<w:pgSz w:w=\"%1\" w:h=\"%2\"/>")
		.arg(::mmToTwips(paperSize.width()))
		.arg(::mmToTwips(paperSize.height()))
		);
	//
	// ... поля документа
	//
	documentXml.append(
		QString("<w:pgMar w:left=\"%1\" w:right=\"%2\" w:top=\"%3\" w:bottom=\"%4\" w:header=\"%5\" w:footer=\"%6\" w:gutter=\"0\"/>")
		.arg(::mmToTwips(style.pageMargins().left()))
		.arg(::mmToTwips(style.pageMargins().right()))
		.arg(::mmToTwips(style.pageMargins().top()))
		.arg(::mmToTwips(style.pageMargins().bottom()))
		.arg(::mmToTwips(style.pageMargins().top() / 2))
		.arg(::mmToTwips(style.pageMargins().bottom() / 2))
		);
	//
	// ... нужна ли титульная страница
	//
	if (_exportParameters.printTilte) {
		documentXml.append("<w:titlePg/>");
	}
	//
	// ... конец блока настроек страницы
	//
	documentXml.append(
			"<w:pgNumType w:fmt=\"decimal\"/>"
			"<w:textDirection w:val=\"lrTb\"/>"
		"</w:sectPr>"
		);

	documentXml.append("</w:body></w:document>");

	//
	// Запишем документ в архив
	//
	_zip->addFile(QString::fromLatin1("word/document.xml"), documentXml.toUtf8());
}

/*
QString DocxExporter::header(const ExportParameters& _exportParameters) const
{
	QString header = "\\rtf1\\ansi";

	//
	// Настройки в соответсвии со стилем
	//
	ScenarioStyle style = ::exportStyle();

	//
	// Настройки шрифтов
	//
	header.append("{\\fonttbl");
	foreach (int blockNumber, ::blockTypes().keys()) {
		ScenarioBlockStyle blockStyle = style.blockStyle(::blockTypes().value(blockNumber));
		header.append(::rtfBlockFont(blockStyle));
	}
	header.append("}\n");

	//
	// Настройки размера документа
	//
	QSizeF paperSize = QPageSize(style.pageSizeId()).size(QPageSize::Millimeter);
	header.append(
				QString("\\paperh%1\\paperw%2")
				.arg(::mmToTwips(paperSize.height()))
				.arg(::mmToTwips(paperSize.width()))
				);

	//
	// Настройки полей документа
	//
	header.append(
				QString("\\margl%1\\margr%2\\margt%3\\margb%4\\headery%5\\footery%6")
				.arg(::mmToTwips(style.pageMargins().left()))
				.arg(::mmToTwips(style.pageMargins().right()))
				.arg(::mmToTwips(style.pageMargins().top()))
				.arg(::mmToTwips(style.pageMargins().bottom()))
				.arg(::mmToTwips(style.pageMargins().top() / 2))
				.arg(::mmToTwips(style.pageMargins().bottom() / 2))
				);

	//
	// Если печатается титульная страница
	//
	if (_exportParameters.printTilte) {
		header.append("\\titlepg");
	}

	//
	// Настройки используемых стилей
	//
	header.append("{\\stylesheet");
	foreach (int blockNumber, ::blockTypes().keys()) {
		ScenarioBlockStyle blockStyle = style.blockStyle(::blockTypes().value(blockNumber));
		header.append(QString("{%1;}").arg(::docxBlockStyle(blockStyle, true)));
	}
	header.append("}");

	//
	// Номера страниц
	//
	if (_exportParameters.printPagesNumbers) {
		header.append("{");
		if (::exportStyle().numberingAlignment().testFlag(Qt::AlignTop)) {
			header.append("\\header");
		} else {
			header.append("\\footer");
		}
		header.append("\\pard");
		if (::exportStyle().numberingAlignment().testFlag(Qt::AlignLeft)) {
			header.append("\\ql");
		} else if (::exportStyle().numberingAlignment().testFlag(Qt::AlignCenter)) {
			header.append("\\qc");
		} else {
			header.append("\\qr");
		}
		header.append(" \\chpgn\\par");
		header.append("}");
	}

	return header;
}
*/
