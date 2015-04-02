#include "PdfExporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>

#include <Domain/Scenario.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/PagesTextEdit/PageMetrics.h>

#include <QAbstractTextDocumentLayout>
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
	static ScenarioTemplate exportStyle() {
		return ScenarioTemplateFacade::getTemplate(
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
	 * @brief Напечатать страницу документа
	 * @note Адаптация функции QTextDocument.cpp::anonymous::printPage
	 */
	static void printPage(int _pageNumber, QPainter* _painter, const QTextDocument* _document,
		const QRectF& _body, const QPagedPaintDevice::Margins& _margins)
	{
		const int pageYPos = (_pageNumber - 1) * _body.height();

		_painter->save();
		_painter->translate(_body.left(), _body.top() - pageYPos);
		QRectF currentPageRect(0, pageYPos, _body.width(), _body.height());
		QAbstractTextDocumentLayout *layout = _document->documentLayout();
		QAbstractTextDocumentLayout::PaintContext ctx;
		_painter->setClipRect(currentPageRect);
		ctx.clip = currentPageRect;
		// don't use the system palette text as default text color, on HP/UX
		// for example that's white, and white text on white paper doesn't
		// look that nice
		ctx.palette.setColor(QPalette::Text, Qt::black);
		layout->draw(_painter, ctx);

		//
		// Если необходимо рисуем нумерацию страниц
		//
		if (_document->property(PRINT_PAGE_NUMBERS_KEY).toBool()) {
			//
			// Если печатается титульная страница
			//
			if (_document->property(PRINT_TITLE_KEY).toBool()
				&& _pageNumber == 1) {
				//
				// Не печатаем номер на первой странице
				//
			}
			//
			// Печатаем номера страниц
			//
			else {
				_painter->save();
				_painter->setFont(QFont("Courier New", 12));

				//
				// Середины верхнего и нижнего полей
				//
				qreal headerY = pageYPos - PageMetrics::mmToPx(_margins.top) / 2;
				qreal footerY = pageYPos + currentPageRect.height() + PageMetrics::mmToPx(_margins.bottom) / 2;

				//
				// Области для прорисовки текста на полях
				//
				QRectF headerRect(0, headerY, currentPageRect.width(), 20);
				QRectF footerRect(0, footerY - 20, currentPageRect.width(), 20);

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
				_painter->setClipRect(numberingRect);
				_painter->drawText(numberingRect, numberingAlignment,
								 QString::number(_pageNumber + titleDelta));
				_painter->restore();
			}
		}
		_painter->restore();
	}

	/**
	 * @brief Напечатать документ
	 * @note Адаптация функции QTextDocument::print
	 */
	static void printDocument(QTextDocument* _document, QPrinter* _printer)
	{
		QPainter painter(_printer);
		// Check that there is a valid device to print to.
		if (!painter.isActive())
			return;
		QScopedPointer<QTextDocument> clonedDoc;
		(void)_document->documentLayout(); // make sure that there is a layout
		QRectF body = QRectF(QPointF(0, 0), _document->pageSize());

		{
			qreal sourceDpiX = painter.device()->logicalDpiX();
			qreal sourceDpiY = sourceDpiX;
			QPaintDevice *dev = _document->documentLayout()->paintDevice();
			if (dev) {
				sourceDpiX = dev->logicalDpiX();
				sourceDpiY = dev->logicalDpiY();
			}
			const qreal dpiScaleX = qreal(_printer->logicalDpiX()) / sourceDpiX;
			const qreal dpiScaleY = qreal(_printer->logicalDpiY()) / sourceDpiY;
			// scale to dpi
			painter.scale(dpiScaleX, dpiScaleY);
			QSizeF scaledPageSize = _document->pageSize();
			scaledPageSize.rwidth() *= dpiScaleX;
			scaledPageSize.rheight() *= dpiScaleY;
			const QSizeF printerPageSize(_printer->pageRect().size());
			// scale to page
			painter.scale(printerPageSize.width() / scaledPageSize.width(),
					printerPageSize.height() / scaledPageSize.height());
		}

		int docCopies;
		int pageCopies;
		if (_printer->collateCopies() == true){
			docCopies = 1;
			pageCopies = _printer->supportsMultipleCopies() ? 1 : _printer->copyCount();
		} else {
			docCopies = _printer->supportsMultipleCopies() ? 1 : _printer->copyCount();
			pageCopies = 1;
		}
		int fromPage = _printer->fromPage();
		int toPage = _printer->toPage();
		bool ascending = true;
		if (fromPage == 0 && toPage == 0) {
			fromPage = 1;
			toPage = _document->pageCount();
		}
		// paranoia check
		fromPage = qMax(1, fromPage);
		toPage = qMin(_document->pageCount(), toPage);
		if (toPage < fromPage) {
			// if the user entered a page range outside the actual number
			// of printable pages, just return
			return;
		}
		if (_printer->pageOrder() == QPrinter::LastPageFirst) {
			int tmp = fromPage;
			fromPage = toPage;
			toPage = tmp;
			ascending = false;
		}
		for (int i = 0; i < docCopies; ++i) {
			int page = fromPage;
			while (true) {
				for (int j = 0; j < pageCopies; ++j) {
					if (_printer->printerState() == QPrinter::Aborted
						|| _printer->printerState() == QPrinter::Error)
						return;
					printPage(page, &painter, _document, body, _printer->margins());
					if (j < pageCopies - 1)
						_printer->newPage();
				}
				if (page == toPage)
					break;
				if (ascending)
					++page;
				else
					--page;
				_printer->newPage();
			}
			if ( i < docCopies - 1)
				_printer->newPage();
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
