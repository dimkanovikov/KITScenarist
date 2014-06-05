#include "ScenarioTextBlockInfo.h"

#include <3rd_party/Helpers/TextEditHelper.h>

using namespace BusinessLogic;


ScenarioTextBlockInfo::ScenarioTextBlockInfo()
{
}

QString ScenarioTextBlockInfo::synopsis(bool htmlEscaped) const
{
	QString resultSynopsis = m_synopsis;
	if (htmlEscaped) {
		resultSynopsis = TextEditHelper::toHtmlEscaped(resultSynopsis);
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
		inputSynopsis = TextEditHelper::fromHtmlEscaped(inputSynopsis);
	}

	//
	// Уберём лишнее
	//
	inputSynopsis = TextEditHelper::removeDocumentTags(inputSynopsis);

	//
	// Проверим не пуст ли синопсис
	//
	QString plainSynopsis = TextEditHelper::removeHtmlTags(inputSynopsis);

	//
	// Если это не пустой синопсис, да ещё и новый, то обновим
	//
	if (!plainSynopsis.isEmpty()
		&& m_synopsis != inputSynopsis) {
		m_synopsis = inputSynopsis;
	}
}
