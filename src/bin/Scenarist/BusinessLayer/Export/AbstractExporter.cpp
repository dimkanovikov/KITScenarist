#include "AbstractExporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextDocument.h>

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
		ScenarioTemplate exportStyle = ::exportStyle();
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
		ScenarioBlockStyle style = ::exportStyle().blockStyle(_type);
		QTextBlockFormat format = style.blockFormat();

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
			::insertLine(cursor, _blockFormat, _charFormat);
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

	/**
	 * @brief Оптимизированная версия подсчёта кол-ва строк до конца страницы
	 */
	static int linesToEndOfPage(QTextDocument* _inDocument, const QTextBlockFormat& _blockFormat,
		const QTextCharFormat& _charFormat, const int _blockLines) {
		//
		// Минимальное количество строк для проверки
		//
		const int MINIMUM_LINES_FOR_CHECK = 3;

		int result = 0;

		//
		// Считаем количество строк
		//
		QTextCursor cursor(_inDocument);
		cursor.movePosition(QTextCursor::End);
		// ... количество страниц в исходном документе
		const int documentPagesCount = _inDocument->pageCount();
		// ... верхняя граница количества строк для проверки
		const int checkLimit =
				(_blockLines < MINIMUM_LINES_FOR_CHECK) ? MINIMUM_LINES_FOR_CHECK : _blockLines;
		// ... тип текущей линии
		LineType type = UndefinedLine;

		while (type != LastPageLine
			   && result <= checkLimit) {

			::insertLine(cursor, _blockFormat, _charFormat);
			++result;

			if (documentPagesCount == _inDocument->pageCount()) {
				type = MiddlePageLine;
			} else {
				type = LastPageLine;
				cursor.deletePreviousChar();
				--result;
			}
		}

		//
		// Возвращаем документ в исходное состояние
		//
		int deleteLines = result;
		while (deleteLines-- > 0)  {
			cursor.deletePreviousChar();
		}

		return result;
	}

	/**
	 * @brief Поссчитать кол-во строк, занимаемых текстом
	 */
	static int linesOfText(QTextDocument* _inDocument, const QTextBlockFormat& _blockFormat,
		const QTextCharFormat& _charFormat, const QString& _text) {
		//
		// Определим ширину линии текста
		//
		const qreal lineWidth =
				_inDocument->size().width()
				- _inDocument->rootFrame()->frameFormat().leftMargin()
				- _inDocument->rootFrame()->frameFormat().rightMargin()
				- _blockFormat.leftMargin()
				- _blockFormat.rightMargin();
		//
		// Шрифт текста
		//
		const QFont font = _charFormat.font();
		//
		// Посчитаем кол-во линий
		//
		QTextLayout textLayout(_text);
		textLayout.setFont(font);
		textLayout.beginLayout();
		int linesCount = 0;
		forever {
			QTextLine line = textLayout.createLine();
			if (!line.isValid()) {
				break;
			} else {
				++linesCount;
			}

			line.setLineWidth(lineWidth);
		}
		textLayout.endLayout();

		return linesCount;
	}

	/**
	 * @brief Поссчитать кол-во строк, занимаемых текущим блоком
	 */
	static int linesOfText(const QTextCursor& _cursor) {
		return ::linesOfText(_cursor.document(), _cursor.blockFormat(), _cursor.charFormat(), _cursor.block().text());
	}

	// ********
	// Методы проверки переносов строк
	//

	/**
	 * @brief Вспомогательные слова на разрывах реплик
	 */
	static const char* DIALOG_BREAK = QT_TRANSLATE_NOOP("BusinessLogic::AbstractExporter", "(MORE)");
	static const char* DIALOG_CONTINUED = QT_TRANSLATE_NOOP("BusinessLogic::AbstractExporter", " (cont'd)");

	/**
	 * @brief Разбить текст по предложениям
	 */
	static QStringList splitTextToSentences(const QString& _text) {
		const QList<QChar> punctuation = QList<QChar>() << '.' << '!' << '?' << QString("…").at(0);
		QStringList result;
		int lastSentenceEnd = 0;
		for (int charIndex = 0; charIndex < _text.length(); ++charIndex) {
			//
			// Если символ - пунктуация, разрываем
			//
			if (punctuation.contains(_text.at(charIndex))) {
				++charIndex;
				result.append(_text.mid(lastSentenceEnd, charIndex - lastSentenceEnd).simplified());
				lastSentenceEnd = charIndex;
			}
		}

		return result;
	}

	/**
	 * @brief Проверка переноса блоков "Время и место" и "Заголовок группы сцен" на разрыве страниц
	 *
	 * Правила:
	 * - нельзя оставлять на последней строке
	 * - нельзя оставлять на предпоследней строке, т.к. после может идти
	 *   блок с одной пустой строкой
	 */
	static void checkPageBreakForTimeAndPlace(QTextCursor& _destDocumentCursor,
		const QTextBlockFormat& _blockFormat, const QTextCharFormat& _charFormat,
		const int _blockLines, const int _linesToEndOfPage) {
		//
		// ... вставляем столько пустых строк, чтобы перейти на следующую страницу
		//
		if (_linesToEndOfPage <= qMax(2, _blockLines)) {
			int insertLines = _linesToEndOfPage;
			while (insertLines-- > 0) {
				::insertLine(_destDocumentCursor, _blockFormat, _charFormat);
			}
		}
	}

	/**
	 * @brief Проверка переноса блока "Имя персонажа" на разрыве страниц
	 *
	 * Правила:
	 * - нельзя оставлять на последней строке
	 */
	static void checkPageBreakForCharacter(QTextCursor& _destDocumentCursor,
		const QTextBlockFormat& _blockFormat, const QTextCharFormat& _charFormat,
		const int _blockLines, const int _linesToEndOfPage) {
		//
		// ... вставляем пустую строку, чтобы перейти на следующую страницу
		//
		if (_linesToEndOfPage <= _blockLines) {
			int insertLines = _linesToEndOfPage;
			while (insertLines-- > 0) {
				::insertLine(_destDocumentCursor, _blockFormat, _charFormat);
			}
		}
	}

	/**
	 * @brief Проверка переноса блока "Описание действия" на разрыве страниц
	 *
	 * Правила:
	 * - минимум три строчки оставить на предыдущей странице, разделяется по точке
	 * - если блок переносится полностью, то предыдущий блок время и место переносится тоже
	 */
	static void checkPageBreakForAction(QTextCursor& _sourceDocumentCursor,
		QTextCursor& _destDocumentCursor, const QTextBlockFormat& _blockFormat,
		const QTextCharFormat& _charFormat, const int _blockLines, const int _linesToEndOfPage) {
		//
		// Получим необходимые для работы параметры
		//
		// ... тип текущего блока под курсором
		ScenarioBlockStyle::Type currentBlockType = ScenarioBlockStyle::forBlock(_sourceDocumentCursor.block());
		// ... документ в который осуществляется вывод
		QTextDocument* preparedDocument = _destDocumentCursor.document();

		//
		// Минимальное количество строк, что можно оставить на предыдущей странице
		//
		const int MINIMUM_LINES_AT_PAGE_END = 3;

		//
		// ... если не влезает до конца страницы
		//
		if (_blockLines > _linesToEndOfPage) {
			//
			// ... если занимает меньше трёх строк, то переносим полностью
			//
			if (_blockLines <= MINIMUM_LINES_AT_PAGE_END
				|| _linesToEndOfPage <= MINIMUM_LINES_AT_PAGE_END) {
				//
				// ... сколько строк нужно вставить
				//
				int insertLines = _linesToEndOfPage;
				//
				// ... если предыдущим блоком является время и место переносим их вместе
				//
				ScenarioBlockStyle::Type prevBlockType =
						ScenarioBlockStyle::forBlock(_sourceDocumentCursor.block().previous());
				if (prevBlockType == ScenarioBlockStyle::TimeAndPlace) {
					//
					// ... пустые строки
					//
					int emptyLines = ::exportStyle().blockStyle(currentBlockType).topSpace();
					_destDocumentCursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, emptyLines);
					insertLines += emptyLines;
					//
					// ... переходим в начало блока время и место
					//
					_destDocumentCursor.movePosition(QTextCursor::StartOfBlock);
					++insertLines;
				}
				//
				// ... переходим до следующей страницы
				//
				while (insertLines-- > 0) {
					::insertLine(_destDocumentCursor, _blockFormat, _charFormat);
				}
				//
				// ... переводим курсор в конец
				//
				_destDocumentCursor.movePosition(QTextCursor::End);
			}
			//
			// ... Если больше, то пробуем разорвать блок, так, чтобы влезли три строки
			//
			else {
				//
				// ... разрываем предложения по точкам
				//
				QStringList sentences = ::splitTextToSentences(_sourceDocumentCursor.block().text());
				//
				// ... пробуем сформировать текст, который влезет в конец страницы
				//
				QStringList nextPageSentences;
				while (!sentences.isEmpty()) {
					nextPageSentences.prepend(sentences.takeLast());
					int lines = ::linesOfText(preparedDocument, _blockFormat, _charFormat, sentences.join(" "));

					//
					// ... если нашли текст, который влезет в конец страницы, то
					//	   разрываем блок текста исходного документа
					//
					if (lines <= _linesToEndOfPage) {
						QString curBlockText = sentences.join(" ");
						_sourceDocumentCursor.movePosition(QTextCursor::StartOfBlock);
						_sourceDocumentCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
						_sourceDocumentCursor.insertText(curBlockText);
						_sourceDocumentCursor.insertBlock();
						_sourceDocumentCursor.insertText(nextPageSentences.join(" "));
						_sourceDocumentCursor.movePosition(QTextCursor::PreviousBlock);
						break;
					}
				}
			}
		}
	}

	/**
	 * @brief Проверка переноса блока "Ремарка" на разрыве страниц
	 *
	 * Правила:
	 * - нельзя оставлять на последней строке
	 *
	 * Вставляем блок по правилам ремарки с содержанием "ДАЛЬШЕ", а саму ремарку
	 * переносим на следующую страницу, вставляя блок с именем персонажа, его
	 * состоянием "(ЗК)" и далее в скобках "(ПРОД.)"
	 */
	static void checkPageBreakForParenthetical(QTextCursor& _sourceDocumentCursor,
		QTextCursor& _destDocumentCursor, const QTextBlockFormat& _blockFormat,
		const QTextCharFormat& _charFormat, const int _blockLines, const int _linesToEndOfPage) {
		if (_linesToEndOfPage <= _blockLines) {
			//
			// ... ищем имя персонажа
			//
			QTextCursor characterCursor = _sourceDocumentCursor;
			characterCursor.movePosition(QTextCursor::PreviousBlock);

			//
			// ... если предыдущим блоком является персонаж, то переносим его вместе с
			// текущим блоком ремарки
			//
			if (ScenarioBlockStyle::forBlock(characterCursor.block()) == ScenarioBlockStyle::Character) {
				_destDocumentCursor.movePosition(QTextCursor::PreviousBlock);
				_destDocumentCursor.movePosition(QTextCursor::StartOfBlock);
				//
				// ... нужно перенести строки с именем персонажа и ремаркой
				//
				int insertLines = _linesToEndOfPage + ::linesOfText(characterCursor);
				while (insertLines-- > 0) {
					_destDocumentCursor.insertBlock();
				}
				_destDocumentCursor.movePosition(QTextCursor::End);
			}
			//
			// ... в противном случае будем вставлять ДАЛЬШЕ
			//
			else {
				//
				// ... продолжаем искать имя персонажа
				//
				while (ScenarioBlockStyle::forBlock(characterCursor.block()) != ScenarioBlockStyle::Character) {
					characterCursor.movePosition(QTextCursor::PreviousBlock);
				}

				//
				// ... вставляем строку ДАЛЬШЕ
				//
				::insertLine(_destDocumentCursor, _blockFormat, _charFormat);
				_destDocumentCursor.insertText(QApplication::translate("BusinessLogic::AbstractExporter", DIALOG_BREAK));
				//
				// ... вставляем блок персонажа
				//
				::insertLine(_destDocumentCursor, characterCursor.blockFormat(), characterCursor.charFormat());
				_destDocumentCursor.insertText(characterCursor.block().text().toUpper());
				//
				// ... и приписку (ПРОД.)
				//
				_destDocumentCursor.insertText(QApplication::translate("BusinessLogic::AbstractExporter", DIALOG_CONTINUED));
			}
		}
	}

	/**
	 * @brief Проверка переноса блока "Реплика" на разрыве страниц
	 *
	 * Правила:
	 * - нельзя чтобы следущая страница начиналась ремаркой или репликой
	 * - минимум две строчки оставить на предыдущей странице, разделяется по точке
	 * - если блок переносится полностью, то предыдущие блоки персонаж и ремарка
	 *   переносятся тоже
	 *
	 * Вставляем блок по правилам ремарки с содержанием "ДАЛЬШЕ", а саму ремарку
	 * переносим на следующую страницу, вставляя блок с именем персонажа, его
	 * состоянием "(ЗК)" и далее в скобках "(ПРОД.)"
	 */
	static void checkPageBreakForDialog(QTextCursor& _sourceDocumentCursor,
		QTextCursor& _destDocumentCursor, const QTextBlockFormat& _blockFormat,
		const QTextCharFormat& _charFormat, const int _blockLines, const int _linesToEndOfPage) {
		//
		// Получим необходимые для работы параметры
		//
		// ... тип текущего блока под курсором
		ScenarioBlockStyle::Type currentBlockType = ScenarioBlockStyle::forBlock(_sourceDocumentCursor.block());
		// ... тип следующего блока
		ScenarioBlockStyle::Type nextBlockType = ScenarioBlockStyle::forBlock(_sourceDocumentCursor.block().next());
		// ... документ в который осуществляется вывод
		QTextDocument* preparedDocument = _destDocumentCursor.document();

		//
		// ... проверяем необходимость обработать последующую ремарку или реплику
		//
		const bool nextDialogOrParentheticalOnNewPage =
				_blockLines == _linesToEndOfPage
				&& (nextBlockType == ScenarioBlockStyle::Parenthetical
					|| nextBlockType == ScenarioBlockStyle::Dialog	);
		//
		// ... и случай, если не влезает до конца страницы, разорвать блок
		//
		const bool blockDontFitInPageEnd = _blockLines > _linesToEndOfPage;
		if (nextDialogOrParentheticalOnNewPage || blockDontFitInPageEnd) {

			const int MINIMUM_LINES_AT_PAGE_END = 2;
			//
			// ... если занимает меньше двух строк, то переносим полностью
			//
			if (_blockLines <= MINIMUM_LINES_AT_PAGE_END
				|| _linesToEndOfPage <= MINIMUM_LINES_AT_PAGE_END) {
				//
				// ... сколько строк нужно вставить
				//
				int insertLines = _linesToEndOfPage;
				//
				// ... если предыдущим блоком является ремарка или персонаж переносим их вместе
				//
				QTextBlock prevBlock = _sourceDocumentCursor.block().previous();
				ScenarioBlockStyle::Type prevBlockType = ScenarioBlockStyle::forBlock(prevBlock);
				//
				// ... ремарка
				//
				if (prevBlockType == ScenarioBlockStyle::Parenthetical) {
					//
					// ... пустые строки
					//
					int emptyLines = ::exportStyle().blockStyle(currentBlockType).topSpace();
					_destDocumentCursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, emptyLines);
					insertLines += emptyLines;
					//
					// ... строки абзаца
					//
					insertLines += ::linesOfText(_destDocumentCursor);
					//
					// ... переходим в конец блока перед ремаркой
					//
					_destDocumentCursor.movePosition(QTextCursor::PreviousBlock);
					_destDocumentCursor.movePosition(QTextCursor::EndOfBlock);

					//
					// ... проверяем предпредыдущий блок
					//
					prevBlock = prevBlock.previous();
					prevBlockType = ScenarioBlockStyle::forBlock(prevBlock);
				}
				//
				// ... персонаж
				//
				if (prevBlockType == ScenarioBlockStyle::Character) {
					//
					// ... пустые строки
					//
					int emptyLines = ::exportStyle().blockStyle(currentBlockType).topSpace();
					_destDocumentCursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, emptyLines);
					insertLines += emptyLines;
					//
					// ... строки абзаца
					//
					insertLines += ::linesOfText(_destDocumentCursor);
					//
					// ... переходим в конец блока перед персонажем
					//
					_destDocumentCursor.movePosition(QTextCursor::PreviousBlock);
					_destDocumentCursor.movePosition(QTextCursor::EndOfBlock);
				}

				//
				// Если предыдущим блоком не является персонаж, вставляем ДАЛЬШЕ
				//
				if (prevBlockType != ScenarioBlockStyle::Character) {
					::insertLine(_destDocumentCursor,
						::blockFormatForType(ScenarioBlockStyle::Parenthetical),
						::charFormatForType(ScenarioBlockStyle::Parenthetical));
					_destDocumentCursor.insertText(QApplication::translate("BusinessLogic::AbstractExporter", DIALOG_BREAK));
					--insertLines;
				}

				//
				// ... переходим до следующей страницы
				//
				while (insertLines-- > 0) {
					::insertLine(_destDocumentCursor, _blockFormat, _charFormat);
				}
				//
				// ... переводим курсор в конец
				//
				_destDocumentCursor.movePosition(QTextCursor::End);

				//
				// Если предыдущим блоком не был персонаж, вставим его имя и (ПРОД.)
				//
				if (prevBlockType != ScenarioBlockStyle::Character) {
					//
					// ... шаг наверх над предыдущим блоком, если это реплика после реплики
					//
					if (prevBlock != _sourceDocumentCursor.block().previous()) {
						_destDocumentCursor.movePosition(QTextCursor::PreviousBlock);
						_destDocumentCursor.movePosition(QTextCursor::EndOfBlock);
					}

					//
					// ... найдём персонажа
					//
					while (prevBlock.isValid()
						   && ScenarioBlockStyle::forBlock(prevBlock) != ScenarioBlockStyle::Character) {
						prevBlock = prevBlock.previous();
					}

					//
					// ... вставляем (ПРОД.)
					//
					::insertLine(_destDocumentCursor,
						::blockFormatForType(ScenarioBlockStyle::Character),
						::charFormatForType(ScenarioBlockStyle::Character));
					_destDocumentCursor.insertText(prevBlock.text().toUpper());
					_destDocumentCursor.insertText(QApplication::translate("BusinessLogic::AbstractExporter", DIALOG_CONTINUED));

					//
					// ... переводим курсор в конец
					//
					_destDocumentCursor.movePosition(QTextCursor::End);
				}
			}
			//
			// ... Если больше, то пробуем разорвать блок, так, чтобы влезли две строки
			//
			else {
				//
				// ... разрываем предложения по точкам
				//
				QStringList sentences = ::splitTextToSentences(_sourceDocumentCursor.block().text());
				//
				// ... пробуем сформировать текст, который влезет в конец страницы
				//
				QStringList nextPageSentences;
				while (!sentences.isEmpty()) {
					nextPageSentences.prepend(sentences.takeLast());
					int lines = ::linesOfText(preparedDocument, _blockFormat, _charFormat, sentences.join(" "));
					//
					// Учитываем добавочную строку ДАЛЬШЕ, если абзац не пуст
					//
					if (sentences.isEmpty()) {
						lines = 0;
					} else {
						lines += 1;
					}

					//
					// Если нужно перенести весь блок
					//
					if (lines == 0) {
						QTextBlock prevBlock = _sourceDocumentCursor.block().previous();
						ScenarioBlockStyle::Type prevBlockType = ScenarioBlockStyle::forBlock(prevBlock);
						//
						// ... это тот случай, когда мы уже разорвали
						// блок реплики и нужно вставить "прод." и "дальше"
						//
						if (prevBlockType == ScenarioBlockStyle::Dialog) {
							//
							// ДАЛЬШЕ
							//
							::insertLine(_destDocumentCursor,
								::blockFormatForType(ScenarioBlockStyle::Parenthetical),
								::charFormatForType(ScenarioBlockStyle::Parenthetical));
							_destDocumentCursor.insertText(QApplication::translate("BusinessLogic::AbstractExporter", DIALOG_BREAK));

							//
							// ... идём до следующей страницы
							//
							int toEndOfPage = _linesToEndOfPage - 1;
							while (toEndOfPage-- > 0) {
								::insertLine(_destDocumentCursor, _blockFormat, _charFormat);
							}

							//
							// (ПРОД.)
							//
							// ... найдём персонажа
							//
							QTextBlock prevBlock = _sourceDocumentCursor.block().previous();
							while (prevBlock.isValid()
								   && ScenarioBlockStyle::forBlock(prevBlock) != ScenarioBlockStyle::Character) {
								prevBlock = prevBlock.previous();
							}

							//
							// ... вставляем (ПРОД.)
							//
							::insertLine(_destDocumentCursor,
								::blockFormatForType(ScenarioBlockStyle::Character),
								::charFormatForType(ScenarioBlockStyle::Character));
							_destDocumentCursor.insertText(prevBlock.text().toUpper());
							_destDocumentCursor.insertText(QApplication::translate("BusinessLogic::AbstractExporter", DIALOG_CONTINUED));
						}
						//
						// ... перенос, совместно с ремаркой и именем персонажа
						//
						else {
							//
							// ... сколько строк нужно вставить
							//
							int insertLines = _linesToEndOfPage;
							//
							// ... ремарка
							//
							if (prevBlockType == ScenarioBlockStyle::Parenthetical) {
								//
								// ... пустые строки
								//
								int emptyLines = ::exportStyle().blockStyle(currentBlockType).topSpace();
								_destDocumentCursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, emptyLines);
								insertLines += emptyLines;
								//
								// ... переходим в конец блока перед ремаркой
								//
								_destDocumentCursor.movePosition(QTextCursor::PreviousBlock);
								_destDocumentCursor.movePosition(QTextCursor::EndOfBlock);
								++insertLines;

								//
								// ... проверяем предпредыдущий блок
								//
								prevBlock = prevBlock.previous();
								prevBlockType = ScenarioBlockStyle::forBlock(prevBlock);
							}
							//
							// ... персонаж
							//
							if (prevBlockType == ScenarioBlockStyle::Character) {
								//
								// ... пустые строки
								//
								int emptyLines = ::exportStyle().blockStyle(currentBlockType).topSpace();
								_destDocumentCursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, emptyLines);
								insertLines += emptyLines;
								//
								// ... переходим в конец блока перед персонажем
								//
								_destDocumentCursor.movePosition(QTextCursor::PreviousBlock);
								_destDocumentCursor.movePosition(QTextCursor::EndOfBlock);
								++insertLines;
							}

							//
							// Если предыдущим блоком не является персонаж, вставляем ДАЛЬШЕ
							//
							if (prevBlockType != ScenarioBlockStyle::Character) {
								::insertLine(_destDocumentCursor,
									::blockFormatForType(ScenarioBlockStyle::Parenthetical),
									::charFormatForType(ScenarioBlockStyle::Parenthetical));
								_destDocumentCursor.insertText(QApplication::translate("BusinessLogic::AbstractExporter", DIALOG_BREAK));
								--insertLines;
							}

							//
							// ... переходим до следующей страницы
							//
							while (insertLines-- > 0) {
								::insertLine(_destDocumentCursor, _blockFormat, _charFormat);
							}
							//
							// ... переводим курсор в конец
							//
							_destDocumentCursor.movePosition(QTextCursor::End);
						}
					}
					//
					// ... если нашли текст, который влезет в конец страницы, то
					//	   разрываем блок текста исходного документа.
					//
					else if (lines <= _linesToEndOfPage) {
						QString curBlockText = sentences.join(" ");
						//
						// ... текст окончания страницы
						//
						_sourceDocumentCursor.movePosition(QTextCursor::StartOfBlock);
						_sourceDocumentCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
						_sourceDocumentCursor.insertText(curBlockText);
						//
						// ... остальной текст
						//
						::insertLine(_sourceDocumentCursor, _blockFormat, _charFormat);
						_sourceDocumentCursor.insertText(nextPageSentences.join(" "));
						_sourceDocumentCursor.movePosition(QTextCursor::PreviousBlock);
						break;
					}
				}
			}
		}
	}

	/**
	 * @brief Проверить переносы текста на разрывах страниц и в случае необходимости их корректировка
	 * @param Курсор в исходном документа
	 * @param Крсор в целевом документе
	 */
	static void checkPageBreak(QTextCursor& _sourceDocumentCursor, QTextCursor& _destDocumentCursor) {
		//
		// Получим необходимые для работы параметры
		//
		// ... тип текущего блока под курсором
		ScenarioBlockStyle::Type currentBlockType = ScenarioBlockStyle::forBlock(_sourceDocumentCursor.block());
		// ... определим стили
		QTextBlockFormat blockFormat = ::blockFormatForType(currentBlockType);
		QTextCharFormat charFormat = ::charFormatForType(currentBlockType);
		// ... документ в который осуществляется вывод
		QTextDocument* preparedDocument = _destDocumentCursor.document();


		//
		// Посчитаем сколько строк до конца страницы и сколько строк в блоке
		//
		const int blockLines = ::linesOfText(preparedDocument, blockFormat, charFormat, _sourceDocumentCursor.block().text());
		const int linesToEndOfPage = ::linesToEndOfPage(preparedDocument, blockFormat, charFormat, blockLines);

		//
		// Для блоков "Время и место" и "Группа сцен"
		//
		if (currentBlockType == ScenarioBlockStyle::TimeAndPlace
			|| currentBlockType == ScenarioBlockStyle::SceneGroupHeader) {
			checkPageBreakForTimeAndPlace(_destDocumentCursor, blockFormat, charFormat, blockLines, linesToEndOfPage);
		}

		//
		// Для блока "Персонаж"
		//
		else if (currentBlockType == ScenarioBlockStyle::Character) {
			checkPageBreakForCharacter(_destDocumentCursor, blockFormat, charFormat, blockLines, linesToEndOfPage);
		}

		//
		// Для блока "Описание действия"
		//
		else if (currentBlockType == ScenarioBlockStyle::Action) {
			checkPageBreakForAction(_sourceDocumentCursor, _destDocumentCursor, blockFormat,
				charFormat, blockLines, linesToEndOfPage);
		}

		//
		// Для блока "Ремарка"
		//
		else if (currentBlockType == ScenarioBlockStyle::Parenthetical) {
			checkPageBreakForParenthetical(_sourceDocumentCursor, _destDocumentCursor,
				blockFormat, charFormat, blockLines, linesToEndOfPage);
		}

		//
		// Для блока "Реплика"
		//
		else if (currentBlockType == ScenarioBlockStyle::Dialog) {
			checkPageBreakForDialog(_sourceDocumentCursor, _destDocumentCursor, blockFormat,
				charFormat, blockLines, linesToEndOfPage);
		}
	}
}

QTextDocument* AbstractExporter::prepareDocument(const BusinessLogic::ScenarioDocument* _scenario,
		const ExportParameters& _exportParameters) const
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
	int currentSceneNumber = 1;
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
		if (currentBlockType != ScenarioBlockStyle::NoprintableText
			&& currentBlockType != ScenarioBlockStyle::FolderHeader
			&& currentBlockType != ScenarioBlockStyle::FolderFooter) {

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
			// Проверяем разрывы страниц, на корректность переноса
			//
			if (_exportParameters.checkPageBreaks) {
				::checkPageBreak(sourceDocumentCursor, destDocumentCursor);
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
				if (currentBlockType == ScenarioBlockStyle::TimeAndPlace) {
					//
					// Префикс экспорта
					//
					destDocumentCursor.insertText(_exportParameters.scenesPrefix);
					//
					// Номер сцены, если необходимо
					//
					if (_exportParameters.printScenesNumbers) {
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
