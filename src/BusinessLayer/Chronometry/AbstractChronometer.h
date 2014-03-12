#ifndef ABSTRACTCHRONOMETER_H
#define ABSTRACTCHRONOMETER_H

#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockStyle.h>

class QString;


namespace BusinessLogic
{
	/**
	 * @brief Базовый класс рассчёта хронометража
	 */
	class AbstractChronometer
	{
	public:
		virtual ~AbstractChronometer() {}

		/**
		 * @brief Наименование хронометра
		 */
		virtual QString name() const = 0;

		/**
		 * @brief Подсчитать длительность заданного текста определённого типа
		 */
		virtual float calculateFrom(
				BusinessLogic::ScenarioTextBlockStyle::Type _type, const QString& _text) const = 0;
	};
}

#endif // ABSTRACTCHRONOMETER_H
