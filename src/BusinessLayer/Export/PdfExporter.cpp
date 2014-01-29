#include "PdfExporter.h"

#include <QString>
#include <QPrinter>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>

using namespace BusinessLogic;


PdfExporter::PdfExporter()
{
}

void PdfExporter::exportTo(QTextDocument* _document, const QString& _toFile) const
{
	//
	// Настроим принтер
	//
	QPrinter printer;
	printer.setPaperSize(QPrinter::A4);
	printer.setOutputFileName(_toFile);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setPageMargins(37.5, 25, 25, 12.5, QPrinter::Millimeter);

	//
	// Сформируем документ
	//
	QTextDocument* preparedDocument = prepareDocument(_document);

	//
	// Настроим размер страниц
	//
	preparedDocument->setPageSize(QSizeF(printer.pageRect().size()));

	//
	// Печатаем документ
	//
	preparedDocument->print(&printer);

	//
	// Освобождаем память
	//
	delete preparedDocument;
	preparedDocument = 0;
}

QTextDocument* PdfExporter::prepareDocument(QTextDocument* _document) const
{
	//
	// Настроим новый документ
	//
	QTextDocument* preparedDocument = new QTextDocument;
	preparedDocument->setDefaultFont(_document->defaultFont());
	preparedDocument->setDocumentMargin(0);
	preparedDocument->setIndentWidth(0);

	//
	// Данные считываются из исходного документа, если необходимо преобразовываются
	// и записываются в новый документ, который будет печататься
	//
	QTextCursor sourceDocumentCursor(_document);
	QTextCursor destDocumentCursor(preparedDocument);

	ScenarioTextBlockStyle::Type currentBlockType = ScenarioTextBlockStyle::Undefined;
	while (!sourceDocumentCursor.atEnd()) {
		//
		// Получим тип текущего блока под курсором
		//
		currentBlockType = ScenarioTextBlockStyle::forBlock(sourceDocumentCursor.block());

		//
		// Если блок содержит текст, который необходимо вывести на печать
		//
		if (currentBlockType != ScenarioTextBlockStyle::NoprintableText
			&& currentBlockType != ScenarioTextBlockStyle::FolderHeader
			&& currentBlockType != ScenarioTextBlockStyle::FolderFooter) {
			//
			// Если вставляется не первый блок текста
			//
			if (!preparedDocument->isEmpty()) {
				//
				// Если нужно сделать отступ
				//
				if (currentBlockType != ScenarioTextBlockStyle::Dialog
					&& currentBlockType != ScenarioTextBlockStyle::Parenthetical) {
					destDocumentCursor.insertBlock();
				}

				//
				// Вставим новый абзац для наполнения текстом
				//
				destDocumentCursor.insertBlock();
			}

			//
			// Настроить стиль блока
			//
			destDocumentCursor.setCharFormat(charFormatForType(currentBlockType));
			destDocumentCursor.setBlockFormat(blockFormatForType(currentBlockType));

			//
			// Вставить текст
			//
			destDocumentCursor.insertText(sourceDocumentCursor.block().text());
		}

		//
		// Переходим к следующему блоку
		//
		sourceDocumentCursor.movePosition(QTextCursor::EndOfBlock);
		sourceDocumentCursor.movePosition(QTextCursor::NextBlock);
	}

	return preparedDocument;
}

QTextCharFormat PdfExporter::charFormatForType(ScenarioTextBlockStyle::Type _type) const
{
	QTextCharFormat format;

	switch (_type) {
		case ScenarioTextBlockStyle::TimeAndPlace:
		case ScenarioTextBlockStyle::Character:
		case ScenarioTextBlockStyle::Note:
		case ScenarioTextBlockStyle::TitleHeader:
		case ScenarioTextBlockStyle::SceneGroupHeader:
		case ScenarioTextBlockStyle::SceneGroupFooter:
		case ScenarioTextBlockStyle::Transition: {
			format.setFontCapitalization(QFont::AllUppercase);
			break;
		}

		default: {
			break;
		}
	}

	return format;
}

QTextBlockFormat PdfExporter::blockFormatForType(ScenarioTextBlockStyle::Type _type) const
{
	//
	// TODO: проверить на других машинах/ОС
	//

	QTextBlockFormat format;

	//
	// Настраиваем отступы
	//
	switch (_type) {
		case ScenarioTextBlockStyle::Character: {
			format.setLeftMargin(259);
			break;
		}

		case ScenarioTextBlockStyle::Dialog: {
			format.setLeftMargin(144);
			format.setRightMargin(144);
			break;
		}

		case ScenarioTextBlockStyle::Parenthetical:
		case ScenarioTextBlockStyle::Title: {
			format.setLeftMargin(211);
			format.setRightMargin(173);
			break;
		}

		case ScenarioTextBlockStyle::Transition: {
			format.setAlignment(Qt::AlignRight);
			break;
		}

		default: {
			break;
		}
	}

	return format;
}
