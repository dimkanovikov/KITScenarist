#ifndef TEXTEDITHELPER_H
#define TEXTEDITHELPER_H

#include <QFontMetricsF>
#include <QLocale>
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
    inline static QString toHtmlEscaped(const QString& _text)
    {
        QString escapedText = _text;
        escapedText = escapedText.replace("&", "&amp;");
        escapedText = escapedText
                      .replace("<", "&lt;")
                      .replace(">", "&gt;")
                      .replace("\"", "&quot;")
                      .replace("\n", "&#10;");
        return escapedText;
    }

    /**
     * @brief Преобразовать html-специфичные символы к обычному виду
     */
    inline static QString fromHtmlEscaped(const QString& _escapedText)
    {
        QString text = _escapedText;
        text = text.replace("&amp;", "&");
        text = text
               .replace("&lt;", "<")
               .replace("&gt;", ">")
               .replace("&quot;", "\"")
               .replace("&#10;", "\n");
        return text;
    }

    /**
     * @brief Оставить только текст внутри тэгов body
     */
    inline static QString removeDocumentTags(const QString& _text)
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
    inline static QString removeHtmlTags(const QString& _text)
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

    inline static QString removeXmlTags(const QString& _text)
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
    inline static qreal fontLineHeight(const QFont& _font) {
        return QFontMetricsF(_font).lineSpacing();
    }

    /**
     * @brief Функции для получения корректных кавычек в зависимости от локали приложения
     */
    /** @{ */
    inline static QString localeQuote(bool _open) {
        QString quote = "";
        switch (QLocale().language()) {
            default: {
                quote = "\"";
                break;
            }

            case QLocale::Russian:
            case QLocale::Spanish:
            case QLocale::French: {
                if (_open) {
                    quote = "«";
                } else {
                    quote = "»";
                }
                break;
            }

            case QLocale::English: {
                if (_open) {
                    quote = "“";
                } else {
                    quote = "”";
                }
                break;
            }
        }

        return quote;
    }
    inline static QString localOpenQuote() { return localeQuote(true); }
    inline static QString localCloseQuote() { return localeQuote(false); }
    /** @{ */

    /**
     * @brief Украшения документа
     *
     * Например подмена многоточий и т.п.
     */
    inline static void beautifyDocument(QTextDocument* _document, bool _replaceThreeDots, bool _smartQuotes) {
        if (_document != 0) {
            QTextCursor cursor(_document);

            //
            // Заменяем три точки на многоточие
            //
            if (_replaceThreeDots) {
                while (!cursor.isNull() && !cursor.atEnd()) {
                    cursor = _document->find("...", cursor);

                    if (!cursor.isNull()) {
                        cursor.insertText("…");
                    }
                }
            }

            //
            // Ставим пробелы после многоточий
            //
            cursor = QTextCursor(_document);
            while (!cursor.isNull() && !cursor.atEnd()) {
                cursor = _document->find("…", cursor);

                if (!cursor.isNull() && !cursor.atBlockEnd()) {
                    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                    if (cursor.selectedText() != "… ") {
                        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                        cursor.insertText("… ");
                    }
                }
            }

            //
            // Корректируем кавычки
            //
            if (_smartQuotes) {
                cursor = QTextCursor(_document);
                while (!cursor.isNull() && !cursor.atEnd()) {
                    cursor = _document->find("\"", cursor);

                    if (!cursor.isNull()) {
                        QTextCursor cursorCopy = cursor;
                        cursorCopy.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
                        if (cursorCopy.selectedText().isEmpty()
                            || QStringList({" ", "("}).contains(cursorCopy.selectedText().right(1))) {
                            cursor.insertText(localOpenQuote());
                        } else {
                            cursor.insertText(localCloseQuote());
                        }
                    }
                }
            }
        }
    }

    /**
     * @brief Украшения документа при вводе текста
     *
     * Оптимизация, чтобы не просматривать весь документ
     */
    inline static void beautifyDocument(QTextCursor _cursor, const QString& _enteredText,
        bool _replaceThreeDots, bool _smartQuotes) {
        if (_replaceThreeDots && _enteredText == ".") {
            //
            // 3 предшествующих символа
            //
            _cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 3);

            if (_cursor.selectedText() == "...") {
                _cursor.insertText("…");
            }
        } else if (_smartQuotes && _enteredText == "\"") {
            //
            // Выделим введённый символ
            //
            _cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1);
            //
            // Определим предшествующий текст
            //
            QTextCursor cursorCopy = _cursor;
            cursorCopy.setPosition(_cursor.selectionStart());
            cursorCopy.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);

            if (cursorCopy.selectedText().isEmpty()
                || QStringList({" ", "("}).contains(cursorCopy.selectedText().right(1))) {
                _cursor.insertText(localOpenQuote());
            } else {
                _cursor.insertText(localCloseQuote());
            }
        }
    }
}

#endif // TEXTEDITHELPER_H
