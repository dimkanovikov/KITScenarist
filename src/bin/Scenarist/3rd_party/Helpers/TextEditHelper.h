#ifndef TEXTEDITHELPER_H
#define TEXTEDITHELPER_H

#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionMatch>


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
}

#endif // TEXTEDITHELPER_H
