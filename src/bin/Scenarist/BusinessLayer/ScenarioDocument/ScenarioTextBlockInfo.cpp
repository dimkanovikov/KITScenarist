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

QColor ScenarioTextBlockInfo::color() const
{
	return m_color;
}

void ScenarioTextBlockInfo::setColor(const QColor& _color)
{
	if (m_color != _color) {
		m_color = _color;
	}
}

QString ScenarioTextBlockInfo::description(bool htmlEscaped) const
{
	QString resultDescription = m_description;
	if (htmlEscaped) {
		resultDescription = TextEditHelper::toHtmlEscaped(resultDescription);
	}

	return resultDescription;
}

void ScenarioTextBlockInfo::setDescription(const QString& _description, bool htmlEscaped)
{
	//
	// При необходимости избавимся от html-преобразованных символов
	//
	QString inputDescription = _description;
	if (htmlEscaped) {
		inputDescription = TextEditHelper::fromHtmlEscaped(inputDescription);
	}

	//
	// Уберём лишнее
	//
	inputDescription = TextEditHelper::removeDocumentTags(inputDescription);

	//
	// Обновим описание, если он изменился
	//
	if (m_description != inputDescription) {
		m_description = inputDescription;
	}
}

ScenarioTextBlockInfo* ScenarioTextBlockInfo::clone() const
{
	ScenarioTextBlockInfo* copy = new ScenarioTextBlockInfo;
	copy->m_sceneNumber = m_sceneNumber;
	copy->m_color = m_color;
	copy->m_description = m_description;
	return copy;
}
