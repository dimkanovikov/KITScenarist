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

QString ScenarioTextBlockInfo::colors() const
{
	return m_colors;
}

void ScenarioTextBlockInfo::setColors(const QString& _colors)
{
	if (m_colors != _colors) {
		m_colors = _colors;
	}
}

QString ScenarioTextBlockInfo::title() const
{
	return m_title;
}

void ScenarioTextBlockInfo::setTitle(const QString& _title)
{
	if (m_title != _title) {
		m_title = _title;
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
	copy->m_colors = m_colors;
	copy->m_description = m_description;
	return copy;
}
