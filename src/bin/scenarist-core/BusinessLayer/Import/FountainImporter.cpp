#include "FountainImporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <QDomDocument>
#include <QFile>
#include <QStack>
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

/**
  * @brief С чего может начинаться название сцены
  */
const QStringList sceneHeadings = {QApplication::translate("BusinessLayer::FountainImporter", "INT"),
                                   QApplication::translate("BusinessLayer::FountainImporter", "EXT"),
                                   QApplication::translate("BusinessLayer::FountainImporter", "EST"),
                                   QApplication::translate("BusinessLayer::FountainImporter", "INT./EXT"),
                                   QApplication::translate("BusinessLayer::FountainImporter", "INT/EXT"),
                                   QApplication::translate("BusinessLayer::FountainImporter", "I/E")};
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

        const int paragraphsCount = paragraphs.size();
        ScenarioBlockStyle::Type prevBlockType = ScenarioBlockStyle::Undefined;
        QStack<QString> dirs;
        ScenarioBlockStyle::Type blockType;
        for (int i = 0; i != paragraphsCount; ++i) {
            if (notation
                    || commenting) {
                //
                // Если мы комментируем или делаем заметку, то продолжим это
                //
                processBlock(writer, paragraphs[i], prevBlockType);
                continue;
            }

            if (paragraphs[i].isEmpty()) {
                continue;
            }

            blockType = ScenarioBlockStyle::Action;
            QString paragraphText;

            switch(paragraphs[i].toStdString()[0]) {
                case '.':
                {
                    blockType = ScenarioBlockStyle::SceneHeading;
                    //
                    // TODO: номера сцен игнорируем, поскольку в фонтане они являются строками
                    //
                    int sharpPos = paragraphs[i].size();
                    if (paragraphs[i].endsWith("#")) {
                        sharpPos = paragraphs[i].lastIndexOf('#', paragraphs[i].size() - 2);
                    }
                    if (sharpPos == -1) {
                        sharpPos = paragraphs[i].size();
                    }
                    paragraphText = paragraphs[i].mid(1, sharpPos - 1);
                    break;
                }

                case '!':
                {
                    blockType = ScenarioBlockStyle::Action;
                    paragraphText = paragraphs[i].mid(1);
                    break;
                }

                case '@':
                {
                    blockType = ScenarioBlockStyle::Character;
                    paragraphText = paragraphs[i].mid(1);
                    break;
                }

                case '>':
                {
                    if (paragraphs[i].endsWith("<")) {
                        blockType = ScenarioBlockStyle::Action;
                        paragraphText = paragraphs[i].mid(1, paragraphs[i].size() - 2);
                    } else {
                        blockType = ScenarioBlockStyle::Transition;
                        paragraphText = paragraphs[i].mid(1);
                    }
                    break;
                }

                case '=':
                {
                    bool isPageBreak = false;
                    if (paragraphs[i].startsWith("===")) {
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
                        paragraphText = paragraphs[i].mid(1);
                    }
                    break;
                }

                case '~':
                {
                    //
                    // TODO: Вообще, это Lyrics блок. Но у нас такого нет
                    //
                    blockType = ScenarioBlockStyle::Action;
                    paragraphText = paragraphs[i].mid(1);
                    break;
                }

                case '#':
                {
                    //
                    // Директории
                    //
                    int sharpCount = 0;
                    while(paragraphs[i].toStdString()[sharpCount] == '#') {
                        ++sharpCount;
                    }

                    if (sharpCount <= dirs.size()) {
                        //
                        // Закроем нужное число раз уже открытые
                        //
                        unsigned toClose = dirs.size() - sharpCount + 1;
                        for (unsigned i = 0; i != toClose; ++i) {
                            processBlock(writer, "КОНЕЦ " + dirs.top(), ScenarioBlockStyle::FolderFooter);
                            dirs.pop();
                        }
                        prevBlockType = ScenarioBlockStyle::FolderFooter;
                    }
                    //
                    // И откроем новую
                    //
                    QString text = paragraphs[i].mid(sharpCount);
                    processBlock(writer, text, ScenarioBlockStyle::FolderHeader);
                    dirs.push(text);
                    prevBlockType = ScenarioBlockStyle::FolderHeader;

                    //
                    // Поскольку директории добавляются прямо здесь без обработки, то в конец цикла идти не надо
                    //
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
                            && i + 1 < paragraphsCount
                            && paragraphs[i + 1].isEmpty()) {
                        //
                        // Если начинается с одного из времен действия, а после обязательно пустая строка
                        // Значит это заголовок сцены
                        //
                        blockType = ScenarioBlockStyle::SceneHeading;

                        //
                        // TODO: номера сцен игнорируем, поскольку в фонтане они являются строками
                        //
                        int sharpPos = paragraphs[i].size();
                        if (paragraphs[i].startsWith("#")) {
                            sharpPos = paragraphs[i].lastIndexOf('#', paragraphs[i].size() - 2);
                        }
                        if (sharpPos == -1) {
                            sharpPos = paragraphs[i].size();
                        }
                        paragraphText = paragraphs[i].left(sharpPos);
                    } else if (paragraphs[i].startsWith("[[")
                               && paragraphs[i].endsWith("]]")) {
                        //
                        // Редакторская заметка
                        //
                        notes.append(std::make_tuple(paragraphs[i].mid(2, paragraphs[i].size() - 4), noteStartPos, noteLen));
                        noteStartPos += noteLen;
                        noteLen = 0;
                        continue;
                    } else if (paragraphs[i].startsWith("/*")) {
                        //
                        // Начинается комментарий
                        paragraphText = paragraphs[i];
                    } else if (paragraphs[i] == paragraphs[i].toUpper()
                               && i != 0
                               && paragraphs[i-1].isEmpty()
                               && i + 1 < paragraphsCount
                               && paragraphs[i+1].isEmpty()
                               && paragraphs[i].endsWith("TO:")) {
                        //
                        // Если состоит только из заглавных букв, предыдущая и следующая строки пустые
                        // и заканчивается "TO:", то это переход
                        //
                        blockType = ScenarioBlockStyle::Transition;
                        paragraphText = paragraphs[i].left(paragraphs[i].size()-4);
                    } else if (paragraphs[i].startsWith("(")
                               && paragraphs[i].endsWith(")")
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
                               && i + 1 < paragraphsCount
                               && !paragraphs[i+1].isEmpty()) {
                        //
                        // Если состоит из только из заглавных букв, впереди не пустая строка, а перед пустая
                        // Значит это имя персонажа (для реплики)
                        //
                        blockType = ScenarioBlockStyle::Character;
                        if (paragraphs[i].endsWith("^")) {
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
                    } else {
                        //
                        // Во всех остальных случаях - Action
                        //
                        blockType = ScenarioBlockStyle::Action;
                        paragraphText = paragraphs[i];
                    }
                }
            }
            //
            // Отправим блок на обработку
            //
            processBlock(writer, paragraphText, blockType);
            prevBlockType = blockType;
        }
        //
        // Добавим комментарии к последнему блоку
        //
        appendComments(writer);

        //
        // Закроем последний блок
        //
        writer.writeEndElement();

        //
        // Закроем директории нужное число раз
        //
        while (!dirs.empty()) {
            processBlock(writer, "КОНЕЦ " + dirs.top(), ScenarioBlockStyle::FolderFooter);
            dirs.pop();
        }

        //
        // Закроем документ
        //
        writer.writeEndElement();
        writer.writeEndDocument();
    }

    return scenarioXml;
}

void FountainImporter::processBlock(QXmlStreamWriter& writer, QString paragraphText,
                                    ScenarioBlockStyle::Type type) const
{
    if (!notation
            && !commenting) {
        //
        // Начинается новая сущность
        //
        text.reserve(paragraphText.size());

        //
        // Добавим комментарии к предыдущему блоку
        //
        appendComments(writer);

        noteLen = 0;
        noteStartPos = 0;
    }

    char prevSymbol = '\0';

    for (int i = 0; i != paragraphText.size(); ++i) {
        if (prevSymbol == '\\') {
            //
            // Если предыдущий символ - \, то просто добавим текущий
            //
            if (notation) {
                note.append(paragraphText[i]);
            }
            else {
                text.append(paragraphText[i]);
            }
            continue;
        }
        switch (paragraphText.toStdString()[i]) {
            case '\\':
            {
                if (notation) {
                    note.append(paragraphText[i]);
                } else {
                    text.append(paragraphText[i]);
                }
                break;
            }

            case '/':
            {
                if (prevSymbol == '*'
                        && commenting) {
                    //
                    // Заканчивается комментирование
                    //
                    commenting = false;
                    noteStartPos += noteLen;
                    noteLen = text.size() - 1;

                    //
                    // Закроем предыдущий блок, добавим текущий
                    //
                    writer.writeEndElement();
                    appendBlock(writer, text.left(text.size() - 1), ScenarioBlockStyle::NoprintableText);
                    text.clear();
                } else {
                    if (notation) {
                        note.append('/');
                    } else {
                        text.append('/');
                    }
                }
                break;
            }

            case '*':
            {
                if (prevSymbol == '/'
                        && !commenting
                        && !notation) {
                    //
                    // Начинается комментирование
                    //
                    commenting = true;
                    noteStartPos += noteLen;
                    noteLen = text.size() - 1;

                    //
                    // Закроем предыдущий блок и, если комментирование начинается в середние текущего блока
                    // то добавим этот текущий блок
                    //
                    writer.writeEndElement();
                    if (text.size() != 1) {
                        appendBlock(writer, text.left(text.size() - 1), type);
                        appendComments(writer);
                        notes.clear();
                    }
                    text.clear();
                } else {
                    if (notation) {
                        note.append('*');
                    } else {
                        //
                        // Игнорируем *, поскольку они являются символом форматирования, которое мы еще не умеем
                        //
                        //text.append('*');
                    }
                }
                break;
            }

            case '[':
            {
                if (prevSymbol == '['
                        && !commenting
                        && !notation) {
                    //
                    // Начинается редакторская заметка
                    //
                    notation = true;
                    noteLen = text.size() - 1 - noteStartPos;
                    text = text.left(text.size() - 1);
                } else {
                    if (notation) {
                        note.append('[');
                    } else {
                        text.append('[');
                    }
                }
                break;
            }

            case ']':
            {
                if (prevSymbol == ']'
                        && notation) {
                    //
                    // Закончилась редакторская заметка. Добавим ее в список редакторских заметок к текущему блоку
                    //
                    notation = false;
                    notes.append(std::make_tuple(note.left(note.size() - 1), noteStartPos, noteLen));
                    noteStartPos += noteLen;
                    noteLen = 0;
                    note.clear();
                } else {
                    if (notation) {
                        note.append(']');
                    } else {
                        text.append(']');
                    }
                }
                break;
            }

            case '_':
                //
                // Игнорируем подчеркивания, которые являются символом форматирования
                //
                break;
            default:
            {
                //
                // Самый обычный символ
                //
                if (notation) {
                    note.append(paragraphText[i]);
                } else {
                    text.append(paragraphText[i]);
                }
                break;
            }
        }
        prevSymbol = paragraphText.toStdString()[i];
    }


    if (!notation
            && !commenting) {
        //
        // Если блок действительно закончился
        //
        noteLen += text.size() - noteStartPos;

        //
        // Закроем предыдущий блок
        //
        if (!firstBlock) {
            writer.writeEndElement();
        }

        //
        // Добавим текущий блок
        //
        if (!text.isEmpty() || type == ScenarioBlockStyle::FolderFooter) {
            appendBlock(writer, text, type);
        }
        text.clear();
    }

    //
    // Первый блок в тексте может встретиться лишь однажды
    //
    if (!firstBlock) {
        firstBlock = false;
    }
}

void FountainImporter::appendBlock(QXmlStreamWriter &writer, const QString &paragraphText,
                                   ScenarioBlockStyle::Type type) const
{
    const QString& blockTypeName = ScenarioBlockStyle::typeName(type);
    writer.writeStartElement(blockTypeName);
    writer.writeStartElement(NODE_VALUE);
    writer.writeCDATA(paragraphText.trimmed());
    writer.writeEndElement();

    //
    // Не закрываем блок, чтобы можно было добавить редакторских заметок
    //
    //writer.writeEndElement();
}

void FountainImporter::appendComments(QXmlStreamWriter &writer) const
{
    if (notes.isEmpty()) {
        return;
    }

    writer.writeStartElement("reviews");

    for (int i = 0; i != notes.size(); ++i) {
        if (std::get<2>(notes[i]) != 0) {
            if (i != 0) {
                writer.writeEndElement();
            }
            writer.writeStartElement("review");
            writer.writeAttribute("from", QString::number(std::get<1>(notes[i])));
            writer.writeAttribute("length", QString::number(std::get<2>(notes[i])));
            writer.writeAttribute("bgcolor", "#ffff00");
            writer.writeAttribute("is_highlight", "true");
        }
        writer.writeEmptyElement("review_comment");
        writer.writeAttribute("comment", std::get<0>(notes[i]));
    }

    writer.writeEndElement();

    writer.writeEndElement();

    notes.clear();
}
