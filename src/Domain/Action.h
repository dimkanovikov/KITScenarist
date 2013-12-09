#ifndef ACTION_H
#define ACTION_H

#include "ScenarioObject.h"

#include <QString>


namespace Scenario
{
	/**
	 * @brief Класс блока описание действия
	 */
	class Action : public ScenarioObject
	{
	public:
		Action();

	private:
		/**
		 * @brief Текст блока описание действия
		 */
		QString m_actionText;
	};
}

#endif // ACTION_H
