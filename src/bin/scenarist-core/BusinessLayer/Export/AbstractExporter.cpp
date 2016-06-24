#include "AbstractExporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h>

#include <Domain/Scenario.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/TextEditHelper.h>
#include <3rd_party/Widgets/PagesTextEdit/PageMetrics.h>

#include <QApplication>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

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
	 * @brief Определить размер страницы документа
	 */
	static QSizeF documentSize() {
		QSizeF pageSize = QPageSize(exportStyle().pageSizeId()).size(QPageSize::Millimeter);
		QMarginsF pageMargins = exportStyle().pageMargins();

		return QSizeF(
					PageMetrics::mmToPx(pageSize.width() - pageMargins.left() - pageMargins.right()),
					PageMetrics::mmToPx(pageSize.height() - pageMargins.top() - pageMargins.bottom(), false)
					);
	}

	/**
	 * @brief Получить стиль оформления символов для заданного типа
	 */
	static QTextCharFormat charFormatForType(ScenarioBlockStyle::Type _type) {
		QTextCharFormat format = exportStyle().blockStyle(_type).charFormat();

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
		ScenarioBlockStyle style = exportStyle().blockStyle(_type);
		QTextBlockFormat format = style.blockFormat();

		format.setProperty(ScenarioBlockStyle::PropertyType, _type);

		//
		// Очищаем цвета
		//
		format.setBackground(Qt::white);

		//
		// Оставляем только реальные отступы (отступы в строках будут преобразованы в пустые строки)
		//
		int topMargin = 0;
		int bottomMargin = 0;
		if (style.hasVerticalSpacingInMM()) {
			topMargin = PageMetrics::mmToPx(style.topMargin());
			bottomMargin = PageMetrics::mmToPx(style.bottomMargin());
		}
		format.setTopMargin(topMargin);
		format.setBottomMargin(bottomMargin);

		return format;
	}

	/**
	 * @brief Определить, нужно ли записать блок с заданным типом в результирующий файл
	 */
	static bool needPrintBlock(ScenarioBlockStyle::Type _blockType, bool _outline) {
		static QList<ScenarioBlockStyle::Type> s_outlinePrintableBlocksTypes =
			QList<ScenarioBlockStyle::Type>()
			<< ScenarioBlockStyle::SceneHeading
			<< ScenarioBlockStyle::SceneCharacters
			<< ScenarioBlockStyle::SceneGroupHeader
			<< ScenarioBlockStyle::SceneGroupFooter
			<< ScenarioBlockStyle::SceneDescription;

		static QList<ScenarioBlockStyle::Type> s_scenarioPrintableBlocksTypes =
			QList<ScenarioBlockStyle::Type>()
			<< ScenarioBlockStyle::SceneHeading
			<< ScenarioBlockStyle::SceneCharacters
			<< ScenarioBlockStyle::Action
			<< ScenarioBlockStyle::Character
			<< ScenarioBlockStyle::Dialogue
			<< ScenarioBlockStyle::Parenthetical
			<< ScenarioBlockStyle::Title
			<< ScenarioBlockStyle::Note
			<< ScenarioBlockStyle::Transition
			<< ScenarioBlockStyle::SceneGroupHeader
			<< ScenarioBlockStyle::SceneGroupFooter;

		return
				_outline
				? s_outlinePrintableBlocksTypes.contains(_blockType)
				: s_scenarioPrintableBlocksTypes.contains(_blockType);
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
	 * @brief Вставить строку с заданным форматированием
	 */
	static void insertLine(QTextCursor& _cursor, const QTextBlockFormat& _blockFormat,
		const QTextCharFormat& _charFormat) {
		_cursor.insertBlock(_blockFormat, _charFormat);
	}

	/**
	 * @brief Определить тип следующей строки документа
	 */
	static LineType currentLine(QTextDocument* _inDocument, const QTextBlockFormat& _blockFormat,
		const QTextCharFormat& _charFormat) {
		LineType type = UndefinedLine;

		if (_inDocument->isEmpty()) {
			type = FirstDocumentLine;
		} else {
			//
			// Определяем конец страницы или середина при помощи проверки
			// на количество страниц, после вставки новой строки
			//
			const int documentPagesCount = _inDocument->pageCount();
			QTextCursor cursor(_inDocument);
			cursor.movePosition(QTextCursor::End);
			insertLine(cursor, _blockFormat, _charFormat);
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
		const ExportParameters& _exportParameters)
{
	ScenarioTemplate exportStyle = ::exportStyle();

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
	// NOTE: делаем копию документа, т.к. данные могут быть изменены, удаляем, при выходе
	//
	QTextDocument* scenarioDocument = _scenario->document()->clone();
	//
	// ... копируем пользовательские данные из блоков
	//
	{
		QTextBlock sourceDocumentBlock = _scenario->document()->begin();
		QTextBlock copyDocumentBlock = scenarioDocument->begin();
		while (sourceDocumentBlock.isValid()) {
			if (ScenarioTextBlockInfo* sceneInfo = dynamic_cast<ScenarioTextBlockInfo*>(sourceDocumentBlock.userData())) {
				copyDocumentBlock.setUserData(sceneInfo->clone());
			}
			sourceDocumentBlock = sourceDocumentBlock.next();
			copyDocumentBlock = copyDocumentBlock.next();
		}
	}
	QTextCursor sourceDocumentCursor(scenarioDocument);
	QTextCursor destDocumentCursor(preparedDocument);


	//
	// Формирование титульной страницы
	//
	if (_exportParameters.printTilte) {
		QTextCharFormat titleFormat;
		titleFormat.setFont(QFont("Courier New", 12));
		QTextBlockFormat centerFormat;
		centerFormat.setAlignment(Qt::AlignCenter);
		centerFormat.setLineHeight(
					TextEditHelper::fontLineHeight(titleFormat.font()),
					QTextBlockFormat::FixedHeight);
		QTextBlockFormat rightFormat;
		rightFormat.setAlignment(Qt::AlignRight);
		rightFormat.setLineHeight(
					TextEditHelper::fontLineHeight(titleFormat.font()),
					QTextBlockFormat::FixedHeight);
		//
		// Номер текущей строки
		//
		int currentLineNumber = 1;
		destDocumentCursor.setBlockFormat(rightFormat);
		destDocumentCursor.setCharFormat(titleFormat);

		//
		// Название [13 строка]
		//
		while ((currentLineNumber++) < 12) {
			::insertLine(destDocumentCursor, centerFormat, titleFormat);
		}
		::insertLine(destDocumentCursor, centerFormat, titleFormat);
		destDocumentCursor.insertText(_exportParameters.scenarioName);
		//
		// Жанр [через одну под предыдущим]
		//
		if (!_exportParameters.scenarioGenre.isEmpty()) {
			::insertLine(destDocumentCursor, centerFormat, titleFormat);
			::insertLine(destDocumentCursor, centerFormat, titleFormat);
			destDocumentCursor.insertText(_exportParameters.scenarioGenre);
			currentLineNumber += 2;
		}
		//
		// Автор [через одну под предыдущим]
		//
		if (!_exportParameters.scenarioAuthor.isEmpty()) {
			::insertLine(destDocumentCursor, centerFormat, titleFormat);
			::insertLine(destDocumentCursor, centerFormat, titleFormat);
			destDocumentCursor.insertText(_exportParameters.scenarioAuthor);
			currentLineNumber += 2;
		}
		//
		// Доп. инфо [через одну под предыдущим]
		//
		if (!_exportParameters.scenarioAdditionalInfo.isEmpty()) {
			::insertLine(destDocumentCursor, centerFormat, titleFormat);
			::insertLine(destDocumentCursor, centerFormat, titleFormat);
			destDocumentCursor.insertText(_exportParameters.scenarioAdditionalInfo);
			currentLineNumber += 2;
		}
		//
		// необходимое количество пустых строк до 30ой
		//
		while ((currentLineNumber++) < 29) {
			::insertLine(destDocumentCursor, centerFormat, titleFormat);
		}
		//
		// Контакты [30 строка]
		//
		::insertLine(destDocumentCursor, rightFormat, titleFormat);
		destDocumentCursor.insertText(_exportParameters.scenarioContacts);

		//
		// Год печатается на последней строке документа
		//
		LineType currentLineType = ::currentLine(preparedDocument, centerFormat, titleFormat);
		while (currentLineType != LastPageLine) {
			++currentLineNumber;
			::insertLine(destDocumentCursor, centerFormat, titleFormat);
			currentLineType = ::currentLine(preparedDocument, centerFormat, titleFormat);
		}
		destDocumentCursor.insertText(_exportParameters.scenarioYear);
	}


	//
	// Запись текста документа
	//
	ScenarioBlockStyle::Type currentBlockType = ScenarioBlockStyle::Undefined;
	//
	// Кол-во пустых строк после предыдущего блока с текстом. Отслеживаем их, для случая, когда
	// подряд идут два блока у первого из которых есть отступ снизу, а у второго сверху. В таком
	// случае отступ должен не суммироваться, а стать наибольшим из имеющихся. Например у верхнего
	// отступ снизу 2 строки, а у нижнего отступ сверху 1 строка, отступ между ними должен быть
	// 2 строки.
	//
	int lastEmptyLines = 0;
	while (!sourceDocumentCursor.atEnd()) {
		QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

		//
		// Получим тип текущего блока под курсором
		//
		currentBlockType = ScenarioBlockStyle::forBlock(sourceDocumentCursor.block());

		//
		// Если блок содержит текст, который необходимо вывести на печать
		//
		if (::needPrintBlock(currentBlockType, _exportParameters.outline)) {

			//
			// Определим стили и настроим курсор
			//
			QTextBlockFormat blockFormat = ::blockFormatForType(currentBlockType);
			QTextCharFormat charFormat = ::charFormatForType(currentBlockType);

			//
			// Если вставляется не первый блок текста, возможно следует сделать отступы
			//
			{
				LineType currentLineType = ::currentLine(preparedDocument, blockFormat, charFormat);
				if (currentLineType == MiddlePageLine) {
					int emptyLines = exportStyle.blockStyle(currentBlockType).topSpace();
					//
					// Корректируем кол-во вставляемых строк в зависимости от предыдущего блока
					//
					if (lastEmptyLines > 0) {
						if (lastEmptyLines > emptyLines) {
							emptyLines = 0;
						} else {
							emptyLines -= lastEmptyLines;
						}
					}

					//
					// ... выполняется до тех пор, пока не будут вставлены все строки,
					//	   или не будет достигнут конец страницы
					//
					while (emptyLines-- > 0
						   && currentLineType == MiddlePageLine) {
						//
						// ... вставим линию и настроим её стиль
						//
						::insertLine(destDocumentCursor, blockFormat, charFormat);
						currentLineType = ::currentLine(preparedDocument, blockFormat, charFormat);
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
				LineType currentLineType = ::currentLine(preparedDocument, blockFormat, charFormat);
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
				if (currentBlockType == ScenarioBlockStyle::SceneHeading) {
					//
					// Данные сцены
					//
					QTextBlockUserData* textBlockData = sourceDocumentCursor.block().userData();
					ScenarioTextBlockInfo* sceneInfo = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
					if (sceneInfo != 0) {
						destDocumentCursor.block().setUserData(sceneInfo->clone());
					}

					//
					// Префикс экспорта
					//
					destDocumentCursor.insertText(_exportParameters.scenesPrefix);
					//
					// Номер сцены, если необходимо
					//
					if (_exportParameters.printScenesNumbers) {
						if (sceneInfo != 0) {
							QString sceneNumber = QString("%1. ").arg(sceneInfo->sceneNumber());
							destDocumentCursor.insertText(sceneNumber);
						}
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

				//
				// Добавляем редакторские пометки, если необходимо
				//
				if (_exportParameters.saveReviewMarks) {
					//
					// Если в блоке есть пометки, добавляем их
					//
					if (!sourceDocumentCursor.block().textFormats().isEmpty()) {
						const int startBlockPosition = destDocumentCursor.block().position();
						foreach (const QTextLayout::FormatRange& range, sourceDocumentCursor.block().textFormats()) {
							if (range.format.boolProperty(ScenarioBlockStyle::PropertyIsReviewMark)) {
								destDocumentCursor.setPosition(startBlockPosition + range.start);
								destDocumentCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, range.length);
								destDocumentCursor.mergeCharFormat(range.format);
							}
						}
						destDocumentCursor.movePosition(QTextCursor::EndOfBlock);
					}
				}
			}

			//
			// После текста, так же возможно следует сделать отступы
			//
			{
				LineType currentLineType = ::currentLine(preparedDocument, blockFormat, charFormat);
				if (currentLineType == MiddlePageLine) {
					int emptyLines = exportStyle.blockStyle(currentBlockType).bottomSpace();
					//
					// ... сохраним кол-во пустых строк в последнем блоке
					//
					lastEmptyLines = emptyLines;
					//
					// ... выполняется до тех пор, пока не будут вставлены все строки,
					//	   или не будет достигнут конец страницы
					//
					while (emptyLines-- > 0
						   && currentLineType == MiddlePageLine) {
						//
						// ... вставим линию и настроим её стиль
						//
						::insertLine(destDocumentCursor, blockFormat, charFormat);
						currentLineType = ::currentLine(preparedDocument, blockFormat, charFormat);
					}
				}
			}
		}

		//
		// Переходим к следующему блоку
		//
		sourceDocumentCursor.movePosition(QTextCursor::EndOfBlock);
		sourceDocumentCursor.movePosition(QTextCursor::NextBlock);
	}

	//
	// Удаляем копию документа с текстом сценария
	//
	delete scenarioDocument;
	scenarioDocument = 0;

	return preparedDocument;
}
