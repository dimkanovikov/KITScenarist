#ifndef FDXIMPORTER_H
#define FDXIMPORTER_H

#include "AbstractImporter.h"


namespace BusinessLogic
{
	/**
	 * @brief Импортер FDX-документов
	 */
	class FdxImporter : public AbstractImporter
	{
	public:
		FdxImporter();

		/**
		 * @brief Импорт сценария из документа
		 */
		QString importScenario(const ImportParameters& _importParameters) const;
	};
}

#endif // FDXIMPORTER_H
