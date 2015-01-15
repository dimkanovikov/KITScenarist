#include "PdfExporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>

#include <Domain/Scenario.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/PagesTextEdit/PageMetrics.h>

#include <QApplication>
#include <QString>
#include <QPainter>
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

	/**
	 * @brief Есть ли в документе титульная страница
	 */
	const char* PRINT_TITLE_KEY = "print_title";

	/**
	 * @brief Необходимо ли печатать номера страниц
	 */
	const char* PRINT_PAGE_NUMBERS_KEY = "page_numbers";

	/**
	 * @brief Напечатать документ
	 */
	static void printDocument(QTextDocument* _document, QPrinter* _printer) {
		if (_document != 0
			&& _printer != 0) {

			QRect innerRect = _printer->pageRect();
			QRect contentRect = QRect(QPoint(0,0), _document->size().toSize());
			QRect currentRect = QRect(QPoint(0,0), innerRect.size());
			QPainter painter(_printer);
			int pageNumber = 0;
			painter.save();


			while (currentRect.intersects(contentRect)) {
				_document->drawContents(&painter, currentRect);
				pageNumber++;
				currentRect.translate(0, currentRect.height());
				painter.restore();

				//
				// Если необходимо рисуем нумерацию страниц
				//
				if (_document->property(PRINT_PAGE_NUMBERS_KEY).toBool()) {
					//
					// Если печатается титульная страница
					//
					if (_document->property(PRINT_TITLE_KEY).toBool()
						&& pageNumber == 1) {
						//
						// Не печатаем номер на первой странице
						//
					}
					//
					// Печатаем номера страниц
					//
					else {
						painter.setFont(QFont("Courier New", 12));

						//
						// Середины верхнего и нижнего полей
						//
						qreal headerY = 0 - PageMetrics::mmToPx(_printer->margins().top) / 2;
						qreal footerY = currentRect.height() + PageMetrics::mmToPx(_printer->margins().bottom) / 2;

						//
						// Области для прорисовки текста на полях
						//
						QRectF headerRect(0, headerY, currentRect.width(), 20);
						QRectF footerRect(0, footerY - 20, currentRect.width(), 20);

						//
						// Определяем где положено находиться нумерации
						//
						QRectF numberingRect;
						if (::exportStyle().numberingAlignment().testFlag(Qt::AlignTop)) {
							numberingRect = headerRect;
						} else {
							numberingRect = footerRect;
						}
						Qt::Alignment numberingAlignment = Qt::AlignVCenter;
						if (::exportStyle().numberingAlignment().testFlag(Qt::AlignLeft)) {
							numberingAlignment |= Qt::AlignLeft;
						} else if (::exportStyle().numberingAlignment().testFlag(Qt::AlignCenter)) {
							numberingAlignment |= Qt::AlignCenter;
						} else {
							numberingAlignment |= Qt::AlignRight;
						}

						//
						// Рисуем нумерацию в положеном месте (отнимаем единицу, т.к. нумерация
						// должна следовать с единицы для первой страницы текста сценария)
						//
						int titleDelta = _document->property(PRINT_TITLE_KEY).toBool() ? -1 : 0;
						painter.drawText(numberingRect, numberingAlignment,
							QString::number(pageNumber + titleDelta));
					}
				}

				painter.save();
				painter.translate(0, -currentRect.height() * pageNumber);
				if (currentRect.intersects(contentRect))
					_printer->newPage();
			}
			painter.restore();
			painter.end();
		}
	}
}


PdfExporter::PdfExporter(QObject* _parent) :
	QObject(_parent),
	m_documentForPrint(0)
{
}

void PdfExporter::exportTo(ScenarioDocument* _scenario, const ExportParameters& _exportParameters) const
{
	//
	// Настроим принтер
	//
	QPrinter* printer = preparePrinter(_exportParameters.filePath);

	//
	// Сформируем документ
	//
	QTextDocument* preparedDocument = prepareDocument(_scenario, _exportParameters);
	preparedDocument->setProperty(PRINT_TITLE_KEY, _exportParameters.printTilte);
	preparedDocument->setProperty(PRINT_PAGE_NUMBERS_KEY, _exportParameters.printPagesNumbers);

	//
	// Печатаем документ
	//
	::printDocument(preparedDocument, printer);

	//
	// Освобождаем память
	//
	delete printer;
	printer = 0;
	delete preparedDocument;
	preparedDocument = 0;
}

void PdfExporter::printPreview(ScenarioDocument* _scenario, const ExportParameters& _exportParameters)
{
	//
	// Настроим принтер
	//
	QPrinter* printer = preparePrinter();

	//
	// Сформируем документ
	//
	QTextDocument* preparedDocument = prepareDocument(_scenario, _exportParameters);
	preparedDocument->setProperty(PRINT_TITLE_KEY, _exportParameters.printTilte);
	preparedDocument->setProperty(PRINT_PAGE_NUMBERS_KEY, _exportParameters.printPagesNumbers);

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
	::printDocument(m_documentForPrint, _printer);
}

QPrinter* PdfExporter::preparePrinter(const QString& _forFile) const
{
	QPrinter* printer = new QPrinter;
	printer->setPageSize(QPageSize(::exportStyle().pageSizeId()));
	QMarginsF margins = ::exportStyle().pageMargins();
	printer->setPageMargins(margins.left(), margins.top(), margins.right(), margins.bottom(),
							QPrinter::Millimeter);

	if (!_forFile.isNull()) {
		printer->setOutputFileName(_forFile);
		printer->setOutputFormat(QPrinter::PdfFormat);
	}

	return printer;
}
