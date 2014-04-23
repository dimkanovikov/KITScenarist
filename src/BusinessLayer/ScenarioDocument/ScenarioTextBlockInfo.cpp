#include "ScenarioTextBlockInfo.h"

using namespace BusinessLogic;


ScenarioTextBlockInfo::ScenarioTextBlockInfo(const QString& _synopsis) :
	m_synopsis(_synopsis)
{
}

QString ScenarioTextBlockInfo::synopsis() const
{
	return m_synopsis;
}

void ScenarioTextBlockInfo::setSynopsis(const QString& _synopsis)
{
	if (m_synopsis != _synopsis) {
		m_synopsis = _synopsis;
	}
}
