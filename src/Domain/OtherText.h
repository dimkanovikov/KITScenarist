#ifndef OTHERTEXT_H
#define OTHERTEXT_H

#include "ScenarioObject.h"

#include <QString>


namespace Scenario
{
	/**
	 * @brief Класс для прочих блоков (примечание, кадр и т.п.)
	 *
	 * @note На всю ширину страницы, заглавными буквами.
	 */
	class OtherText : public ScenarioObject
	{
	public:
		OtherText();

	private:
		/**
		 * @brief Текст блока прочее
		 */
		QString m_otherText;
	};
}

#endif // OTHERTEXT_H
