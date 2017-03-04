#include "FountainImporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <QDomDocument>
#include <QFile>
#include <QXmlStreamWriter>

using namespace BusinessLogic;

namespace {
    /**
     * @brief Ключ для формирования xml из импортируемого документа
     */
    /** @{ */
    const QString NODE_SCENARIO = "scenario";
    const QString NODE_VALUE = "v";

    const QString ATTRIBUTE_VERSION = "version";
    /** @} */
}

FountainImporter::FountainImporter() :
    AbstractImporter()
{

}

QString FountainImporter::importScenario(const ImportParameters &_importParameters) const
{
    QString scenarioXml;

    //
    // Открываем файл
    //
    QFile fountainFile(_importParameters.filePath);
    if (fountainFile.open(QIODevice::ReadOnly)) {
        //
        // Читаем plain text
        //
        // ... и пишем в сценарий
        //
        QXmlStreamWriter writer(&scenarioXml);
        writer.writeStartDocument();
        writer.writeStartElement(NODE_SCENARIO);
        writer.writeAttribute(ATTRIBUTE_VERSION, "1.0");

        //
        // Текст сценария
        //
        QVector<QString> paragraphs;
        for (const QString& str : QString(fountainFile.readAll()).split("\n")) {
            paragraphs.push_back(str.trimmed());
        }

        const QStringList sceneHeadings = {"INT", "EXT", "EST", "INT./EXT", "INT/EXT", "I/E"
                                          "ИНТ", "ЭКСТ"};
        ScenarioBlockStyle::Type prevBlockType = ScenarioBlockStyle::Undefined;
        bool isComment = false;
        bool isNote = false;
        unsigned dirNesting = 0;
        ScenarioBlockStyle::Type blockType;
        QString noteParagraph;
        for (int i = 0; i != paragraphs.size(); ++i) {
            if (paragraphs[i].isEmpty()) {
                continue;
            }

            blockType = ScenarioBlockStyle::Action;
            QString paragraphText;

            switch(paragraphs[i].toStdString()[0]) {
            case '.':
                blockType = ScenarioBlockStyle::SceneHeading;
                paragraphText = paragraphs[i].right(paragraphs[i].size());
                break;
            case '!':
                blockType = ScenarioBlockStyle::Action;
                paragraphText = paragraphs[i].right(paragraphs[i].size());
                break;
            case '@':
                blockType = ScenarioBlockStyle::Character;
                paragraphText = paragraphs[i].right(paragraphs[i].size());
                break;
            case '~':
                //
                // TODO: Вообще, это Lyrics блок. Но у нас такого нет
                //
                blockType = ScenarioBlockStyle::Action;
                paragraphText = paragraphs[i].right(paragraphs[i].size());
                break;
            case '#':
                int sharpCount = 0;
                while(paragraphText[i][sharpCount] == '#') {
                    ++sharpCount;
                }

                if (sharpCount < dirNesting) {
                    for (int i = 0; i != dirNesting - sharpCount; ++i) {
                        writeBlock(writer, "", ScenarioBlockStyle::FolderFooter);
                    }
                    prevBlockType = ScenarioBlockStyle::FolderFooter;
                } else {
                    writeBlock(writer, paragraphText[i].right(sharpCount), ScenarioBlockStyle::FolderHeader);
                    prevBlockType = ScenarioBlockStyle::FolderHeader;
                }
                continue;
            default:
            {
                bool startsWithHeading = false;
                for (const QString &sceneHeading : sceneHeadings) {
                    if (paragraphs[i].startsWith(sceneHeading)) {
                        startsWithHeading = true;
                        break;
                    }
                }

                if (startsWithHeading
                        && i + 1 < paragraphs.size()
                        && paragraphs[i + 1].isEmpty()) {
                    //
                    // Если начинается с одного из интерьерных слов, а после обязательно пустая строка
                    // Значит это заголовок сцены
                    //
                    blockType = ScenarioBlockStyle::SceneHeading;
                    paragraphText = paragraphs[i];
                } else if (paragraphs[i] == paragraphs[i].toUpper()
                           && i != 0
                           && paragraphs[i-1].isEmpty()
                           && i + 1 < paragraphs.size()
                           && !paragraphs[i+1].isEmpty()) {
                    //
                    // Если состоит из только из заглавных букв, впереди не пустая строка, а перед пустая
                    // Значит это имя персонажа (для реплики)
                    //
                    blockType = ScenarioBlockStyle::Character;
                    paragraphText = paragraphs[i];
                } else if (prevBlockType == ScenarioBlockStyle::Character
                           || prevBlockType == ScenarioBlockStyle::Parenthetical) {
                    //
                    // Если предыдущий блок - имя персонажа или ремарка, то сейчас диалог
                    //
                    blockType = ScenarioBlockStyle::Dialogue;
                    if (paragraph[i][paragraphs[i].size() - 1] == '^') {
                        //
                        // Двойной диалог, который мы пока что не умеем обрабатывать
                        //
                        paragraphText = paragraphs[i].left(paragraphs[i].size() - 1);
                    } else {
                        paragraphText = paragraphs[i];
                    }
                } else if (paragraphs[i][0] == '('
                           && paragraphs[i][paragraphs[i].size()-1] == ')'
                           && (prevBlockType == ScenarioBlockStyle::Character
                               || prevBlockType == ScenarioBlockStyle::Dialogue)) {
                    //
                    // Если текущий блок обернут в (), то это ремарка
                    //
                    blockType = ScenarioBlockStyle::Parenthetical;
                    paragraphText = paragraphs[i];
                } else if (paragraphs[i] == paragraphs[i].toUpper()
                           && i != 0
                           && paragraphs[i-1].isEmpty()
                           && i + 1 < paragraphs.size()
                           && paragraphs[i+1].isEmpty()
                           && paragraphs[i].endsWith("TO:")) {
                    //
                    // Если состоит только из заглавных букв, предыдущая и следующая строки пустые
                    // и заканчивается "TO:", то это переход
                    //
                    blockType = ScenarioBlockStyle::Transition;
                    paragraphText = paragraphs[i].left(paragraphs[i].size()-4);
                } else if (paragraphs[i].size() >= 3
                           && paragraphs[i][0] == paragraphs[i][1]
                           && paragraphs[i][1] == paragraphs[i][2]
                           && paragraphs[i][2] == '=') {
                    bool isPageBreak = true;
                    for (int j = 3; j != paragraphs[i].size(); ++j) {
                        if (paragraphs[i][j] != '=') {
                            isPageBreak = false;
                            break;
                        }
                    }

                    //
                    // Если состоит из трех или более '=', то это PageBreak
                    // У нас такого сейчас нет
                    //
                    continue;
                } else if (paragraphs[i].startsWith("[[")
                           && paragraphs[i].endsWith("]]")) {
                    //
                    // Строка обернута в [[ ]], значит это заметка
                    //
                    blockType = ScenarioBlockStyle::Note;
                    paragraphText = paragraphs[i].left(paragraphs[i].size()-2).right(paragraphs[i].size()-4);
                } else {
                    //
                    // Во всех остальных случаях - Action
                    //
                    blockType = ScenarioBlockStyle::Action;
                    paragraphText = paragraphs[i];
                }
            }
            }
            int notePos = 0;
            int unnotePos = -1;
            if (isNote) {
                unnotePos = paragraphText.indexOf("]]", notePos);
                if (unnotePos == -1) {
                    noteParagraph += paragraphText;
                    continue;
                }
                else {
                    noteParagraph += paragraphText.left(unnotePos);
                    writeBlock(writer, noteParagraph, ScenarioBlockStyle::Note);
                }
            }
            int commentPos = 0;
            int uncommentPos = -1;

            if (isComment) {
                uncommentPos = paragraphText.indexOf("*/", commentPos);

                writeBlock(writer, paragraphText.mid(commentPos, uncommentPos - commentPos - 2),
                           ScenarioBlockStyle::NoprintableText);
            }

            while ((commentPos = paragraphText.indexOf("/*", uncommentPos)) != -1) {
                //
                // Тут есть комментариий
                //
                isComment = true;

                uncommentPos = paragraphText.indexOf("*/", commentPos);

                writeBlock(writer, paragraphText.mid(commentPos + 2, uncommentPos == -1 ? -1 : uncommentPos - commentPos - 4),
                           blockType);

                if (uncommentPos == -1) {
                    writeBlock(writer, paragraphText.mid(commentPos + 2), ScenarioBlockStyle::NoprintableText);
                    break;
                } else {
                    writeBlock(writer, paragraphText.mid(commentPos + 2, uncommentPos - commentPos - 4), ScenarioBlockStyle::NoprintableText);
                    isComment = false;
                }

            }

            if (!isComment) {
                writeBlock(writer, paragraphText.mid(commentPos), blockType);
            }
            prevBlockType = blockType;
        }
    }
    return scenarioXml;
}

void FountainImporter::writeBlock(QXmlStreamWriter& writer, QString paragraphText,
                                  ScenarioBlockStyle::Type type) const
{
    if (paragraphText[0] == '>'
            && paragraphText[paragraphText.size() - 1] == '<') {
        paragraphText = paragraphText.mid(1, paragraphText.size() - 2);
    }

    QString text;
    text.reserve(paragraphText.size());

    bool escaping = false;

    for (int i = 0; i != paragraphText.size(); ++i) {
        if (escaping) {
            escaping = false;
            text.append(paragraphText[i]);
            continue;
        } if (paragraphText[i] == '\\') {
            escaping = true;
            text.append(paragraphText[i]);
            continue;
        }
        if (paragraphText[i] == '*'
                || paragraphText[i] == '_') {
            continue;
        } else {
            text.append(paragraphText[i]);
        }
    }


    const QString& blockTypeName = ScenarioBlockStyle::typeName(type);
    writer.writeStartElement(blockTypeName);
    writer.writeStartElement(NODE_VALUE);
    writer.writeCDATA(text.trimmed());
    writer.writeEndElement();
    writer.writeEndElement();
}
