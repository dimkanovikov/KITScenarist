#ifndef TRELBYIMPORTER_H
#define TRELBYIMPORTER_H

#include "AbstractImporter.h"


namespace BusinessLogic
{
    /**
     * @brief Импортер Trelby-документов
     */
    class TrelbyImporter : public AbstractImporter
    {
    public:
        TrelbyImporter();

        /**
         * @brief Импорт сценария из документа
         */
        QString importScenario(const ImportParameters& _importParameters) const;
    };
}

#endif // TRELBYIMPORTER_H
