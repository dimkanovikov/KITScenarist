#include "ScenarioTextBlockInfo.h"

#include <3rd_party/Helpers/TextEditHelper.h>

#include <QUuid>

using namespace BusinessLogic;


ScenarioTextBlockInfo::ScenarioTextBlockInfo()
	: m_uuid(QUuid::createUuid().toString()), m_sceneNumber(0)
{
}

QString ScenarioTextBlockInfo::uuid() const
{
	return m_uuid;
}

void ScenarioTextBlockInfo::setUuid(const QString& _uuid)
{
	if (m_uuid!= _uuid) {
		m_uuid= _uuid;
	}
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

QString ScenarioTextBlockInfo::description() const
{
    return m_description;
}

void ScenarioTextBlockInfo::setDescription(const QString& _description)
{
	//
	// Обновим описание, если он изменился
	//
    if (m_description != _description) {
        m_description = _description;
	}
}

ScenarioTextBlockInfo* ScenarioTextBlockInfo::clone() const
{
	ScenarioTextBlockInfo* copy = new ScenarioTextBlockInfo;
	copy->m_uuid = m_uuid;
	copy->m_sceneNumber = m_sceneNumber;
	copy->m_colors = m_colors;
	copy->m_description = m_description;
	return copy;
}
