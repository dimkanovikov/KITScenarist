#include "PdfExporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>

#include <Domain/Scenario.h>

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
