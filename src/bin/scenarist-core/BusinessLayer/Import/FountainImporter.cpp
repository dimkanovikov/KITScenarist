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
        writer.setAutoFormatting(true);
        writer.setAutoFormattingIndent(true);
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
        unsigned dirNesting = 0;
        ScenarioBlockStyle::Type blockType;
        for (int i = 0; i != paragraphs.size(); ++i) {
            if (noting
                    || commenting) {
                //
                // Если мы комментируем или делаем заметку, то продолжим это
                //
                writeBlock(writer, paragraphs[i], prevBlockType);
                continue;
            }

            if (paragraphs[i].isEmpty()) {
                continue;
            }

            blockType = ScenarioBlockStyle::Action;
            QString paragraphText;

            switch(paragraphs[i].toStdString()[0]) {
            case '.':
                blockType = ScenarioBlockStyle::SceneHeading;
                paragraphText = paragraphs[i].right(paragraphs[i].size() - 1);
                break;
            case '!':
                blockType = ScenarioBlockStyle::Action;
                paragraphText = paragraphs[i].right(paragraphs[i].size() - 1);
                break;
            case '@':
                blockType = ScenarioBlockStyle::Character;
                paragraphText = paragraphs[i].right(paragraphs[i].size() - 1);
                break;
            case '>':
                if (paragraphs[i][paragraphs[i].size() - 1] == '<') {
                    blockType = ScenarioBlockStyle::Action;
                    paragraphText = paragraphs[i].mid(1, paragraphs[i].size() - 2);
                } else {
                    blockType = ScenarioBlockStyle::Transition;
                    paragraphText = paragraphs[i].right(paragraphs[i].size() - 1);
                }
                break;
            case '=':
            {
                bool isPageBreak = false;
                if (paragraphs[i].size() >= 3
                           && paragraphs[i][0] == paragraphs[i][1]
                           && paragraphs[i][1] == paragraphs[i][2]
                           && paragraphs[i][2] == '=') {
                    isPageBreak = true;
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
                }
                if (!isPageBreak) {
                    blockType = ScenarioBlockStyle::SceneDescription;
                    paragraphText = paragraphs[i].right(paragraphs[i].size() - 1);
                }
                break;
            }
            case '~':
                //
                // TODO: Вообще, это Lyrics блок. Но у нас такого нет
                //
                blockType = ScenarioBlockStyle::Action;
                paragraphText = paragraphs[i].right(paragraphs[i].size() - 1);
                break;
            case '#':
            {
                unsigned sharpCount = 0;
                while(paragraphs[i].toStdString()[sharpCount] == '#') {
                    ++sharpCount;
                }

                if (sharpCount <= dirNesting) {
                    for (unsigned i = 0; i != dirNesting - sharpCount + 1; ++i) {
                        writeBlock(writer, "", ScenarioBlockStyle::FolderFooter);
                    }
                    prevBlockType = ScenarioBlockStyle::FolderFooter;
                }
                writeBlock(writer, paragraphs[i].right(paragraphs[i].size() - sharpCount), ScenarioBlockStyle::FolderHeader);
                prevBlockType = ScenarioBlockStyle::FolderHeader;
                dirNesting = sharpCount;
                continue;
                break;
            }
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
                } else if (paragraphs[i].startsWith("[[")
                           && paragraphs[i].endsWith("]]")) {
                    //
                    // Комментарий
                    //
                    comments.append(std::make_tuple(paragraphs[i].mid(2, paragraphs[i].size() - 4), prevBlockStart, prevBlockLen));
                    prevBlockStart += prevBlockLen;
                    prevBlockLen = 0;
                    //writeComment(writer, paragraphs[i].mid(2, paragraphs[i].size() - 4), prevBlockStart, prevBlockLen);
                    continue;
                } else if (paragraphs[i].startsWith("/*")) {
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
                    if (paragraphs[i][paragraphs[i].size() - 1] == '^') {
                        //
                        // Двойной диалог, который мы пока что не умеем обрабатывать
                        //
                        paragraphText = paragraphs[i].left(paragraphs[i].size() - 1);
                    } else {
                        paragraphText = paragraphs[i];
                    }
                } else if (prevBlockType == ScenarioBlockStyle::Character
                           || prevBlockType == ScenarioBlockStyle::Parenthetical) {
                    //
                    // Если предыдущий блок - имя персонажа или ремарка, то сейчас диалог
                    //
                    blockType = ScenarioBlockStyle::Dialogue;
                    paragraphText = paragraphs[i];
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
            writeBlock(writer, paragraphText, blockType);
            //prevBlockLen = paragraphText.size();
            //prevBlockStart = 0;
            prevBlockType = blockType;
        }
        writeComments(writer);
        writer.writeEndElement();
        for (int i = 0; i != dirNesting; ++i) {
            writeBlock(writer, "", ScenarioBlockStyle::FolderFooter);
        }
        writer.writeEndElement();
        writer.writeEndDocument();
    }

    QFile file("/home/voltage/file.xml");
    file.open(QIODevice::WriteOnly);
    file.write(scenarioXml.toStdString().c_str());
    return scenarioXml;
}

void FountainImporter::writeBlock(QXmlStreamWriter& writer, QString paragraphText,
                                  ScenarioBlockStyle::Type type) const
{
    if (!firstBlock) {
        if (!noting
                && !commenting) {
            text.clear();
            text.reserve(paragraphText.size());
            comment.clear();

            writeComments(writer);

            prevBlockLen = 0;
            prevBlockStart = 0;
        }
    }

    const QString& blockTypeName = ScenarioBlockStyle::typeName(type);

    char prevSymbol = '\0';
    //writer.writeStartElement(blockTypeName);

    for (int i = 0; i != paragraphText.size(); ++i) {
        if (prevSymbol == '\\') {
            if (noting) {
                comment.append(paragraphText[i]);
            }
            else {
                text.append(paragraphText[i]);
            }
            continue;
        }
        switch (paragraphText.toStdString()[i]) {
        case '\\':
            if (noting) {
                comment.append(paragraphText[i]);
            } else {
                text.append(paragraphText[i]);
            }
            break;
        case '/':
            if (prevSymbol == '*'
                    && commenting) {
                commenting = false;
                prevBlockStart += prevBlockLen;
                prevBlockLen = text.size() - 1;
                writer.writeEndElement();
                reallyWriteBlock(writer, text.left(text.size() - 1), ScenarioBlockStyle::NoprintableText);
                text.clear();
            } else {
                if (noting) {
                    comment.append('/');
                } else {
                    text.append('/');
                }
            }
            break;
        case '*':
            if (prevSymbol == '/'
                    && !commenting
                    && !noting) {
                writeComments(writer);
                comments.clear();

                commenting = true;
                prevBlockStart += prevBlockLen;
                prevBlockLen = text.size() - 1;
                writer.writeEndElement();
                if (text.size() != 1) {
                    reallyWriteBlock(writer, text.left(text.size() - 1), type);
                }
                text.clear();
            } else {
                if (noting) {
                    comment.append('*');
                } else {
                    text.append('*');
                }
            }
            break;
        case '[':
            if (prevSymbol == '['
                    && !commenting
                    && !noting) {
                noting = true;
                prevBlockLen = text.size() - 1 - prevBlockStart;
                text = text.left(text.size() - 1);
            } else {
                if (noting) {
                    comment.append('[');
                } else {
                    text.append('[');
                }
            }
            break;
        case ']':
            if (prevSymbol == ']'
                    && noting) {
                noting = false;
                comments.append(std::make_tuple(comment.left(comment.size() - 1), prevBlockStart, prevBlockLen));
                prevBlockStart += prevBlockLen;
                prevBlockLen = 0;
                comment.clear();
            } else {
                if (noting) {
                    comment.append(']');
                } else {
                    text.append(']');
                }
            }
            break;
        case '_':
            break;
        default:
            if (noting) {
                comment.append(paragraphText[i]);
            } else {
                text.append(paragraphText[i]);
            }
            break;
        }
        prevSymbol = paragraphText.toStdString()[i];
    }


    if (!noting
            && !commenting) {
        prevBlockLen += text.size() - prevBlockStart;
        if (!firstBlock) {
            writer.writeEndElement();
        }
        if (!text.isEmpty()) {
            reallyWriteBlock(writer, text, type);
        }
        /*
        writer.writeStartElement(NODE_VALUE);
        writer.writeCDATA(text.trimmed());
        writer.writeEndElement();
        */
        text.clear();
    }
    if (!firstBlock) {
        firstBlock = false;
    }
}

void FountainImporter::reallyWriteBlock(QXmlStreamWriter &writer, const QString &paragraphText,
                                        ScenarioBlockStyle::Type type) const
{
    const QString& blockTypeName = ScenarioBlockStyle::typeName(type);
    writer.writeStartElement(blockTypeName);
    writer.writeStartElement(NODE_VALUE);
    writer.writeCDATA(paragraphText.trimmed());
    writer.writeEndElement();
    //writer.writeEndElement();
}

void FountainImporter::writeComments(QXmlStreamWriter &writer) const
{
    if (comments.isEmpty()) {
        return;
    }

    writer.writeStartElement("reviews");

    for (unsigned int i = 0; i != comments.size(); ++i) {
        if (std::get<2>(comments[i]) != 0) {
            if (i != 0) {
                writer.writeEndElement();
            }
            writer.writeStartElement("review");
            writer.writeAttribute("from", QString::number(std::get<1>(comments[i])));
            writer.writeAttribute("length", QString::number(std::get<2>(comments[i])));
            writer.writeAttribute("bgcolor", "#ffff00");
            writer.writeAttribute("is_highlight", "true");
        }
        writer.writeEmptyElement("review_comment");
        writer.writeAttribute("comment", std::get<0>(comments[i]));
    }

    writer.writeEndElement();

    writer.writeEndElement();

    comments.clear();
}
