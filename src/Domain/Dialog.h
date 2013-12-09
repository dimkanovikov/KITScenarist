#ifndef DIALOG_H
#define DIALOG_H

#include "ScenarioObject.h"

#include <QString>


namespace Scenario
{
	/**
	 * @brief Класс блока реплики персонажа
	 */
	class Dialog : public ScenarioObject
	{
	public:
		Dialog();

	private:


		/**
		 * @brief Текст реплики
		 */
		QString m_dialogText;
	};
}

#endif // DIALOG_H
