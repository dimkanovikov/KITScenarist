#include "ScenarioTextCorrector.h"

#include "ScenarioTemplate.h"
#include "ScenarioTextBlockParsers.h"

#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QDebug>
#include <QFontMetricsF>
#include <QPair>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

using namespace BusinessLogic;

namespace {
	/**
	 * @brief Автоматически добавляемые продолжения в диалогах
	 */
	//: Continued
	static const char* DIALOG_CONTINUED = QT_TRANSLATE_NOOP("BusinessLogic::ScenarioTextCorrector", "CONT'D");

	/**
	 * @brief Курсор находится на границе сцены
	 */
	static bool cursorAtSceneBorder(const QTextCursor& _cursor) {
		return ScenarioBlockStyle::forBlock(_cursor.block()) == ScenarioBlockStyle::SceneHeading
				|| ScenarioBlockStyle::forBlock(_cursor.block()) == ScenarioBlockStyle::SceneGroupHeader
				|| ScenarioBlockStyle::forBlock(_cursor.block()) == ScenarioBlockStyle::SceneGroupFooter
				|| ScenarioBlockStyle::forBlock(_cursor.block()) == ScenarioBlockStyle::FolderHeader
				|| ScenarioBlockStyle::forBlock(_cursor.block()) == ScenarioBlockStyle::FolderFooter;
	}

	/**
	 * @brief BlockPages
	 */
	class BlockInfo
	{
	public:
		BlockInfo() : topPage(0), bottomPage(0), topLinesCount(0), bottomLinesCount(0), width(0) {}
		BlockInfo(int _top, int _bottom, int _topLines, int _bottomLines, int _width)
			: topPage(_top), bottomPage(_bottom), topLinesCount(_topLines),
			  bottomLinesCount(_bottomLines), width(_width)
		{}

		/**
		 * @brief Страница, на которой начинается блок
		 */
		int topPage;

		/**
		 * @brief Стрница, на которой заканчивается блок
		 */
		int bottomPage;

		/**
		 * @brief Количество строк, занимаемых блоком на начальной странице
		 */
		int topLinesCount;

		/**
		 * @brief Количество строк, занимаемых блоком на конечной странице
		 */
		int bottomLinesCount;

		/**
		 * @brief Ширина блока
		 */
		int width;
	};

	/**
	 * @brief Получить номер страницы, на которой находится начало и конец блока
	 */
	static BlockInfo blockInfo(const QTextBlock& _block) {
		QAbstractTextDocumentLayout* layout = _block.document()->documentLayout();
		const QRectF blockRect = layout->blockBoundingRect(_block);
		const qreal pageHeight = _block.document()->pageSize().height();
		int topPage = blockRect.top() / pageHeight;
		int bottomPage = blockRect.bottom() / pageHeight;

		//
		// Если хотя бы одна строка не влезает в конце текущей страницы,
		// то значит блок будет располагаться на следующей странице
		//
		const qreal positionOnTopPage = blockRect.top() - (topPage * pageHeight);
		const qreal pageBottomMargin = _block.document()->rootFrame()->frameFormat().bottomMargin();
		const qreal lineHeight = _block.blockFormat().lineHeight();
		if (pageHeight - positionOnTopPage - pageBottomMargin < lineHeight) {
			if (topPage == bottomPage) {
				++topPage;
				++bottomPage;
			} else {
				++topPage;
			}
		}

		//
		// Определим кол-во строк на страницах
		//
		int topLinesCount = (pageHeight - positionOnTopPage - pageBottomMargin) / lineHeight;
		int bottomLinesCount = 0;
		if (topPage == bottomPage) {
			bottomLinesCount = blockRect.height() / lineHeight - topLinesCount;
		} else {
			const qreal pagesInterval =
				pageBottomMargin + _block.document()->rootFrame()->frameFormat().topMargin();
			bottomLinesCount = (blockRect.height() - pagesInterval) / lineHeight - topLinesCount;
		}

		//
		// NOTE: Номера страниц ночинаются с нуля
		//
		return BlockInfo(topPage, bottomPage, topLinesCount, bottomLinesCount, blockRect.width());
	}

	/**
	 * @brief Определить, сколько нужно вставить пустых блоков, для переноса заданного количества
	 *		  строк с текстом для блока с этим форматом
	 */
	static int neededEmptyBlocks(const QTextBlockFormat& _format, int _linesWithText) {
		int result = 0;
		//
		// Если есть хоть один отступ, то получается, что нужно пустых блоков в два раза меньше,
		// чем строк, при том округляя всегда в большую сторону
		//
		if (_format.topMargin() > 0 || _format.bottomMargin() > 0) {
			result = _linesWithText / 2 + (_linesWithText % 2 > 0 ? 1 : 0);
		}
		//
		// Если отступов нет, нужно столько же блоков, сколько и строк
		//
		else {
			result = _linesWithText;
		}
		return result;
	}

	/**
	 * @brief Узнать сколько строк занимает текст
	 */
	static int linesCount(const QString _text, const QFont& _font, int _width) {
		static QTextLayout textLayout;

		textLayout.clearLayout();
		textLayout.setText(_text);
		textLayout.setFont(_font);

		textLayout.beginLayout();
		forever {
			QTextLine line = textLayout.createLine();
			if (!line.isValid()) {
				break;
			}

			line.setLineWidth(_width);
		}
		textLayout.endLayout();

		return textLayout.lineCount();
	}
}


ScenarioTextCorrector::ScenarioTextCorrector()
{

}

void ScenarioTextCorrector::correctScenarioText(QTextDocument* _document, int _startPosition)
{
	//
	// Вводим карту с текстами обрабатываемых документов, чтобы не выполнять лишнюю работу,
	// повторно обрабатывая один и тот же документ
	//
	static QMap<QTextDocument*, QString> s_documentTexts;
	if (s_documentTexts.contains(_document)
		&& s_documentTexts.value(_document) == _document->toHtml()) {
		return;
	}

	//
	// Защищаемся от рекурсии
	//
	static bool s_proccessedNow = false;
	if (s_proccessedNow == false) {
		s_proccessedNow = true;

		QTextCursor mainCursor(_document);
		mainCursor.setPosition(_startPosition);

		//
		// Сперва нужно подняться до начала сцены и начинать корректировки с этого положения
		//
		while (!mainCursor.atStart()
			   && !::cursorAtSceneBorder(mainCursor)) {
			mainCursor.movePosition(QTextCursor::PreviousBlock);
			mainCursor.movePosition(QTextCursor::StartOfBlock);
		}


		//
		// Для имён персонажей, нужно добавлять ПРОД (только, если имя полностью идентично предыдущему)
		//
		{
			QTextCursor cursor = mainCursor;
			cursor.beginEditBlock();

			//
			// Храним последнего персонажа сцены
			//
			QString lastSceneCharacter;
			while (!cursor.atEnd()) {
				if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::Character) {
					const QString character = CharacterParser::name(cursor.block().text());
					const bool isStartPositionInBlock =
							cursor.block().position() <= _startPosition
							&& cursor.block().position() + cursor.block().length() > _startPosition;
					//
					// Если имя текущего персонажа не пусто и курсор не находится в этом блоке
					//
					if (!character.isEmpty() && !isStartPositionInBlock) {
						//
						// Не второе подряд появление, удаляем из него вспомогательный текст, если есть
						//
						if (lastSceneCharacter.isEmpty()
							|| character != lastSceneCharacter) {
							foreach (const QTextLayout::FormatRange& range, cursor.block().textFormats()) {
								if (range.format.boolProperty(ScenarioBlockStyle::PropertyIsInlineCorrection)) {
									cursor.setPosition(cursor.block().position() + range.start);
									cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, range.length);
									cursor.removeSelectedText();
									break;
								}
							}
						}
						//
						// Если второе подряд, добавляем вспомогательный текст
						//
						else if (character == lastSceneCharacter){
							QString characterState = CharacterParser::state(cursor.block().text());
							if (characterState.isEmpty()) {
								//
								// ... вставляем текст
								//
								cursor.movePosition(QTextCursor::EndOfBlock);
								static const QString textForInsert =
										QString(" (%1)").arg(QApplication::translate("BusinessLogic::ScenarioTextCorrector", DIALOG_CONTINUED));
								cursor.insertText(textForInsert);
								//
								// ... настраиваем формат текста автодополнения
								//
								cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, textForInsert.length());
								QTextCharFormat format;
								format.setProperty(ScenarioBlockStyle::PropertyIsInlineCorrection, true);
								cursor.mergeCharFormat(format);
							}
						}

						lastSceneCharacter = character;
					}
				}
				//
				// Очищаем имя последнего, если текущая сцена закончилась
				//
				else if (::cursorAtSceneBorder(cursor)) {
					lastSceneCharacter.clear();
				}

				cursor.movePosition(QTextCursor::NextBlock);
				cursor.movePosition(QTextCursor::EndOfBlock);
			}

			cursor.endEditBlock();

			//
			// Стараемся не блокировать ввод пользователя
			//
			QApplication::processEvents();
		}


		//
		// Обрабатываем блоки находящиеся в конце страницы
		//
		if (_document->pageSize().isValid()) {
			//
			// Удаляем блоки с дополнительными декорациями
			//
			{
				QTextCursor cursor = mainCursor;
				cursor.beginEditBlock();

				while (!cursor.atEnd()) {
					//
					// Если текущий блок декорация, то убираем его
					//
					const QTextBlockFormat blockFormat = cursor.block().blockFormat();
					if (blockFormat.boolProperty(ScenarioBlockStyle::PropertyIsCorrection)) {
						//
						// ... запоминаем формат предыдущего блока
						//
						cursor.movePosition(QTextCursor::PreviousBlock);
						QTextBlockFormat previousBlockFormat = cursor.blockFormat();
						//
						// ... удаляем блок декорации
						//
						cursor.movePosition(QTextCursor::EndOfBlock);
						cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
						cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
						cursor.deletePreviousChar();
						//
						// ... восстанавливаем формат блока
						//
						cursor.setBlockFormat(previousBlockFormat);
					}
					//
					// Если в текущем блоке начинается разрыв, пробуем его вернуть
					//
					else if (blockFormat.boolProperty(ScenarioBlockStyle::PropertyIsBreakCorrectionStart)) {
						cursor.movePosition(QTextCursor::EndOfBlock);
						do {
							cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
						} while (cursor.blockFormat().boolProperty(ScenarioBlockStyle::PropertyIsCorrection));
						//
						// ... если дошли до конца разрыва, то сшиваем его
						//
						if (cursor.blockFormat().boolProperty(ScenarioBlockStyle::PropertyIsBreakCorrectionEnd)) {
							cursor.insertText(" ");
						}
						//
						// ... а если после начала разрыва идёт другой блок, то просто убираем декорации
						//
						else {
							cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
							cursor.deleteChar();
						}
						//
						// ... очищаем значения обрывов
						//
						QTextBlockFormat cleanFormat = blockFormat;
						cleanFormat.setProperty(ScenarioBlockStyle::PropertyIsBreakCorrectionStart, QVariant());
						cleanFormat.setProperty(ScenarioBlockStyle::PropertyIsBreakCorrectionStart, QVariant());
						cursor.setBlockFormat(cleanFormat);
					}
					//
					// Если текущий блок не декорация, просто переходим к следующему
					//
					else {
						cursor.movePosition(QTextCursor::EndOfBlock);
						cursor.movePosition(QTextCursor::NextCharacter);
					}
				}

				cursor.endEditBlock();

				//
				// Стараемся не блокировать ввод пользователя
				//
				QApplication::processEvents();
			}

			//
			// Корректируем обрывы строк
			//
			{
				QTextCursor cursor = mainCursor;

				QTextBlock currentBlock = cursor.block();
				while (currentBlock.isValid()) {
					//
					// Определим следующий видимый блок
					//
					QTextBlock nextBlock = currentBlock.next();
					while (nextBlock.isValid() && !nextBlock.isVisible()) {
						nextBlock = nextBlock.next();
					}

					if (nextBlock.isValid()) {
						//
						// Проверяем не находится ли текущий блок в конце страницы
						//
						BlockInfo currentBlockInfo = ::blockInfo(currentBlock);
						BlockInfo nextBlockInfo = ::blockInfo(nextBlock);

						//
						// Нашли конец страницы, обрабатываем его соответствующим для типа блока образом
						//
						if (currentBlockInfo.topPage != nextBlockInfo.topPage) {

							//
							// Время и место
							// Имя персонажа
							//
							// переносим на следующую страницу
							// - если в конце предыдущей страницы
							//
							if (ScenarioBlockStyle::forBlock(currentBlock) == ScenarioBlockStyle::SceneHeading
								|| ScenarioBlockStyle::forBlock(currentBlock) == ScenarioBlockStyle::Character) {
								cursor.beginEditBlock();

								int emptyBlocksCount = ::neededEmptyBlocks(currentBlock.blockFormat(), currentBlockInfo.topLinesCount);
								while (emptyBlocksCount-- > 0) {
									cursor.setPosition(currentBlock.position());
									cursor.insertBlock();
									cursor.movePosition(QTextCursor::PreviousBlock);
									QTextBlockFormat format = currentBlock.blockFormat();
									format.setProperty(ScenarioBlockStyle::PropertyType, ScenarioBlockStyle::SceneHeadingShadow);
									format.setProperty(ScenarioBlockStyle::PropertyIsCorrection, true);
									cursor.setBlockFormat(format);
								}

								cursor.endEditBlock();
							}

							//
							// Участники сцены
							//
							// переносим на следующую страницу
							// - если в конце предыдущей страницы
							// - если перед участниками стоит время и место, переносим и его тоже
							//



							//
							// Описание действия
							// - если находится на обеих страницах
							// -- если на странице можно оставить текст, который займёт 2 и более строк,
							//    оставляем максимум, а остальное переносим. Разрываем по предложениям
							// -- в остальном случае переносим полностью
							// --- если перед описанием действия идёт время и место, переносим и его тоже
							// --- если перед описанием действия идёт список участников, то переносим их
							//	   вместе с предыдущим блоком время и место
							//
							else if (ScenarioBlockStyle::forBlock(currentBlock) == ScenarioBlockStyle::Action
									 && currentBlockInfo.topPage != currentBlockInfo.bottomPage) {
								cursor.beginEditBlock();

								//
								// Пробуем разорвать так, чтобы часть текста осталась на предыдущей странице
								//
								const int availableLinesOnPageEnd = 2;
								bool breakSuccess = false;
								if (currentBlockInfo.topLinesCount >= availableLinesOnPageEnd) {
									QStringList prevPageSentences = currentBlock.text().split(". ", QString::SkipEmptyParts);
									QStringList nextPageSentences;
									while (!prevPageSentences.isEmpty()) {
										nextPageSentences << prevPageSentences.takeLast();
										const QString newText = prevPageSentences.join(". ") + ".";
										int linesCount = ::linesCount(newText, currentBlock.charFormat().font(), currentBlockInfo.width);
										if (linesCount <= currentBlockInfo.topLinesCount
											&& linesCount >= availableLinesOnPageEnd) {
											cursor.setPosition(currentBlock.position());
											const QTextBlockFormat format = currentBlock.blockFormat();
											//
											// ... корректируем текст в оставшемся блоке
											//
											{
												cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
												cursor.insertText(newText);
												QTextBlockFormat breakFormat = format;
												breakFormat.setProperty(ScenarioBlockStyle::PropertyIsBreakCorrectionStart, true);
												cursor.setBlockFormat(breakFormat);
											}
											//
											// ... добавляем новый блок с оторванным текстом
											//
											{
												cursor.insertBlock();
												cursor.insertText(nextPageSentences.join(". "));
												QTextBlockFormat breakFormat = format;
												breakFormat.setProperty(ScenarioBlockStyle::PropertyIsBreakCorrectionEnd, true);
												cursor.setBlockFormat(breakFormat);
											}
											breakSuccess = true;
											break;
										}
									}
								}
								//
								// Если блок не удалось разорвать переносим его на следующую страницу
								//
								if (breakSuccess == false) {
									int startPosition = currentBlock.position();

									//
									// Если перед описанием действия идёт время и место и его тоже переносим
									//
									QTextBlock previousBlock = currentBlock.previous();
									while (previousBlock.isValid() && !previousBlock.isVisible()) {
										previousBlock = previousBlock.previous();
									}
									if (previousBlock.isValid()) {
										BlockInfo previousBlockInfo = ::blockInfo(previousBlock);
										if (ScenarioBlockStyle::forBlock(previousBlock) == ScenarioBlockStyle::SceneHeading) {
											startPosition = previousBlock.position();
											int emptyBlocksCount = ::neededEmptyBlocks(previousBlock.blockFormat(), previousBlockInfo.topLinesCount);
											while (emptyBlocksCount-- > 0) {
												cursor.setPosition(startPosition);
												cursor.insertBlock();
												cursor.movePosition(QTextCursor::PreviousBlock);
												QTextBlockFormat format = previousBlock.blockFormat();
												format.setProperty(ScenarioBlockStyle::PropertyType, ScenarioBlockStyle::SceneHeadingShadow);
												format.setProperty(ScenarioBlockStyle::PropertyIsCorrection, true);
												cursor.setBlockFormat(format);
											}
										}
									}

									//
									// Делаем пропуски необходимые для переноса самого описания действия
									//
									int emptyBlocksCount = ::neededEmptyBlocks(currentBlock.blockFormat(), currentBlockInfo.topLinesCount);
									while (emptyBlocksCount-- > 0) {
										cursor.setPosition(startPosition);
										cursor.insertBlock();
										cursor.movePosition(QTextCursor::PreviousBlock);
										QTextBlockFormat format = currentBlock.blockFormat();
										format.setProperty(ScenarioBlockStyle::PropertyIsCorrection, true);
										cursor.setBlockFormat(format);
									}
								}

								cursor.endEditBlock();
							}

							//
							// Ремарка
							// - если перед ремаркой идёт имя персонажа, переносим их вместе на след. страницу
							// - если перед ремаркой идёт реплика, вместо ремарки пишем ДАЛЬШЕ, а на следующую
							//	 страницу добавляем сперва имя персонажа с (ПРОД), а затем саму ремарку
							//

							//
							// Диалог
							// - если можно, то оставляем текст так, чтобы он занимал не менее 2 строк,
							//	 добавляем ДАЛЬШЕ и на следующей странице имя персонажа с (ПРОД) и сам диалог
							// - в противном случае
							// -- если перед диалогом идёт имя персонажа, то переносим их вместе на след.
							// -- если перед диалогом идёт ремарка
							// --- если перед ремаркой идёт имя персонажа, то переносим их всех вместе
							// --- если перед ремаркой идёт диалог, то разрываем по ремарке, пишем вместо неё
							//	   ДАЛЬШЕ, а на следующей странице имя персонажа с (ПРОД), ремарку и сам диалог
							//

							//
							// Стараемся не блокировать ввод пользователя
							//
							QApplication::processEvents();
						}
					}

					currentBlock = nextBlock;
				}
			}
		}


		//
		// Обновляем текст текущего документа
		//
		s_documentTexts[_document] = _document->toHtml();

		s_proccessedNow = false;
	}
}

