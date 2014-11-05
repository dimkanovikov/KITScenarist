#ifndef TEXTEDITHELPER_H
#define TEXTEDITHELPER_H

#include <QFontMetricsF>
#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QTextCursor>
#include <QTextDocument>


namespace
{
	/**
	 * @brief Размер шрифта для функции определения высоты строки
	 */
	const qreal DEFINED_FONT_POINT_SIZEF = 12.0;

	/**
	 * @brief Предопределённые высоты шрифтов для размера 12pt
	 */
	static QMap<QString, qreal> definedFontLineHeight() {
		static QMap<QString, qreal> s_definedFontLineHeight;
		if (s_definedFontLineHeight.isEmpty()) {
			//
			// В маке всё определяется отлично
			//
	#ifndef Q_OS_MAC
			s_definedFontLineHeight.insert("Courier New", 18.1);
			s_definedFontLineHeight.insert("Times New Roman", 18.5);
			s_definedFontLineHeight.insert("Arial", 18.3);
			s_definedFontLineHeight.insert("CourierCyrillic", 13.1);
	#endif
		}
		return s_definedFontLineHeight;
	}
}

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
	 * @brief Рассчитать высоту строки заданного шрифта
	 */
	static qreal fontLineHeight(const QFont& _font) {
		qreal realLineHeight = 0;

		//
		// Для некоторых шрифтов высоты предопределены
		//
		if (definedFontLineHeight().contains(_font.family())) {
			realLineHeight = definedFontLineHeight().value(_font.family()) * _font.pointSizeF() / DEFINED_FONT_POINT_SIZEF;
		} else {
			const QFontMetricsF fontMetrics(_font);
			realLineHeight = fontMetrics.lineSpacing();

			/*
			 * Пока оставим этот код до лучших времён
			 *

			//
			// Qt грязно округляет высоту линии считанную из файла шрифта,
			// поэтому восстанавливаем его при помощи следующей хитрости:
			// 1. определяем расстояние между строк для 100пт размера шрифта
			// 2. вычисляем расстояние мужду строк для заданного размера шрифта
			//
			QFont font100pt = _font;
			font100pt.setPointSizeF(100.0);
			QFontInfo font100ptInfo(font100pt);
			const QFontMetricsF font100ptMetrics(font100pt);
			const qreal lineHeightFont100pt = (font100ptMetrics.lineSpacing() + font100ptMetrics.height()) / 2;
			//
			// "- 0.1" - появилось, т.к. некоторые неточности не удаётся победить даже таким костылём
			//
			// ... работает для большинства протестированных мною шрифтов
			//
			realLineHeight = lineHeightFont100pt * _font.pointSizeF() / font100ptInfo.pointSizeF() - 0.1;
			*/
		}

		return realLineHeight;
	}

	/**
	 * @brief Украшения документа
	 *
	 * Например подмена многоточий и т.п.
	 */
	static void beautifyDocument(QTextDocument* _document) {
		QTextCursor cursor(_document);
		while (!cursor.isNull() && !cursor.atEnd()) {
			cursor = _document->find("...", cursor);

			if (!cursor.isNull()) {
				cursor.insertText("…");
			}
		}
	}
}

#endif // TEXTEDITHELPER_H
