#include "FountainExporter.h"

//#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>
#include <BusinessLayer/ScenarioDocument/ScenarioTextBlockInfo.h>

#include <QFile>
#include <QTextBlock>

using namespace BusinessLogic;

namespace {
    QStringList sceneHeadingStart = {"INT", "EXT", "EST", "INT./EXT", "INT/EXT",
                                     "ИНТ", "ЭКСТ"};
}

FountainExporter::FountainExporter() :
    AbstractExporter()
{
}

void FountainExporter::exportTo(ScenarioDocument *_scenario, const ExportParameters &_exportParameters) const
{
    //
    // Открываем документ на запись
    //
    QFile fountainFile(_exportParameters.filePath);
    if (fountainFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
       ExportParameters fakeParameters;
       fakeParameters.saveReviewMarks = true;
       fakeParameters.saveNoprintable = true;
       QTextDocument* preparedDocument = prepareDocument(_scenario, fakeParameters);
       QTextCursor documentCursor(preparedDocument);
       bool isFirst = true;
       unsigned dirNesting = 0;
       while (!documentCursor.atEnd()) {
           QString paragraphText;
           if (!documentCursor.block().text().isEmpty()) {
                paragraphText = documentCursor.block().text();
                QVector<QTextLayout::FormatRange> notes;

                //
                // Не знаю, какая это магия, но если вместо этого цикла использовать remove_copy_if
                // или copy_if, то получаем сегфолт
                //
                for (const QTextLayout::FormatRange& format : documentCursor.block().textFormats()) {
                    if (!format.format.property(ScenarioBlockStyle::PropertyComments).toStringList().isEmpty()) {
                        notes.push_back(format);
                    }
                }

                bool fullBlockComment = true;
                if (notes.size() != 1
                        || (!notes.isEmpty()
                            && notes[0].length != paragraphText.size())) {
                    fullBlockComment = false;
                    for (int i = notes.size() - 1; i >= 0; --i) {
                        QStringList comments = notes[i].format.property(ScenarioBlockStyle::PropertyComments)
                                .toStringList();
                        for (int j = comments.size() - 1; j >= 0; --j) {
                            paragraphText.insert(notes[i].start + notes[i].length, "[[" + comments[j] + "]]");
                        }
                    }
                }
               switch (ScenarioBlockStyle::forBlock(documentCursor.block())) {
               case ScenarioBlockStyle::SceneHeading:
               {
                   bool startsWithHeading = false;
                   for (const QString& heading : sceneHeadingStart) {
                        if (paragraphText.startsWith(heading)) {
                            startsWithHeading = true;
                            break;
                        }
                   }

                   if (!startsWithHeading) {
                       paragraphText = "." + paragraphText;
                   }
                   if (_exportParameters.printScenesNumbers) {
                    QTextBlockUserData* textBlockData = documentCursor.block().userData();
                    ScenarioTextBlockInfo* sceneInfo = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
                    if (sceneInfo != 0 && sceneInfo->sceneNumber()) {
                        paragraphText += " #" + QString::number(sceneInfo->sceneNumber()) + "#";
                    }
                   }

                   if (!isFirst) {
                       paragraphText = '\n' + paragraphText;
                   }
               }
                   break;
               case ScenarioBlockStyle::Character:
               {
                   if (paragraphText == paragraphText.toUpper()) {
                       paragraphText = '\n' + paragraphText;
                   } else {
                       paragraphText = '@' + paragraphText;
                   }
               }
                   break;
               case ScenarioBlockStyle::Dialogue:
               {
               }
                   break;
               case ScenarioBlockStyle::Parenthetical:
               {
                   paragraphText = '(' + paragraphText + ")";
               }
                   break;
               case ScenarioBlockStyle::Transition:
               {
                   if (paragraphText.toUpper() == paragraphText) {
                       paragraphText = paragraphText + " TO:";
                   } else {
                       paragraphText = "> " + paragraphText;
                   }
               }
                   break;
               case ScenarioBlockStyle::NoprintableText:
               {
                   paragraphText = "/*\n" + paragraphText + "\n*/";
               }
                   break;
               case ScenarioBlockStyle::Action:
               {
               }
                   break;
               case ScenarioBlockStyle::FolderHeader:
               {
                   ++dirNesting;
                   paragraphText = " " + paragraphText;
                   for (unsigned i = 0; i != dirNesting; ++i) {
                       paragraphText = paragraphText + "#";
                   }
               }
               case ScenarioBlockStyle::FolderFooter:
               {
                   --dirNesting;
               }
                   break;
               default:
               {
                     continue;
               }
               }
               paragraphText += '\n';
               if (fullBlockComment) {
                   paragraphText += '\n';
                   QStringList comments = notes[0].format
                           .property(ScenarioBlockStyle::PropertyComments).toStringList();
                   for (const QString& comment: comments) {
                       paragraphText += "[[" + comment + "]]\n";
                   }
                   paragraphText += '\n';
               }
               fountainFile.write(paragraphText.toLocal8Bit());
           }
           documentCursor.movePosition(QTextCursor::EndOfBlock);
           documentCursor.movePosition(QTextCursor::NextBlock);
       }
    }
}
