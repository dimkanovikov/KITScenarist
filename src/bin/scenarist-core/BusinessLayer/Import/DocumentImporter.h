#ifndef RTFIMPORTER_H
#define RTFIMPORTER_H

#include "AbstractImporter.h"


namespace BusinessLogic
{
	/**
	 * @brief Импортер документов
	 */
	class DocumentImporter : public AbstractImporter
	{
	public:
		DocumentImporter();

		/**
		 * @brief Импорт сценария из документа
		 */
		QString importScenario(const ImportParameters& _importParameters) const;
	};
}

#endif // RTFIMPORTER_H
