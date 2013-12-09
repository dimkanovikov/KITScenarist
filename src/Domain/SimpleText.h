#ifndef SIMPLETEXT_H
#define SIMPLETEXT_H

#include "ScenarioObject.h"

#include <QString>


namespace Scenario
{
	/**
	 * @brief Класс блока простого текста
	 */
	class SimpleText : public ScenarioObject
	{
	public:
		SimpleText();

	private:
		/**
		 * @brief Текст блока
		 */
		QString m_simpleText;
	};
}

#endif // SIMPLETEXT_H
