#ifndef RTFIMPORTER_H
#define RTFIMPORTER_H

#include "AbstractImporter.h"


namespace BusinessLogic
{
	/**
	 * @brief Импортер RTF-документа
	 */
	class RtfImporter : public AbstractImporter
	{
	public:
		RtfImporter();

		/**
		 * @brief Импорт сценария из rtf-документа
		 */
		QString importScenario(const ImportParameters &_importParameters) const;
	};
}

#endif // RTFIMPORTER_H
