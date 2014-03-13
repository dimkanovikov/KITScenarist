#include "RtfExporter.h"

#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QFile>
#include <QChar>

using namespace BusinessLogic;


namespace {
	const char* END_OF_LINE = "\r\n";
	const char* EMPTY_LINE = "\\pard\\plain\\par\r\n";
}

RtfExporter::RtfExporter()
{
}

void RtfExporter::exportTo(QTextDocument* _document, const QString& _toFile) const
{
	//
	// Открываем документ на запись
	//
	QFile rtfFile(_toFile);
	if (rtfFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		rtfFile.write("{");
		rtfFile.write(header().toUtf8().data());
		rtfFile.write(END_OF_LINE);

		//
		// Данные считываются из исходного документа, определяется тип блока
		// и записываются прямо в файл
		//
		QTextCursor documentCursor(_document);

		ScenarioTextBlockStyle::Type currentBlockType = ScenarioTextBlockStyle::Undefined;
		while (!documentCursor.atEnd()) {
			//
			// Получим тип текущего блока под курсором
			//
			currentBlockType = ScenarioTextBlockStyle::forBlock(documentCursor.block());

			//
			// Если блок содержит текст, который необходимо вывести на печать
			//
			if (currentBlockType != ScenarioTextBlockStyle::NoprintableText
				&& currentBlockType != ScenarioTextBlockStyle::FolderHeader
				&& currentBlockType != ScenarioTextBlockStyle::FolderFooter) {
				//
				// Если вставляется не первый блок текста и нужно сделать отступ
				//
				if (!documentCursor.atStart()
					&& !documentCursor.block().text().isEmpty()
					&& currentBlockType != ScenarioTextBlockStyle::Dialog
					&& currentBlockType != ScenarioTextBlockStyle::Parenthetical) {
						rtfFile.write(EMPTY_LINE);
				}

				//
				// Определить стиль блока
				//
				QString blockStyle = style(currentBlockType);

				//
				// Вставить текст
				//
				rtfFile.write("\\pard\\plain ");
				rtfFile.write(blockStyle.toUtf8().data());
				rtfFile.write(" ");
				rtfFile.write(stringToUtfCode(documentCursor.block().text()).toUtf8().data());
				rtfFile.write("\\par");
				rtfFile.write(END_OF_LINE);
			}

			//
			// Переходим к следующему блоку
			//
			documentCursor.movePosition(QTextCursor::EndOfBlock);
			documentCursor.movePosition(QTextCursor::NextBlock);
		}

		rtfFile.write("}");
		rtfFile.close();
	}
}

QString RtfExporter::header() const
{
	QString header = "\\rtf1\\ansi";
	//
	// Настройки шрифтов
	//
	header.append("{\\fonttbl\\f0\\fmodern\\fcharset0 Courier New;}");

	//
	// Настройки полей документа
	//
	header.append("\\margl2126\\margr1417\\margt1417\\margb709");

	//
	// Настройки используемых стилей
	//
	header.append("{\\stylesheet{\\f0 \\sbasedon222\\snext0 Normal;}"
				  "{\\s1\\f0\\caps\\fs24\\fi0\\ql\\sbasedon0 TIME AND PLACE;}"
				  "{\\s2\\f0\\fs24\\fi0\\ql\\sbasedon0 ACTION;}"
				  "{\\s3\\f0\\caps\\fs24\\fi0\\li3827\\ql\\sbasedon0 CHARACTER;}"
				  "{\\s4\\f0\\fs24\\fi0\\li3118\\ri2551\\ql\\sbasedon0 PARENTHETICAL;}"
				  "{\\s5\\f0\\fs24\\fi0\\li2126\\ri2126\\ql\\sbasedon0 DIALOG;}"
				  "{\\s6\\f0\\caps\\fs24\\fi0\\qr\\sbasedon0 TRANSITION;}"
				  "{\\s7\\f0\\caps\\fs24\\fi0\\ql\\sbasedon0 NOTE;}"
				  "}");

	return header;
}

QString RtfExporter::style(ScenarioTextBlockStyle::Type _type) const
{
	QString style = "\\s0";

	switch (_type) {
		case ScenarioTextBlockStyle::TimeAndPlace: {
			style = "\\s1\\f0\\caps\\fs24\\fi0\\ql";
			break;
		}

		case ScenarioTextBlockStyle::Action: {
			style = "\\s2\\f0\\fs24\\fi0\\ql";
			break;
		}

		case ScenarioTextBlockStyle::Character: {
			style = "\\s3\\f0\\caps\\fs24\\fi0\\li3827\\ql";
			break;
		}

		case ScenarioTextBlockStyle::Parenthetical:
		case ScenarioTextBlockStyle::Title: {
			style = "\\s4\\f0\\fs24\\fi0\\li3118\\ri2551\\ql";
			break;
		}

		case ScenarioTextBlockStyle::Dialog: {
			style = "\\s5\\f0\\fs24\\fi0\\li2126\\ri2126\\ql";
			break;
		}

		case ScenarioTextBlockStyle::Transition: {
			style = "\\s6\\f0\\caps\\fs24\\fi0\\qr";
			break;
		}

		case ScenarioTextBlockStyle::Note:
		case ScenarioTextBlockStyle::TitleHeader:
		case ScenarioTextBlockStyle::SceneGroupHeader:
		case ScenarioTextBlockStyle::SceneGroupFooter: {
			style = "\\s7\\f0\\caps\\fs24\\fi0\\ql";
			break;
		}

		default: {
			break;
		}
	}

	return style;
}

QString RtfExporter::stringToUtfCode(const QString& _text) const
{
	QString result;

	QString utfCharacter;
	foreach (QChar character, _text)	{
		utfCharacter.setNum(character.unicode());
		utfCharacter.prepend("\\u");
		result += utfCharacter;
	}

	return result;
}
