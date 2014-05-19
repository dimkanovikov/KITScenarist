#include "PdfExporter.h"

#include <QApplication>
#include <QString>
#include <QPrinter>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QPrintPreviewDialog>

using namespace BusinessLogic;


PdfExporter::PdfExporter(QObject* _parent) :
	QObject(_parent),
	m_documentForPrint(0)
{
}

void PdfExporter::exportTo(QTextDocument* _document, const QString& _toFile) const
{
	//
	// Настроим принтер
	//
	QPrinter* printer = preparePrinter(_toFile);

	//
	// Сформируем документ
	//
	QTextDocument* preparedDocument = prepareDocument(_document);

	//
	// Настроим размер страниц
	//
	QFontMetrics fm(preparedDocument->defaultFont());
	int pageWidth = fm.width("W") * 60;
	int pageHeight = fm.lineSpacing() * 50;
	QSizeF documentSize = QSizeF(pageWidth, pageHeight);
	preparedDocument->setPageSize(documentSize);

	//
	// Печатаем документ
	//
	preparedDocument->print(printer);

	//
	// Освобождаем память
	//
	delete printer;
	printer = 0;
	delete preparedDocument;
	preparedDocument = 0;
}

void PdfExporter::printPreview(QTextDocument* _document)
{
	//
	// Настроим принтер
	//
	QPrinter* printer = preparePrinter();

	//
	// Сформируем документ
	//
	QTextDocument* preparedDocument = prepareDocument(_document);

	//
	// Настроим размер страниц
	//
	QFontMetrics fm(preparedDocument->defaultFont());
	int pageWidth = fm.width("W") * 60;
	int pageHeight = fm.lineSpacing() * 50;
	QSizeF documentSize = QSizeF(pageWidth, pageHeight);
	preparedDocument->setPageSize(documentSize);

	//
	// Сохраним указатель на документ для печати
	//
	m_documentForPrint = preparedDocument;

	//
	// Настроим диалог предварительного просмотра
	//
	QPrintPreviewDialog printDialog(printer, qApp->activeWindow());
	printDialog.setWindowState( Qt::WindowMaximized );
	connect(&printDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(aboutPrint(QPrinter*)));

	//
	// Вызываем диалог предварительного просмотра и печати
	//
	printDialog.exec();

	//
	// Освобождаем память
	//
	delete printer;
	printer = 0;
	m_documentForPrint = 0;
	delete preparedDocument;
	preparedDocument = 0;
}

void PdfExporter::aboutPrint(QPrinter* _printer)
{
	if (m_documentForPrint != 0) {
		m_documentForPrint->print(_printer);
	}
}

QPrinter* PdfExporter::preparePrinter(const QString& _forFile) const
{
	QPrinter* printer = new QPrinter;
	printer->setPaperSize(QPrinter::A4);
	printer->setOutputFileName(_forFile);
	printer->setOutputFormat(QPrinter::PdfFormat);
	printer->setPageMargins(37.5, 25, 25, 12.5, QPrinter::Millimeter);
	return printer;
}

QTextDocument* PdfExporter::prepareDocument(QTextDocument* _document) const
{
	//
	// Настроим новый документ
	//
	QTextDocument* preparedDocument = new QTextDocument;
	QFont documentFont = _document->defaultFont();
	documentFont.setPointSize(12);
	preparedDocument->setDefaultFont(documentFont);
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
			if (!sourceDocumentCursor.atStart()) {
				//
				// Если нужно сделать отступ
				//
//				if (!sourceDocumentCursor.block().text().isEmpty()
//					&& currentBlockType != ScenarioTextBlockStyle::Dialog
//					&& currentBlockType != ScenarioTextBlockStyle::Parenthetical) {
//					destDocumentCursor.insertBlock();
//				}

				//
				// Вставим новый абзац для наполнения текстом
				//
				destDocumentCursor.insertBlock();
			}

			//
			// Настроить стиль блока
			//
			destDocumentCursor.setCharFormat(charFormatForType(currentBlockType));
			destDocumentCursor.setBlockFormat(blockFormatForType(currentBlockType, documentFont));

			//
			// Вставить текст
			//
			// Приходится вручную устанавливать верхний регистр для текста,
			// т.к. при выводе в диалог предварительного просмотра эта
			// настройка не учитывается...
			//
			if (charFormatForType(currentBlockType).fontCapitalization() == QFont::AllUppercase) {
				destDocumentCursor.insertText(sourceDocumentCursor.block().text().toUpper());
			} else {
				destDocumentCursor.insertText(sourceDocumentCursor.block().text());
			}
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

QTextBlockFormat PdfExporter::blockFormatForType(ScenarioTextBlockStyle::Type _type, const QFont& _font) const
{
	ScenarioTextBlockStyle style(_type, _font);
	QTextBlockFormat styleBlockFormat = style.blockFormat();

	//
	// TODO: проверить на других машинах/ОС
	//

	QTextBlockFormat format;

	//
	// Настраиваем отступы
	//
	format.setTopMargin(styleBlockFormat.topMargin());
	format.setLeftMargin(styleBlockFormat.leftMargin());
	format.setRightMargin(styleBlockFormat.rightMargin());
	format.setAlignment(styleBlockFormat.alignment());

	/*
	switch (_type) {
		case ScenarioTextBlockStyle::Character: {
			format.setLeftMargin(styleBlockFormat.leftMargin());
			break;
		}

		case ScenarioTextBlockStyle::Dialog: {
			format.setLeftMargin(styleBlockFormat.leftMargin());
			format.setRightMargin(styleBlockFormat.rightMargin());
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
	}*/

	return format;
}
