#ifndef PARENTHETICAL_H
#define PARENTHETICAL_H

#include "ScenarioObject.h"

#include <QString>


namespace Scenario
{
	/**
	 * @brief Класс блока ремарки
	 */
	class Parenthetical : public ScenarioObject
	{
	public:
		Parenthetical();

	private:
		/**
		 * @brief Текст ремарки
		 */
		QString m_parentheticalText;
	};
}

#endif // PARENTHETICAL_H
