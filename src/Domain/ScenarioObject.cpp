#include "ScenarioObject.h"


namespace Scenario
{
	ScenarioObject::ScenarioObject(ScenarioObject* _parent) :
		m_parent(_parent)
	{
	}

	ScenarioObject::~ScenarioObject()
	{

	}
}
