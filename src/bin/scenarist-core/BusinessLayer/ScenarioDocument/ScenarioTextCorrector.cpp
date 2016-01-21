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
	static const char* CONTINUED = QT_TRANSLATE_NOOP("BusinessLogic::ScenarioTextCorrector", "CONT'D");
	static const QString continuedTerm() {
		return QString(" (%1)").arg(QApplication::translate("BusinessLogic::ScenarioTextCorrector", CONTINUED));
	}

	/**
	 * @brief Автоматически добавляемые продолжения в диалогах
	 */
	static const char* MORE = QT_TRANSLATE_NOOP("BusinessLogic::ScenarioTextCorrector", "MORE");
	static const QString moreTerm() {
		return QApplication::translate("BusinessLogic::ScenarioTextCorrector", MORE);
	}

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
				//
				// Обрабатываем ситуацию со сдвоенными знаками, наподобии "!?"
				//
				if (charIndex < _text.length()
					&& punctuation.contains(_text.at(charIndex))) {
					++charIndex;
				}
				result.append(_text.mid(lastSentenceEnd, charIndex - lastSentenceEnd).simplified());
				lastSentenceEnd = charIndex;
			}
		}

		return result;
	}

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
	 * @brief Удалить заданные блок
	 */
	static void removeTextBlock(QTextDocument* _document, const QTextBlock& _block) {
		QTextCursor cursor(_document);
		cursor.setPosition(_block.position());
		cursor.select(QTextCursor::BlockUnderCursor);
		cursor.deleteChar();
	}

	/**
	 * @brief BlockPages
	 */
	class BlockInfo
	{
	public:
		BlockInfo() : onPageTop(false), topPage(0), bottomPage(0), topLinesCount(0), bottomLinesCount(0), width(0) {}
		BlockInfo(bool _onPageTop, int _top, int _bottom, int _topLines, int _bottomLines, int _width)
			: onPageTop(_onPageTop), topPage(_top), bottomPage(_bottom), topLinesCount(_topLines),
			  bottomLinesCount(_bottomLines), width(_width)
		{}

		/**
		 * @brief Блок находится в начале страницы
		 */
		bool onPageTop;

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
		bool onPageTop = false;
		if (pageHeight - positionOnTopPage - pageBottomMargin < lineHeight) {
			onPageTop = true;
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
		int topLinesCount = blockRect.height() / lineHeight;
		int bottomLinesCount = 0;
		if (topPage != bottomPage) {
			topLinesCount = (pageHeight - positionOnTopPage - pageBottomMargin) / lineHeight;
			const qreal pagesInterval =
				pageBottomMargin + _block.document()->rootFrame()->frameFormat().topMargin();
			bottomLinesCount = (blockRect.height() - pagesInterval) / lineHeight - topLinesCount;
		}

		//
		// NOTE: Номера страниц ночинаются с нуля
		//
		return BlockInfo(onPageTop, topPage, bottomPage, topLinesCount, bottomLinesCount, blockRect.width());
	}

	static bool checkCorrectionBlock(QTextDocument* _document, const QTextBlock& _block)
	{
		bool result = false;

		QTextBlock currentBlock = _block;
		QTextBlock nextBlock = _block.next();

		while (nextBlock.isValid()) {
			BlockInfo currentBlockInfo = ::blockInfo(currentBlock);
			BlockInfo nextBlockInfo = ::blockInfo(nextBlock);

			//
			// Если декорация в конце страницы
			//
			if (currentBlockInfo.topPage != nextBlockInfo.topPage) {
				//
				// Если следующий блок декорация
				//
				if (nextBlock.blockFormat().boolProperty(ScenarioBlockStyle::PropertyIsCorrection)
					&& !nextBlock.blockFormat().boolProperty(ScenarioBlockStyle::PropertyIsCorrectionCharacter)) {
					//
					// ... то удалим его и проверим новый следующий
					//
					::removeTextBlock(_document, nextBlock);
					nextBlock = currentBlock.next();
				}
				//
				// Если следующий блок не декорация
				//
				else {
					//
					// Дальше можно не проверять, декорация находится по месту
					//
					result = true;
					break;
				}
			}
			//
			// Если декорация не в конце страницы
			//
			else {
				//
				// Если следующий блок декорация
				//
				if (nextBlock.blockFormat().boolProperty(ScenarioBlockStyle::PropertyIsCorrection)) {
					//
					// И если следующий блок на месте, то значит и текущий блок на месте
					//
					if (checkCorrectionBlock(_document, nextBlock)) {
						result = true;
						break;
					}
					//
					// А если следующий блок не на месте
					//
					else {
						//
						// ... то удалим его и проверим новый следующий
						//
						::removeTextBlock(_document, nextBlock);
						nextBlock = currentBlock.next();
					}
				}
				//
				// Если следующий блок не декорация
				//
				else {
					//
					// Дальше можно не проверять, декорация не по месту
					//
					result = false;
					break;
				}
			}
		}

		return result;
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

	/**
	 * @brief Сместить блок вниз при помощи блоков декораций
	 */
	static void moveBlockDown(QTextBlock& _block, QTextCursor& _cursor, int _position) {
		BlockInfo blockInfo = ::blockInfo(_block);
		//
		// Смещаем блок, если он не в начале страницы
		//
		if (!blockInfo.onPageTop) {
			int emptyBlocksCount = ::neededEmptyBlocks(_block.blockFormat(), blockInfo.topLinesCount);
			while (emptyBlocksCount-- > 0) {
				_cursor.setPosition(_position);
				_cursor.insertBlock();
				_cursor.movePosition(QTextCursor::PreviousBlock);
				QTextBlockFormat format = _block.blockFormat();
				if (ScenarioBlockStyle::forBlock(_block) == ScenarioBlockStyle::SceneHeading) {
					format.setProperty(ScenarioBlockStyle::PropertyType, ScenarioBlockStyle::SceneHeadingShadow);
				}
				format.setProperty(ScenarioBlockStyle::PropertyIsCorrection, true);
				_cursor.setBlockFormat(format);
			}
		}
	}

	/**
	 * @brief Сместить блок вниз при помощи блоков декораций внутри диалога
	 * @note Этот метод используется только для блоков ремарки и диалога
	 */
	static void moveBlockDownInDialogue(QTextBlock& _block, QTextCursor& _cursor, int _position) {
		if (ScenarioBlockStyle::forBlock(_block) == ScenarioBlockStyle::Parenthetical
			|| ScenarioBlockStyle::forBlock(_block) == ScenarioBlockStyle::Dialogue) {

			BlockInfo blockInfo = ::blockInfo(_block);
			int emptyBlocksCount = ::neededEmptyBlocks(_block.blockFormat(), blockInfo.topLinesCount);
			bool isFirstDecoration = true;
			while (emptyBlocksCount-- > 0) {
				_cursor.setPosition(_position);
				_cursor.insertBlock();
				_cursor.movePosition(QTextCursor::PreviousBlock);
				QTextBlockFormat format = _block.blockFormat();

				//
				// Первый вставляемый блок оформляем как ремарку и добавляем туда текст ДАЛЬШЕ
				//
				if (isFirstDecoration) {
					isFirstDecoration = false;

					ScenarioBlockStyle parentheticalStyle =
						ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::Parenthetical);
					format = parentheticalStyle.blockFormat();

					_cursor.insertText(moreTerm());

					//
					// Обновляем позицию курсора, чтобы остальной текст добавлялся ниже блока с текстом ДАЛЬШЕ
					//
					_position += moreTerm().length() + 1;
				} else {
					_position += 1;
				}

				format.setProperty(ScenarioBlockStyle::PropertyIsCorrection, true);
				_cursor.setBlockFormat(format);
			}

			//
			// Когда перешли на новую страницу - добавляем блок с именем персонажа и (ПРОД)
			// и делаем его декорацией
			//
			QTextBlock characterBlock = _block.previous();
			while (characterBlock.isValid()
				   && ScenarioBlockStyle::forBlock(characterBlock) != ScenarioBlockStyle::Character) {
				characterBlock = characterBlock.previous();
			}
			if (characterBlock.isValid()) {
				QString characterName = CharacterParser::name(characterBlock.text());

				_cursor.setPosition(_position);
				_cursor.insertBlock();
				_cursor.movePosition(QTextCursor::PreviousBlock);

				ScenarioBlockStyle characterStyle =
						ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::Character);
				QTextBlockFormat format = characterStyle.blockFormat();
				format.setProperty(ScenarioBlockStyle::PropertyIsCorrection, true);
				format.setProperty(ScenarioBlockStyle::PropertyIsCorrectionCharacter, true);
				_cursor.setBlockFormat(format);

				_cursor.insertText(characterName + continuedTerm());
				_cursor.movePosition(QTextCursor::EndOfBlock);
			}
		}
	}
}


void ScenarioTextCorrector::removeDecorations(const QTextCursor& _cursor, int _startPosition, int _endPosition)
{
	QTextCursor cursor = _cursor;
	cursor.setPosition(_startPosition);
	if (_endPosition == 0) {
		_endPosition = _cursor.document()->characterCount();
	}

	cursor.beginEditBlock();
	while (!cursor.atEnd()
		   && cursor.position() <= _endPosition) {
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
				if (cursor.selectionStart() != cursor.selectionEnd()) {
					cursor.deleteChar();
				}
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
							QString blockText = cursor.block().text();
							if (blockText.endsWith(continuedTerm(), Qt::CaseInsensitive)) {
								cursor.setPosition(cursor.block().position() + blockText.indexOf(continuedTerm(), 0, Qt::CaseInsensitive));
								cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
								cursor.removeSelectedText();
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
								cursor.insertText(continuedTerm());
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
		}


		//
		// Если используется постраничный режим, то обрабатываем блоки находящиеся в конце страницы
		//
		if (_document->pageSize().isValid()) {
			QTextCursor cursor = mainCursor;

			//
			// Удаляем декорации перед редактируемым элементом
			//
			// Делать это нужно для того, чтобы корректно реагировать на ситуации, когда текст
			// был перенесён на следующую страницу из-за того что не влез на предыдущую, а потом
			// пользователь изменил его, оставив там меньше строк и теперь текст влезет
			//
			{
				//
				// ... это ситуация, когда удаляем декорации от начала сцены
				//
				int removeDecorationsFrom = cursor.position();
				//
				// ... или, если это не первая сцена за один блок до начала сцены
				//
				if (!cursor.atStart()) {
					removeDecorationsFrom = cursor.block().previous().position();
				}
				removeDecorations(cursor, removeDecorationsFrom, _startPosition);
			}

			//
			// Корректируем обрывы строк
			//
			// Вносим корректировки пока не достигнем первого корректирующего блока,
			// если он стоит по месту, то прерываешь бег, в противном случае удаляем его
			// и продолжаем выполнять корректировки до следующего корректирующего блока, который
			// находится в правильном месте, или пока не достигнем конца документа
			//

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
					// Если текущий блок декорация, проверяем, в правильном ли месте он расположен
					//
					{
						const QTextBlockFormat blockFormat = currentBlock.blockFormat();

						//
						// Декорации могут находится только в конце страницы
						//
						if (blockFormat.boolProperty(ScenarioBlockStyle::PropertyIsCorrection)) {
							//
							// Если декорация по месту, прерываем корректировки
							//
							if (::checkCorrectionBlock(_document, currentBlock)) {
								break;
							}
							//
							// А если не по месту, удаляем её и переходим к проверке следующего блока
							//
							else {
								QTextBlock previuosBlock = currentBlock.previous();

								::removeTextBlock(_document, currentBlock);

								currentBlock = previuosBlock.next();
								continue;
							}
						}
						//
						// Если в текущем блоке начинается разрыв, пробуем его вернуть
						//
						else if (blockFormat.boolProperty(ScenarioBlockStyle::PropertyIsBreakCorrectionStart)) {
							//
							// Если разрыв по месту, прерываем корректировки
							//
							if (::checkCorrectionBlock(_document, currentBlock)) {
								break;
							}
							//
							// А если не по месту, сшиваем блок и перепроверяем его
							//
							else {
								QTextBlock previuosBlock = currentBlock.previous();

								cursor.setPosition(currentBlock.position());
								cursor.movePosition(QTextCursor::EndOfBlock);
								cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
								cursor.insertText(" ");
								QTextBlockFormat format = cursor.blockFormat();
								format.setProperty(ScenarioBlockStyle::PropertyIsBreakCorrectionStart, QVariant());
								format.setProperty(ScenarioBlockStyle::PropertyIsBreakCorrectionEnd, QVariant());
								cursor.setBlockFormat(format);

								currentBlock = previuosBlock.next();
								continue;
							}
						}
					}


					//
					// Собственно корректировки
					//
					{
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
								::moveBlockDown(currentBlock, cursor, currentBlock.position());
								cursor.endEditBlock();
							}


							//
							// Участники сцены
							//
							// переносим на следующую страницу
							// - если в конце предыдущей страницы
							// - если перед участниками стоит время и место, переносим и его тоже
							//
							if (ScenarioBlockStyle::forBlock(currentBlock) == ScenarioBlockStyle::SceneCharacters) {
								cursor.beginEditBlock();

								int startPosition = currentBlock.position();

								//
								// Проверяем предыдущий блок
								//
								{
									QTextBlock previousBlock = currentBlock.previous();
									while (previousBlock.isValid() && !previousBlock.isVisible()) {
										previousBlock = previousBlock.previous();
									}
									if (previousBlock.isValid()) {
										//
										// Если перед участниками сцены идёт время и место и его тоже переносим
										//
										if (ScenarioBlockStyle::forBlock(previousBlock) == ScenarioBlockStyle::SceneHeading) {
											startPosition = previousBlock.position();
											::moveBlockDown(previousBlock, cursor, startPosition);
										}
									}
								}

								//
								// Делаем пропуски необходимые для переноса самих участников сцены
								//
								::moveBlockDown(currentBlock, cursor, startPosition);

								cursor.endEditBlock();
							}


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
									QStringList prevPageSentences = ::splitTextToSentences(currentBlock.text());
									QStringList nextPageSentences;
									while (!prevPageSentences.isEmpty()) {
										nextPageSentences << prevPageSentences.takeLast();
										const QString newText = prevPageSentences.join(" ");
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
												cursor.insertText(nextPageSentences.join(" "));
												QTextBlockFormat breakFormat = format;
												breakFormat.setProperty(ScenarioBlockStyle::PropertyIsBreakCorrectionEnd, true);
												cursor.setBlockFormat(breakFormat);

												cursor.endEditBlock();
												cursor.beginEditBlock();

												QTextBlock blockForMove = cursor.block();
												::moveBlockDown(blockForMove, cursor, cursor.block().position());
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
									// Проверяем предыдущий блок
									//
									QTextBlock previousBlock = currentBlock.previous();
									while (previousBlock.isValid() && !previousBlock.isVisible()) {
										previousBlock = previousBlock.previous();
									}
									if (previousBlock.isValid()) {
										//
										// Если перед описанием действия идёт время и место и его тоже переносим
										//
										if (ScenarioBlockStyle::forBlock(previousBlock) == ScenarioBlockStyle::SceneHeading) {
											startPosition = previousBlock.position();
											::moveBlockDown(previousBlock, cursor, startPosition);
										}
										//
										// Если перед описанием действия идут участники сцены, то их тоже переносим
										//
										else if (ScenarioBlockStyle::forBlock(previousBlock) == ScenarioBlockStyle::SceneCharacters) {
											startPosition = previousBlock.position();

											//
											// Проверяем предыдущий блок
											//
											QTextBlock prePreviousBlock = previousBlock.previous();
											while (prePreviousBlock.isValid() && !prePreviousBlock.isVisible()) {
												prePreviousBlock = prePreviousBlock.previous();
											}
											if (prePreviousBlock.isValid()) {
												//
												// Если перед участниками сцены идёт время и место его тоже переносим
												//
												if (ScenarioBlockStyle::forBlock(prePreviousBlock) == ScenarioBlockStyle::SceneHeading) {
													startPosition = prePreviousBlock.position();
													::moveBlockDown(prePreviousBlock, cursor, startPosition);
												}
											}

											//
											// Делаем пропуски необходимые для переноса самих участников сцены
											//
											::moveBlockDown(previousBlock, cursor, startPosition);
										}
									}

									//
									// Делаем пропуски необходимые для переноса самого описания действия
									//
									::moveBlockDown(currentBlock, cursor, startPosition);
								}

								cursor.endEditBlock();
							}


							//
							// Ремарка
							// - если перед ремаркой идёт имя персонажа, переносим их вместе на след. страницу
							// - если перед ремаркой идёт реплика, вместо ремарки пишем ДАЛЬШЕ, а на следующую
							//	 страницу добавляем сперва имя персонажа с (ПРОД), а затем саму ремарку
							//
							else if (ScenarioBlockStyle::forBlock(currentBlock) == ScenarioBlockStyle::Parenthetical) {
								cursor.beginEditBlock();

								int startPosition = currentBlock.position();

								//
								// Проверяем предыдущий блок
								//
								{
									QTextBlock previousBlock = currentBlock.previous();
									while (previousBlock.isValid() && !previousBlock.isVisible()) {
										previousBlock = previousBlock.previous();
									}
									if (previousBlock.isValid()) {
										//
										// Если перед ремаркой идёт имя персонажа и его тоже переносим
										//
										if (ScenarioBlockStyle::forBlock(previousBlock) == ScenarioBlockStyle::Character) {
											startPosition = previousBlock.position();
											::moveBlockDown(previousBlock, cursor, startPosition);

											//
											// Делаем пропуски необходимые для переноса самой ремарки
											//
											::moveBlockDown(currentBlock, cursor, startPosition);
										}
										//
										// Если перед ремаркой идёт диалог, то переносим по правилам
										//
										else if (ScenarioBlockStyle::forBlock(previousBlock) == ScenarioBlockStyle::Dialogue) {
											::moveBlockDownInDialogue(currentBlock, cursor, startPosition);
										}
									}
								}

								cursor.endEditBlock();
							}


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
							else if (ScenarioBlockStyle::forBlock(currentBlock) == ScenarioBlockStyle::Dialogue
									 && currentBlockInfo.topPage != currentBlockInfo.bottomPage) {
								cursor.beginEditBlock();

								//
								// Пробуем разорвать так, чтобы часть текста осталась на предыдущей странице.
								//
								const int availableLinesOnPageEnd = 2;
								bool breakSuccess = false;
								//
								// Оставлять будем 2 строки, но проверять нужно на 3, т.к. нам ещё
								// нужно поместить вспомогательную надпись ДАЛЬШЕ
								//
								if (currentBlockInfo.topLinesCount >= availableLinesOnPageEnd + 1) {
									QStringList prevPageSentences = ::splitTextToSentences(currentBlock.text());
									QStringList nextPageSentences;
									while (!prevPageSentences.isEmpty()) {
										nextPageSentences << prevPageSentences.takeLast();
										const QString newText = prevPageSentences.join(" ");
										int linesCount = ::linesCount(newText, currentBlock.charFormat().font(), currentBlockInfo.width);
										//
										// ... опять резервируем одну строку под надпись ДАЛЬШЕ
										//
										if (linesCount <= currentBlockInfo.topLinesCount - 1
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
												cursor.insertText(nextPageSentences.join(" "));
												QTextBlockFormat breakFormat = format;
												breakFormat.setProperty(ScenarioBlockStyle::PropertyIsBreakCorrectionEnd, true);
												cursor.setBlockFormat(breakFormat);

												cursor.endEditBlock();
												cursor.beginEditBlock();

												QTextBlock blockForMove = cursor.block();
												::moveBlockDownInDialogue(blockForMove, cursor, cursor.block().position());
											}
											breakSuccess = true;
											break;
										}
									}
								}

								//
								// Если блок не удалось разорвать смотрим, что мы можем с ним сделать
								//
								if (breakSuccess == false) {
									int startPosition = currentBlock.position();

									//
									// Проверяем предыдущий блок
									//
									QTextBlock previousBlock = currentBlock.previous();
									while (previousBlock.isValid() && !previousBlock.isVisible()) {
										previousBlock = previousBlock.previous();
									}
									if (previousBlock.isValid()) {
										//
										// Если перед диалогом идёт персонаж и его тоже переносим
										//
										if (ScenarioBlockStyle::forBlock(previousBlock) == ScenarioBlockStyle::Character) {
											startPosition = previousBlock.position();
											::moveBlockDown(previousBlock, cursor, startPosition);
										}
										//
										// Если перед диалогом идёт ремарка
										//
										else if (ScenarioBlockStyle::forBlock(previousBlock) == ScenarioBlockStyle::Parenthetical) {
											startPosition = previousBlock.position();

											//
											// Проверяем предыдущий блок
											//
											QTextBlock prePreviousBlock = previousBlock.previous();
											while (prePreviousBlock.isValid() && !prePreviousBlock.isVisible()) {
												prePreviousBlock = prePreviousBlock.previous();
											}
											if (prePreviousBlock.isValid()) {
												//
												// Если перед ремаркой идёт персонаж, просто переносим их вместе
												//
												if (ScenarioBlockStyle::forBlock(prePreviousBlock) == ScenarioBlockStyle::Character) {
													startPosition = prePreviousBlock.position();
													::moveBlockDown(prePreviousBlock, cursor, startPosition);
													::moveBlockDown(previousBlock, cursor, startPosition);
												}
												//
												// В противном случае, заменяем ремарку на ДАЛЬШЕ и переносим её
												//
												else {
													::moveBlockDownInDialogue(previousBlock, cursor, startPosition);
												}
											}
										}
									}

									//
									// Делаем пропуски необходимые для переноса самого диалога
									//
									::moveBlockDown(currentBlock, cursor, startPosition);
								}

								cursor.endEditBlock();
							}
						}
					}
				}

				currentBlock = nextBlock;
			}
		}
		//
		// В противном случае просто удаляем блоки с декорациями
		//
		else {
			removeDecorations(mainCursor);
		}


		//
		// Обновляем текст текущего документа
		//
		s_documentTexts[_document] = _document->toHtml();

		s_proccessedNow = false;
	}
}

