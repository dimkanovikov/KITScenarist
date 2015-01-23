#include "ScenarioTextBlockInfo.h"

#include <3rd_party/Helpers/TextEditHelper.h>

using namespace BusinessLogic;


ScenarioTextBlockInfo::ScenarioTextBlockInfo()
	: m_sceneNumber(0)
{
}

int ScenarioTextBlockInfo::sceneNumber() const
{
	return m_sceneNumber;
}

void ScenarioTextBlockInfo::setSceneNumber(int _number)
{
	if (m_sceneNumber != _number) {
		m_sceneNumber = _number;
	}
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
	// Обновим синопсис, если он изменился
	//
	if (m_synopsis != inputSynopsis) {
		m_synopsis = inputSynopsis;
	}
}
