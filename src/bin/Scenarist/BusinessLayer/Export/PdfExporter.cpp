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

QTextDocument* PdfExporter::prepareDocument(ScenarioDocument* _scenario,
	const ExportParameters& _exportParameters) const
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
	QTextCursor sourceDocumentCursor(_scenario->document());
	QTextCursor destDocumentCursor(preparedDocument);

	//
	// Формирование титульной страницы
	//
	{
		QTextCharFormat titleFormat;
		titleFormat.setFont(QFont("Courier New", 12));
		QTextBlockFormat centerFormat;
		centerFormat.setAlignment(Qt::AlignCenter);
		QTextBlockFormat rightFormat;
		rightFormat.setAlignment(Qt::AlignRight);
		QTextBlockFormat lastBlockFormat;
		lastBlockFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
		lastBlockFormat.merge(centerFormat);

		//
		// 12 пустых строк
		//
		int emptyLines = 12;
		while ((emptyLines--) > 0) {
			destDocumentCursor.insertBlock();
		}
		//
		// Название
		//
		destDocumentCursor.insertBlock(centerFormat);
		destDocumentCursor.setCharFormat(titleFormat);
		destDocumentCursor.insertText(_scenario->scenario()->name());
		//
		// Доп. инфо
		//
		destDocumentCursor.insertBlock();
		destDocumentCursor.insertBlock(centerFormat);
		destDocumentCursor.setCharFormat(titleFormat);
		destDocumentCursor.insertText(_scenario->scenario()->additionalInfo());
		//
		// Жанр
		//
		destDocumentCursor.insertBlock();
		destDocumentCursor.insertBlock(centerFormat);
		destDocumentCursor.setCharFormat(titleFormat);
		destDocumentCursor.insertText(_scenario->scenario()->genre());
		//
		// Автор
		//
		destDocumentCursor.insertBlock();
		destDocumentCursor.insertBlock(centerFormat);
		destDocumentCursor.setCharFormat(titleFormat);
		destDocumentCursor.insertText(_scenario->scenario()->author());
		//
		// 19 пустых строк
		//
		emptyLines = 19;
		while ((emptyLines--) > 0) {
			destDocumentCursor.insertBlock();
		}
		//
		// Контакты
		//
		destDocumentCursor.insertBlock(rightFormat);
		destDocumentCursor.setCharFormat(titleFormat);
		destDocumentCursor.insertText(_scenario->scenario()->contacts());
		//
		// 1 пустых строки
		//
		emptyLines = 1;
		while ((emptyLines--) > 0) {
			destDocumentCursor.insertBlock();
		}
		//
		// Год
		//
		destDocumentCursor.insertBlock(lastBlockFormat);
		destDocumentCursor.setCharFormat(titleFormat);
		destDocumentCursor.insertText(_scenario->scenario()->year());
		destDocumentCursor.insertBlock();
	}

	//
	// Запись текста страницы
	//
	ScenarioBlockStyle::Type currentBlockType = ScenarioBlockStyle::Undefined;
	int currentSceneNumber = 1;
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
			// Для блока "Время и место"
			//
			if (currentBlockType == ScenarioBlockStyle::TimeAndPlace) {
				//
				// Префикс экспорта
				//
				destDocumentCursor.insertText(_exportParameters.scenesPrefix);
				//
				// Номер сцены, если необходимо
				//
				if (_exportParameters.printScenesNubers) {
					QString sceneNumber = QString("%1. ").arg(currentSceneNumber);
					destDocumentCursor.insertText(sceneNumber);
					++currentSceneNumber;
				}
			}

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
