#ifndef FOUNTAINEXPORTER_H
#define FOUNTAINEXPORTER_H

#include "AbstractExporter.h"

namespace BusinessLogic{
    /**
     * @brief Экспортер в Fountain
     */
    class FountainExporter : public AbstractExporter
    {
    public:
        FountainExporter();

        /**
         * @brief Экспорт заданного докумета в указанный файл
         */
        void exportTo(ScenarioDocument *_scenario, const ExportParameters &_exportParameters) const;
    };
}

#endif // FOUNTAINEXPORTER_H
