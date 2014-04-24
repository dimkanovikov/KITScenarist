#include "ScenarioTextBlockInfo.h"

#include <QRegularExpression>

using namespace BusinessLogic;


ScenarioTextBlockInfo::ScenarioTextBlockInfo()
{
}

namespace {
	/**
	 * @brief Преобразовать специфичные символы к html-виду
	 */
	QString toHtmlEscaped(const QString& _text) {
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
	QString fromHtmlEscaped(const QString& _escapedText) {
		QString text = _escapedText;
		text =  text
				.replace("&lt;", "<")
				.replace("&gt;", ">")
				.replace("&quot;", "\"");
		return text;
	}

	/**
	 * @brief Регулярное выражение для изъятия из текста html-документа его содержимого
	 */
	const QRegularExpression RX_HTML_DOCUMENT_CLEANER(
			"<body([^>]*)>(.*)</body>",
			QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);

	const QRegularExpression RX_HTML_TAGS_CLEANER(
			"<([^>]*)>",
			QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);
}

QString ScenarioTextBlockInfo::synopsis(bool htmlEscaped) const
{
	QString resultSynopsis = m_synopsis;
	if (htmlEscaped) {
		resultSynopsis = ::toHtmlEscaped(resultSynopsis);
	}

	return resultSynopsis;
}

void ScenarioTextBlockInfo::setSynopsis(const QString& _synopsis, bool htmlEscaped)
{
	//
	// При необходимости избавимся от html-преобразованных символов
	//
	QString inputSynopsis = _synopsis;
	if (htmlEscaped) {
		inputSynopsis = ::fromHtmlEscaped(inputSynopsis);
	}

	//
	// Уберём лишнее
	//
	QRegularExpressionMatch match = RX_HTML_DOCUMENT_CLEANER.match(inputSynopsis);
	if (match.hasMatch()) {
		inputSynopsis = match.captured(2);
	}

	//
	// Проверим не пуст ли синопсис
	//
	QString plainSynopsis = inputSynopsis;
	plainSynopsis = plainSynopsis.remove(RX_HTML_TAGS_CLEANER);
	plainSynopsis = plainSynopsis.simplified();

	//
	// Если это не пустой синопсис, да ещё и новый, то обновим
	//
	if (!plainSynopsis.isEmpty()
		&& m_synopsis != inputSynopsis) {
		m_synopsis = inputSynopsis;
	}
}
