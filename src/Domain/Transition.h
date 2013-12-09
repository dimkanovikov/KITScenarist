#ifndef TRANSITION_H
#define TRANSITION_H

#include "ScenarioObject.h"

#include <QString>


namespace Scenario
{
	/**
	 * @brief Класс блока перехода
	 */
	class Transition : public ScenarioObject
	{
	public:
		Transition();

	private:
		/**
		 * @brief Название перехода
		 */
		QString m_transitionText;
	};
}

#endif // TRANSITION_H
