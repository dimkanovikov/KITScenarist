#include "AbstractExporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioStyle.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>

#include <Domain/Scenario.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/PagesTextEdit/PageMetrics.h>

#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

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
	 * @brief Определить размер страницы документа
	 */
	static QSizeF documentSize() {
		ScenarioStyle exportStyle = ::exportStyle();
		QSizeF pageSize = QPageSize(exportStyle.pageSizeId()).size(QPageSize::Millimeter);
		QMarginsF pageMargins = exportStyle.pageMargins();

		return QSizeF(
					PageMetrics::mmToPx(pageSize.width() - pageMargins.left() - pageMargins.right()),
					PageMetrics::mmToPx(pageSize.height() - pageMargins.top() - pageMargins.bottom(), false)
					);
	}

	/**
	 * @brief Получить стиль оформления символов для заданного типа
	 */
	static QTextCharFormat charFormatForType(ScenarioBlockStyle::Type _type) {
		QTextCharFormat format = ::exportStyle().blockStyle(_type).charFormat();

		//
		// Очищаем цвета
		//
		format.setForeground(Qt::black);

		return format;
	}

	/**
	 * @brief Получить стиль оформления абзаца для заданного типа
	 */
	static QTextBlockFormat blockFormatForType(ScenarioBlockStyle::Type _type) {
		QTextBlockFormat format = ::exportStyle().blockStyle(_type).blockFormat();

		//
		// Очищаем цвета
		//
		format.setBackground(Qt::white);

		//
		// Убираем горизонтальные отступы
		//
		format.setTopMargin(0);
		format.setBottomMargin(0);

		return format;
	}

	/**
	 * @brief Типы строк документа
	 */
	enum LineType {
		UndefinedLine,
		FirstDocumentLine,
		MiddlePageLine,
		LastPageLine
	};

	/**
	 * @brief Определить тип следующей строки документа
	 */
	static LineType currentLine(QTextDocument* _inDocument) {
		LineType type = UndefinedLine;

		if (_inDocument->isEmpty()) {
			type = FirstDocumentLine;
		} else {
			//
			// Определяем конец страницы или серидина при помощи проверки
			// на количество страниц, после вставки новой строки
			//
			const int documentPagesCount = _inDocument->pageCount();
			QTextCursor cursor(_inDocument);
			cursor.movePosition(QTextCursor::End);
			cursor.insertBlock();
			const int documentPagesCountWithNextLine = _inDocument->pageCount();
			cursor.deletePreviousChar();
			if (documentPagesCount == documentPagesCountWithNextLine) {
				type = MiddlePageLine;
			} else {
				type = LastPageLine;
			}
		}

		return type;
	}
}


QTextDocument* AbstractExporter::prepareDocument(const BusinessLogic::ScenarioDocument* _scenario,
		const ExportParameters& _exportParameters) const
{
	ScenarioStyle exportStyle = ::exportStyle();

	//
	// Настроим новый документ
	//
	QTextDocument* preparedDocument = new QTextDocument;
	preparedDocument->setDocumentMargin(0);
	preparedDocument->setIndentWidth(0);

	//
	// Настроим размер страниц
	//
	preparedDocument->setPageSize(::documentSize());

	//
	// Данные считываются из исходного документа, если необходимо преобразовываются,
	// и записываются в новый документ
	//
	QTextCursor sourceDocumentCursor(_scenario->document());
	QTextCursor destDocumentCursor(preparedDocument);


	//
	// Формирование титульной страницы
	//
	if (_exportParameters.printTilte) {
		QTextCharFormat titleFormat;
		titleFormat.setFont(QFont("Courier New", 12));
		QTextBlockFormat centerFormat;
		centerFormat.setAlignment(Qt::AlignCenter);
		centerFormat.setLineHeight(18.09, QTextBlockFormat::FixedHeight);
		QTextBlockFormat rightFormat;
		rightFormat.setAlignment(Qt::AlignRight);
		rightFormat.setLineHeight(18.09, QTextBlockFormat::FixedHeight);

		//
		// Номер текущей строки
		//
		int currentLineNumber = 0;

		//
		// Название [13 строка]
		//
		while ((currentLineNumber++) < 12) {
			destDocumentCursor.insertBlock(centerFormat);
			destDocumentCursor.setCharFormat(titleFormat);
		}
		destDocumentCursor.insertBlock(centerFormat);
		destDocumentCursor.setCharFormat(titleFormat);
		destDocumentCursor.insertText(_exportParameters.scenarioName);
		//
		// Доп. инфо [через одну под предыдущим]
		//
		if (!_exportParameters.scenarioAdditionalInfo.isEmpty()) {
			destDocumentCursor.insertBlock(centerFormat);
			destDocumentCursor.setCharFormat(titleFormat);
			destDocumentCursor.insertBlock(centerFormat);
			destDocumentCursor.setCharFormat(titleFormat);
			destDocumentCursor.insertText(_exportParameters.scenarioAdditionalInfo);
			currentLineNumber += 2;
		}
		//
		// Жанр [через одну под предыдущим]
		//
		if (!_exportParameters.scenarioGenre.isEmpty()) {
			destDocumentCursor.insertBlock(centerFormat);
			destDocumentCursor.setCharFormat(titleFormat);
			destDocumentCursor.insertBlock(centerFormat);
			destDocumentCursor.setCharFormat(titleFormat);
			destDocumentCursor.insertText(_exportParameters.scenarioGenre);
			currentLineNumber += 2;
		}
		//
		// Автор [через одну под предыдущим]
		//
		if (!_exportParameters.scenarioAuthor.isEmpty()) {
			destDocumentCursor.insertBlock(centerFormat);
			destDocumentCursor.insertBlock(centerFormat);
			destDocumentCursor.setCharFormat(titleFormat);
			destDocumentCursor.insertText(_exportParameters.scenarioAuthor);
			currentLineNumber += 2;
		}
		//
		// необходимое количество пустых строк до 30ой
		//
		while ((currentLineNumber++) < 29) {
			destDocumentCursor.insertBlock(centerFormat);
			destDocumentCursor.setCharFormat(titleFormat);
		}
		//
		// Контакты [30 строка]
		//
		destDocumentCursor.insertBlock(rightFormat);
		destDocumentCursor.setCharFormat(titleFormat);
		destDocumentCursor.insertText(_exportParameters.scenarioContacts);

		//
		// Год печатается на последней строке документа
		//
		LineType currentLineType = ::currentLine(preparedDocument);
		while (currentLineType != LastPageLine) {
			destDocumentCursor.insertBlock();
			currentLineType = ::currentLine(preparedDocument);
		}
		destDocumentCursor.deletePreviousChar();
		destDocumentCursor.insertBlock(centerFormat);
		destDocumentCursor.setCharFormat(titleFormat);
		destDocumentCursor.insertText(_exportParameters.scenarioYear);
	}


	//
	// Запись текста документа
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
			// Определим стили и настроим курсор
			//
			QTextBlockFormat blockFormat = ::blockFormatForType(currentBlockType);
			QTextCharFormat charFormat = ::charFormatForType(currentBlockType);

			//
			// Действие для блоков не учитывающих переносы
			//

			//
			// Если вставляется не первый блок текста, возможно следует сделать отступы
			//
			{
				LineType currentLineType = ::currentLine(preparedDocument);
				if (currentLineType == MiddlePageLine) {
					int emptyLines = exportStyle.blockStyle(currentBlockType).topSpace();
					//
					// ... выполняется до тех пор, пока не будут вставлены все строки,
					//	   или не будет достигнут конец страницы
					//
					while (emptyLines-- > 0
						   && currentLineType == MiddlePageLine) {
						//
						// ... вставим линию и настроим её стиль
						//
						destDocumentCursor.insertBlock();
						destDocumentCursor.setBlockFormat(blockFormat);
						destDocumentCursor.setCharFormat(charFormat);
						currentLineType = ::currentLine(preparedDocument);
					}
				}
			}

			//
			// Вставляется текст блока
			//
			{
				//
				// ... если вставляется не первый блок текста
				//
				LineType currentLineType = ::currentLine(preparedDocument);
				if (currentLineType != FirstDocumentLine) {
					//
					// ... вставим новый абзац для наполнения текстом
					//
					destDocumentCursor.insertBlock();
				}

				//
				// Настроим стиль нового абзаца
				//
				destDocumentCursor.setBlockFormat(blockFormat);
				destDocumentCursor.setCharFormat(charFormat);

				//
				// Для блока "Время и место" добавочная информация
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
		}

		//
		// Переходим к следующему блоку
		//
		sourceDocumentCursor.movePosition(QTextCursor::EndOfBlock);
		sourceDocumentCursor.movePosition(QTextCursor::NextBlock);
	}

	return preparedDocument;
}
