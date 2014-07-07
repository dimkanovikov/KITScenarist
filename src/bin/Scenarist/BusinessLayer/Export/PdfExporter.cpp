#include "PdfExporter.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/PagesTextEdit/PageMetrics.h>

#include <QApplication>
#include <QString>
#include <QPrinter>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QPrintPreviewDialog>

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
}


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
	QSizeF pageSize = QPageSize(::exportStyle().pageSizeId()).size(QPageSize::Millimeter);
	QMarginsF pageMargins = ::exportStyle().pageMargins();
	QSizeF textSize(PageMetrics::mmToPx(pageSize.width() - pageMargins.left() - pageMargins.right()),
					PageMetrics::mmToPx(pageSize.height() - pageMargins.top() - pageMargins.bottom()));
	preparedDocument->setPageSize(textSize);

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
	QSizeF pageSize = QPageSize(::exportStyle().pageSizeId()).size(QPageSize::Millimeter);
	QMarginsF pageMargins = ::exportStyle().pageMargins();
	QSizeF textSize(PageMetrics::mmToPx(pageSize.width() - pageMargins.left() - pageMargins.right()),
					PageMetrics::mmToPx(pageSize.height() - pageMargins.top() - pageMargins.bottom()));
	preparedDocument->setPageSize(textSize);

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
	m_documentForPrint = 0;
	delete printer;
	printer = 0;
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
	printer->setPaperSize((QPagedPaintDevice::PageSize)::exportStyle().pageSizeId());
	QMarginsF margins = ::exportStyle().pageMargins();
	printer->setPageMargins(margins.left(), margins.top(), margins.right(), margins.bottom(),
							QPrinter::Millimeter);

	if (!_forFile.isNull()) {
		printer->setOutputFileName(_forFile);
		printer->setOutputFormat(QPrinter::PdfFormat);
	}

	return printer;
}

QTextDocument* PdfExporter::prepareDocument(QTextDocument* _document) const
{
	//
	// Настроим новый документ
	//
	QTextDocument* preparedDocument = new QTextDocument;
	preparedDocument->setDocumentMargin(0);
	preparedDocument->setIndentWidth(0);

	//
	// Данные считываются из исходного документа, если необходимо преобразовываются
	// и записываются в новый документ, который будет печататься
	//
	QTextCursor sourceDocumentCursor(_document);
	QTextCursor destDocumentCursor(preparedDocument);

	ScenarioBlockStyle::Type currentBlockType = ScenarioBlockStyle::Undefined;
	while (!sourceDocumentCursor.atEnd()) {
		//
		// Получим тип текущего блока под курсором
		//
		currentBlockType = ScenarioBlockStyle::forBlock(sourceDocumentCursor.block());

		//
		// Если блок содержит текст, который необходимо вывести на печать
		//
		if (currentBlockType != ScenarioBlockStyle::NoprintableText
			&& currentBlockType != ScenarioBlockStyle::FolderHeader
			&& currentBlockType != ScenarioBlockStyle::FolderFooter) {
			//
			// Если вставляется не первый блок текста
			//
			if (!sourceDocumentCursor.atStart()) {
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

QTextCharFormat PdfExporter::charFormatForType(ScenarioBlockStyle::Type _type) const
{
	QTextCharFormat format = ::exportStyle().blockStyle(_type).charFormat();

	//
	// Очищаем цвета
	//
	format.setForeground(Qt::black);

	return format;
}

QTextBlockFormat PdfExporter::blockFormatForType(ScenarioBlockStyle::Type _type) const
{
	QTextBlockFormat format = ::exportStyle().blockStyle(_type).blockFormat();

	//
	// Очищаем цвета
	//
	format.setBackground(Qt::white);

	return format;
}
