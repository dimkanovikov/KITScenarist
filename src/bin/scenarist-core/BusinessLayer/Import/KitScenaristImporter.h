#ifndef KITSCENARISTIMPORTER_H
#define KITSCENARISTIMPORTER_H

#include "AbstractImporter.h"


namespace BusinessLogic
{
	/**
	 * @brief Импортер FDX-документов
	 */
	class KitScenaristImporter : public AbstractImporter
	{
	public:
		KitScenaristImporter();

		/**
		 * @brief Импорт сценария из документа
		 */
		QString importScenario(const ImportParameters& _importParameters) const;
	};
}

#endif // KITSCENARISTIMPORTER_H
