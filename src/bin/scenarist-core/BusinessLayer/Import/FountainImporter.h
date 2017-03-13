#ifndef FOUNTAINIMPORTER_H
#define FOUNTAINIMPORTER_H

#include "AbstractImporter.h"
#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

class QXmlStreamWriter;

namespace BusinessLogic
{
    /**
     * @brief Импортер Fountain-документов
     */
    class FountainImporter : public AbstractImporter
    {
    public:
        FountainImporter();

        /**
         * @brief Импорт сценария из документа
         */
        QString importScenario(const ImportParameters &_importParameters) const;

    private:
        void writeBlock(QXmlStreamWriter &writer, QString paragraphText,
                        ScenarioBlockStyle::Type blockStyle) const;
        void reallyWriteBlock(QXmlStreamWriter &writer, const QString& paragraphText,
                              ScenarioBlockStyle::Type blockStyle) const;
        void writeComments(QXmlStreamWriter &writer) const;

        mutable unsigned prevBlockStart;
        mutable unsigned prevBlockLen;
        mutable bool noting = false;
        mutable bool commenting = false;
        mutable bool firstBlock = true;
        mutable QString text;
        mutable QString comment;
        mutable QVector<std::tuple<QString, unsigned, unsigned> > comments;
    };
}

#endif // FOUNTAINIMPORTER_H
