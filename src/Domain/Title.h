#ifndef TITLE_H
#define TITLE_H

#include "ScenarioObject.h"

#include <QString>


namespace Scenario
{
	/**
	 * @brief Класс блока титра
	 */
	class Title : public ScenarioObject
	{
	public:
		Title();

	private:
		/**
		 * @brief Текст титра
		 */
		QString m_titleText;
	};
}

#endif // TITLE_H
