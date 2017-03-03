#include "DocumentImporter.h"

#include <format_manager.h>
#include <format_reader.h>
#include <format_helpers.h>

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockParsers.h>

#include <3rd_party/Widgets/PagesTextEdit/PageMetrics.h>

#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QXmlStreamWriter>

using namespace BusinessLogic;

namespace {
	/**
	 * @brief Регулярное выражение для определения блока "Время и место" по наличию слов места
	 */
    const QRegularExpression PLACE_CONTAINS_CHECKER("(^|[^\\S])(INT|EXT|ИНТ|НАТ|ПАВ|ЭКСТ)([.]|[ - ])");

	/**
	 * @brief Регулярное выражение для определения блока "Время и место" по началу с номера
	 */
	const QRegularExpression START_FROM_NUMBER_CHECKER("^([\\d]{1,}[\\d\\S]{0,})([.]|[-])(([\\d\\S]{1,})([.]|)|) ");

	/**
	 * @brief Допущение для блоков, которые по идее вообще не должны иметь отступа в пикселях (16 мм)
	 */
	const int LEFT_MARGIN_DELTA = 60;

	/**
	 * @brief Некоторые программы выравнивают текст при помощи пробелов
	 */
	const QString OLD_SCHOOL_CENTERING_PREFIX = "                    ";

	/**
	 * @brief Ключи для формирования xml из импортируемого документа
	 */
	/** @{ */
	const QString NODE_SCENARIO = "scenario";
	const QString NODE_VALUE = "v";
	const QString NODE_REVIEW_GROUP = "reviews";
	const QString NODE_REVIEW = "review";
	const QString NODE_REVIEW_COMMENT = "review_comment";

	const QString ATTRIBUTE_VERSION = "version";
	const QString ATTRIBUTE_REVIEW_FROM = "from";
	const QString ATTRIBUTE_REVIEW_LENGTH = "length";
	const QString ATTRIBUTE_REVIEW_COLOR = "color";
	const QString ATTRIBUTE_REVIEW_BGCOLOR = "bgcolor";
	const QString ATTRIBUTE_REVIEW_IS_HIGHLIGHT = "is_highlight";
	const QString ATTRIBUTE_REVIEW_COMMENT = "comment";
	const QString ATTRIBUTE_REVIEW_AUTHOR = "author";
	const QString ATTRIBUTE_REVIEW_DATE = "date";
	/** @} */

	/**
	 * @brief Определить тип блока в текущей позиции курсора
	 *		  с указанием предыдущего типа и количества предшествующих пустых строк
	 */
	static ScenarioBlockStyle::Type typeForTextCursor(const QTextCursor& _cursor,
		ScenarioBlockStyle::Type _lastBlockType, int _prevEmptyLines, int _minLeftMargin,
		bool _outline) {
		//
		// Определим текст блока
		//
		const QString blockText = _cursor.block().text();
		const QString blockTextUppercase = blockText.toUpper();

		//
		// Для всех нераспознаных блоков ставим тип "Описание действия"
		//
		ScenarioBlockStyle::Type blockType = ScenarioBlockStyle::Action;

		//
		// Определим некоторые характеристики исследуемого текста
		//
		// ... стили блока
		const QTextBlockFormat blockFormat = _cursor.blockFormat();
		const QTextCharFormat charFormat = _cursor.charFormat();
		// ... текст в верхнем регистре (FIXME: такие строки, как "Я.")
		bool textIsUppercase =
				charFormat.fontCapitalization() == QFont::AllUppercase
				|| blockText == blockText.toUpper();
		// ... блоки находящиеся в центре
		bool isCentered =
				(blockFormat.leftMargin() > LEFT_MARGIN_DELTA + _minLeftMargin)
				|| (blockFormat.alignment() == Qt::AlignCenter)
				|| blockText.startsWith(OLD_SCHOOL_CENTERING_PREFIX);

		//
		// Собственно определение типа
		//
		{
			//
			// Блоки текста посередине
			//
			if (isCentered) {
				//
				// Персонаж
				// 1. В верхнем регистре
				//
				if (textIsUppercase && _lastBlockType != ScenarioBlockStyle::Character) {
					blockType = ScenarioBlockStyle::Character;
				}
				//
				// Ремарка
				// 1. начинается со скобки
				//
				else if (blockText.startsWith("(")) {
					blockType = ScenarioBlockStyle::Parenthetical;
				}
				//
				// Реплика
				// 1. всё что осталось
				//
				else {
					blockType = ScenarioBlockStyle::Dialogue;
				}

			}
			//
			// Не посередине
			//
			else {
				//
				// Блоки текста в верхнем регистре
				//
				if (textIsUppercase) {
					//
					// Время и место
					// 1. текст в верхнем регистре
					// 2. содержит ключевые сокращения места действия или начинается с номера сцены
					//
					if (blockTextUppercase.contains(PLACE_CONTAINS_CHECKER)
						|| blockTextUppercase.contains(START_FROM_NUMBER_CHECKER)) {
						blockType = ScenarioBlockStyle::SceneHeading;
					}
					//
					// Участника сцены
					// 1. в верхнем регистре
					// 2. идут сразу же после времени и места
					// 3. не имеют сверху отступа
					//
					else if (_lastBlockType == ScenarioBlockStyle::SceneHeading
							 && _prevEmptyLines == 0
							 && blockFormat.topMargin() == 0) {
						blockType = ScenarioBlockStyle::SceneCharacters;
					}
					//
					// Примечание
					// 1. всё что осталось и не имеет отступов
					// 2. выровнено по левому краю
					//
					else if (blockFormat.alignment().testFlag(Qt::AlignLeft)
							 && !isCentered) {
						blockType = ScenarioBlockStyle::Note;
					}
					//
					// Переход
					// 1. всё что осталось и выровнено по правому краю
					//
					else if (blockFormat.alignment().testFlag(Qt::AlignRight)) {
						blockType = ScenarioBlockStyle::Transition;
					}
				}
			}
		}

		//
		// В режиме поэпизодного плана все блоки, кроме "Место и время" и "Персонажи сцены"
		// определяются, как "Описание сцены"
		//
		if (_outline) {
			if (blockType != ScenarioBlockStyle::SceneHeading
				&& blockType != ScenarioBlockStyle::SceneCharacters) {
				blockType = ScenarioBlockStyle::SceneDescription;
			}
		}

		return blockType;
	}

	/**
	 * @brief Шум, который может встречаться в тексте
	 */
	const QString NOISE("([.]|[,]|[:]|[ ]|[-]){1,}");

	/**
	 * @brief Регулярное выражение для удаления мусора в начале текста
	 */
	const QRegularExpression NOISE_AT_START("^" + NOISE);

	/**
	 * @brief Регулярное выражение для удаления мусора в конце текста
	 */
	const QRegularExpression NOISE_AT_END(NOISE + "$");

	/**
	 * @brief Очистка блоков от мусора и их корректировки
	 */
	static QString clearBlockText(ScenarioBlockStyle::Type _blockType, const QString& _blockText) {
		QString result = _blockText;

		//
		// Удаляем длинные тире
		//
		result = result.replace("–", "-");

		//
		// Для блока времени и места:
		// * всевозможные "инт - " меняем на "инт. "
		// * убираем точки в конце названия локации
		//
		if (_blockType == ScenarioBlockStyle::SceneHeading) {
			const QString location = BusinessLogic::SceneHeadingParser::locationName(_blockText);
			QString clearLocation = location.simplified();
			clearLocation.remove(NOISE_AT_START);
			clearLocation.remove(NOISE_AT_END);
			if (location != clearLocation) {
				result = result.replace(location, clearLocation);
			}
		}

		//
		// Для персонажей
		// * убираем точки в конце
		//
		else if (_blockType == ScenarioBlockStyle::Character) {
			const QString name = BusinessLogic::CharacterParser::name(_blockText);
			QString clearName = name.simplified();
			clearName.remove(NOISE_AT_END);
			if (name != clearName) {
				result = result.replace(name, clearName);
			}
		}

		return result;
	}
}


DocumentImporter::DocumentImporter() :
	AbstractImporter()
{
}

QString DocumentImporter::importScenario(const ImportParameters& _importParameters) const
{
	//
	// Преобразовать заданный документ в QTextDocument
	//
	QTextDocument documentForImport;
	QFile documentFile(_importParameters.filePath);
	documentFile.open(QIODevice::ReadOnly);
	FormatReader* reader = FormatManager::createReader(&documentFile);
	reader->read(&documentFile, &documentForImport);

	//
	// Найти минимальный отступ слева для всех блоков
	// ЗАЧЕМ: во многих программах (Final Draft, Screeviner) сделано так, что поля
	//		  задаются за счёт оступов. Получается что и заглавие сцены и описание действия
	//		  имеют отступы. Так вот это и будет минимальным отступом, который не будем считать
	//
	int minLeftMargin = 1000;
	{
		QTextCursor cursor(&documentForImport);
		while (!cursor.atEnd()) {
			if (minLeftMargin > cursor.blockFormat().leftMargin()) {
				minLeftMargin = cursor.blockFormat().leftMargin();
			}

			cursor.movePosition(QTextCursor::NextBlock);
			cursor.movePosition(QTextCursor::EndOfBlock);
		}
	}

	//
	// FIXME: много чего дублируется с ScenarioXml
	//

	//
	// Преобразовать его в xml-строку
	//
	QString scenarioXml;
	QTextCursor cursor(&documentForImport);

	QXmlStreamWriter writer(&scenarioXml);
	writer.writeStartDocument();
	writer.writeStartElement(NODE_SCENARIO);
	writer.writeAttribute(ATTRIBUTE_VERSION, "1.0");

	//
	// Для каждого блока текста определяем тип
	//
	// ... последний стиль блока
	ScenarioBlockStyle::Type lastBlockType = ScenarioBlockStyle::Undefined;
	// ... количество пустых строк
	int emptyLines = 0;
	do {
		cursor.movePosition(QTextCursor::EndOfBlock);

		//
		// Если в блоке есть текст
		//
		if (!cursor.block().text().simplified().isEmpty()) {
			//
			// ... определяем тип
			//
			const ScenarioBlockStyle::Type blockType =
				::typeForTextCursor(cursor, lastBlockType, emptyLines, minLeftMargin,
					_importParameters.outline);
			const QString blockTypeName = ScenarioBlockStyle::typeName(blockType);
			QString blockText = cursor.block().text().simplified();

			//
			// ... запишем данные в строку
			//
			writer.writeStartElement(blockTypeName);

			//
			// Если текущий тип "Время и место" и нужно удалить номер сцены, то делаем это
			//
			if (blockType == ScenarioBlockStyle::SceneHeading
				&& _importParameters.removeScenesNumbers){
				blockText = blockText.toUpper();
				QRegularExpressionMatch match = START_FROM_NUMBER_CHECKER.match(blockText);
				if (match.hasMatch()) {
					blockText = blockText.mid(match.capturedEnd());
				}
			}

			//
			// Выполняем корректировки
			//
			blockText = ::clearBlockText(blockType, blockText);

			//
			// Пишем текст
			//
			writer.writeStartElement(NODE_VALUE);
			writer.writeCDATA(blockText);
			writer.writeEndElement();

			//
			// Пишем редакторские комментарии
			//
			if (_importParameters.saveReviewMarks) {
				const QTextBlock currentBlock = cursor.block();
				if (!currentBlock.textFormats().isEmpty()) {
					writer.writeStartElement(NODE_REVIEW_GROUP);
					foreach (const QTextLayout::FormatRange& range, currentBlock.textFormats()) {
						//
						// Всё, кроме стандартного
						//
						if (range.format.boolProperty(Docx::IsForeground)
							|| range.format.boolProperty(Docx::IsBackground)
							|| range.format.boolProperty(Docx::IsHighlight)
							|| range.format.boolProperty(Docx::IsComment)) {
							writer.writeStartElement(NODE_REVIEW);
							writer.writeAttribute(ATTRIBUTE_REVIEW_FROM, QString::number(range.start));
							writer.writeAttribute(ATTRIBUTE_REVIEW_LENGTH, QString::number(range.length));
							if (range.format.hasProperty(QTextFormat::ForegroundBrush)) {
								writer.writeAttribute(ATTRIBUTE_REVIEW_COLOR,
													  range.format.foreground().color().name());
							}
							if (range.format.hasProperty(QTextFormat::BackgroundBrush)) {
								writer.writeAttribute(ATTRIBUTE_REVIEW_BGCOLOR,
													  range.format.background().color().name());
							}
							writer.writeAttribute(ATTRIBUTE_REVIEW_IS_HIGHLIGHT,
												  range.format.boolProperty(Docx::IsHighlight) ? "true" : "false");
							//
							// ... комментарии
							//
							const QStringList comments = range.format.property(Docx::Comments).toStringList();
							const QStringList authors = range.format.property(Docx::CommentsAuthors).toStringList();
							const QStringList dates = range.format.property(Docx::CommentsDates).toStringList();
							for (int commentIndex = 0; commentIndex < comments.size(); ++commentIndex) {
								writer.writeEmptyElement(NODE_REVIEW_COMMENT);
								writer.writeAttribute(ATTRIBUTE_REVIEW_COMMENT, comments.at(commentIndex));
								writer.writeAttribute(ATTRIBUTE_REVIEW_AUTHOR, authors.at(commentIndex));
								writer.writeAttribute(ATTRIBUTE_REVIEW_DATE, dates.at(commentIndex));
							}
							//
							writer.writeEndElement();
						}
					}
					writer.writeEndElement();
				}
			}
			//
			// ... конец абзаца
			//
			writer.writeEndElement();

			//
			// Запомним последний стиль блока и обнулим счётчик пустых строк
			//
			lastBlockType = blockType;
			emptyLines = 0;
		}
		//
		// Если в блоке нет текста, то увеличиваем счётчик пустых строк
		//
		else {
			++emptyLines;
		}

		cursor.movePosition(QTextCursor::NextCharacter);
	} while (!cursor.atEnd());

	writer.writeEndElement(); // scenario
	writer.writeEndDocument();

	return scenarioXml;
}
