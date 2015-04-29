#ifndef TEXTEDITHELPER_H
#define TEXTEDITHELPER_H

#include <QFontMetricsF>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QString>
#include <QTextCursor>
#include <QTextDocument>


namespace TextEditHelper
{
	/**
	 * @brief Преобразовать специфичные символы к html-виду
	 */
	static QString toHtmlEscaped(const QString& _text)
	{
		QString escapedText = _text;
		escapedText =
				escapedText
				.replace("<", "&lt;")
				.replace(">", "&gt;")
				.replace("\"", "&quot;");
		return escapedText;
	}

	/**
	 * @brief Преобразовать html-специфичные символы к обычному виду
	 */
	static QString fromHtmlEscaped(const QString& _escapedText)
	{
		QString text = _escapedText;
		text =  text
				.replace("&lt;", "<")
				.replace("&gt;", ">")
				.replace("&quot;", "\"");
		return text;
	}

	/**
	 * @brief Оставить только текст внутри тэгов body
	 */
	static QString removeDocumentTags(const QString& _text)
	{
		//
		// Регулярное выражение для изъятия из текста html-документа его содержимого
		//
		const QRegularExpression RX_HTML_DOCUMENT_CLEANER(
					"<body([^>]*)>(.*)</body>",
					QRegularExpression::MultilineOption
					| QRegularExpression::DotMatchesEverythingOption);

		QRegularExpressionMatch match = RX_HTML_DOCUMENT_CLEANER.match(_text);
		QString textWithoutDocumentTags = _text;
		if (match.hasMatch()) {
			textWithoutDocumentTags = match.captured(2);
		}
		return textWithoutDocumentTags;
	}

	/**
	 * @brief Удалить все html-тэги из текста
	 */
	static QString removeHtmlTags(const QString& _text)
	{
		const QRegularExpression RX_HTML_TAGS_CLEANER(
				"<([^>]*)>",
				QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);

		QString plainText = _text;
		plainText = plainText.remove(RX_HTML_TAGS_CLEANER);
		plainText = plainText.simplified();
		return plainText;
	}

	/**
	 * @brief Удалить все xml-тэги из текста
	 */

	static QString removeXmlTags(const QString& _text)
	{
		const QString STR_CDATA_START = "<![CDATA[";
		const QString STR_CDATA_END = "]]>";
		const QRegularExpression RX_HTML_TAGS_CLEANER(
				"<([^>]*)>",
				QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);

		QString plainText = _text;
		plainText = plainText.remove(STR_CDATA_START).remove(STR_CDATA_END);
		plainText = plainText.remove(RX_HTML_TAGS_CLEANER);
		return plainText;
	}

	/**
	 * @brief Рассчитать высоту строки заданного шрифта
	 */
	static qreal fontLineHeight(const QFont& _font) {
		return QFontMetricsF(_font).lineSpacing();
	}

	/**
	 * @brief Украшения документа
	 *
	 * Например подмена многоточий и т.п.
	 */
	static void beautifyDocument(QTextDocument* _document) {
		if (_document != 0) {
			QTextCursor cursor(_document);
			while (!cursor.isNull() && !cursor.atEnd()) {
				cursor = _document->find("...", cursor);

				if (!cursor.isNull()) {
					cursor.insertText("…");
				}
			}
		}
	}

	/**
	 * @brief Украшения документа при вводе текста
	 *
	 * Оптимизация, чтобы не просматривать весь документ
	 */
	static void beautifyDocument(QTextCursor _cursor, const QString& _enteredText) {
		if (_enteredText == ".") {
			//
			// Получим значения
			//
			// ... 3 предшествующих символа
			_cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 3);

			if (_cursor.selectedText() == "...") {
				_cursor.insertText("…");
			}
		}
	}
}

#endif // TEXTEDITHELPER_H
