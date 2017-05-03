#include "ScenarioXml.h"

#include "ScenarioDocument.h"
#include "ScenarioTextDocument.h"
#include "ScenarioModelItem.h"
#include "ScenarioTemplate.h"
#include "ScenarioTextBlockInfo.h"

#include <3rd_party/Helpers/TextEditHelper.h>

#include <QApplication>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextCursor>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

using namespace BusinessLogic;

namespace {
    const QString NODE_SCENARIO = "scenario";
    const QString NODE_VALUE = "v";
    const QString NODE_REVIEW_GROUP = "reviews";
    const QString NODE_REVIEW = "review";
    const QString NODE_REVIEW_COMMENT = "review_comment";

    const QString ATTRIBUTE_VERSION = "version";
    const QString ATTRIBUTE_DESCRIPTION = "description";
    const QString ATTRIBUTE_UUID = "uuid";
    const QString ATTRIBUTE_COLOR = "color";
    const QString ATTRIBUTE_TITLE = "title";
    const QString ATTRIBUTE_REVIEW_FROM = "from";
    const QString ATTRIBUTE_REVIEW_LENGTH = "length";
    const QString ATTRIBUTE_REVIEW_COLOR = "color";
    const QString ATTRIBUTE_REVIEW_BGCOLOR = "bgcolor";
    const QString ATTRIBUTE_REVIEW_IS_HIGHLIGHT = "is_highlight";
    const QString ATTRIBUTE_REVIEW_DONE = "done";
    const QString ATTRIBUTE_REVIEW_COMMENT = "comment";
    const QString ATTRIBUTE_REVIEW_AUTHOR = "author";
    const QString ATTRIBUTE_REVIEW_DATE = "date";

    const QString SCENARIO_XML_VERSION = "1.0";

    /**
     * @brief Есть ли в блоке редакторские заметки
     */
    static bool hasReviewMarks(const QTextBlock& _block) {
        bool hasMarks = false;
        if (!_block.textFormats().isEmpty()) {
            foreach (const QTextLayout::FormatRange& range, _block.textFormats()) {
                if (range.format.boolProperty(ScenarioBlockStyle::PropertyIsReviewMark)) {
                    hasMarks = true;
                    break;
                }
            }
        }
        return hasMarks;
    }

    /**
     * @brief Сформировать хэш для текстового блока
     */
    static inline uint blockHash(const QTextBlock& _block)
    {
        //
        // Формируем уникальную строку, главное, чтобы два разных блока не имели одинакового хэша,
        // но в то же время, нельзя опираться на позицию блока, т.к. при смещение текста на абзац
        // вниз, придётся пересчитывать хэши всех остальных блоков
        //
        QString hash;
        hash.append(_block.text());
        hash.append("#");
        hash.append(_block.revision());
        if (ScenarioTextBlockInfo* blockInfo = dynamic_cast<ScenarioTextBlockInfo*>(_block.userData())) {
            hash.append("#");
            hash.append(blockInfo->uuid());
            hash.append("#");
            hash.append(QString::number(blockInfo->sceneNumber()));
            hash.append("#");
            hash.append(blockInfo->colors());
            hash.append("#");
            hash.append(blockInfo->title());
            hash.append("#");
            hash.append(blockInfo->description());
        }
        foreach (const QTextLayout::FormatRange& range, _block.textFormats()) {
            hash.append("#");
            hash.append(QString::number(range.start));
            hash.append("#");
            hash.append(QString::number(range.length));
            hash.append("#");
            hash.append(range.format.foreground().color().name());
            hash.append("#");
            hash.append(range.format.background().color().name());
            hash.append("#");
            hash.append(range.format.boolProperty(ScenarioBlockStyle::PropertyIsReviewMark) ? "true" : "false");
            hash.append("#");
            hash.append(range.format.boolProperty(ScenarioBlockStyle::PropertyIsHighlight) ? "true" : "false");
            hash.append("#");
            hash.append(range.format.boolProperty(ScenarioBlockStyle::PropertyIsDone) ? "true" : "false");
            hash.append("#");
            hash.append(range.format.property(ScenarioBlockStyle::PropertyComments).toStringList().join("#"));
            hash.append("#");
            hash.append(range.format.property(ScenarioBlockStyle::PropertyCommentsAuthors).toStringList().join("#"));
            hash.append("#");
            hash.append(range.format.property(ScenarioBlockStyle::PropertyCommentsDates).toStringList().join("#"));
        }
        hash.append("#");
        hash.append(ScenarioBlockStyle::forBlock(_block));

        return qHash(hash);
    }
}


QString ScenarioXml::defaultCardsXml()
{
    return "<?xml version=\"1.0\"?>\n"
           "<cards x=\"0\" y=\"0\" width=\"0\" height=\"0\" scale=\"1\" >\n"
           "<Card id=\"{000000-0000000-000000}\" is_folder=\"false\" title=\"\" description=\"\" stamp=\"\" colors=\"\" x=\"0\" y=\"0\" />\n"
           "</cards_xml>";
}

QString ScenarioXml::defaultTextXml()
{
    return makeMimeFromXml(
            "<scene_heading uuid=\"{000000-0000000-000000}\">\n"
            "<v><![CDATA[]]></v>\n"
            "</scene_heading>\n");
}

QString ScenarioXml::makeMimeFromXml(const QString& _xml)
{
    const QString XML_HEADER = "<?xml version=\"1.0\"?>\n";
    const QString SCENARIO_HEADER = "<scenario version=\"1.0\">\n";
    const QString SCENARIO_FOOTER = "</scenario>\n";

    QString mimeXml = _xml;
    if (!mimeXml.contains(XML_HEADER)) {
        if (!mimeXml.contains(SCENARIO_HEADER)) {
            mimeXml.prepend(SCENARIO_HEADER);
        }
        mimeXml.prepend(XML_HEADER);
    }
    if (!mimeXml.endsWith(SCENARIO_FOOTER)) {
        mimeXml.append(SCENARIO_FOOTER);
    }
    return mimeXml;
}

ScenarioXml::ScenarioXml(ScenarioDocument* _scenario) :
    m_scenario(_scenario),
    m_lastMimeFrom(0),
    m_lastMimeTo(0)
{
    Q_ASSERT(m_scenario);

    m_xmlCache.setMaxCost(2000);
}

QString ScenarioXml::scenarioToXml()
{
    //
    // Для формирования xml не используем QXmlStreamWriter, т.к. нам нужно хранить по отдельности
    // xml каждого блока, а QXmlStreamWriter не всегда закрывает последний записанный тэг,
    // оставляя место для записи атрибутов. В результате это приводит к появлению в xml
    // странных последовательностей, наподобии ">>" или ">/>"
    //

    QString resultXml;

    QTextBlock currentBlock = m_scenario->document()->begin();
    QString currentBlockXml;
    do {
        currentBlockXml.clear();
        const uint currentBlockHash = ::blockHash(currentBlock);

        //
        // Если для блока есть кэш, используем его
        //
        if (m_xmlCache.contains(currentBlockHash)) {
            resultXml.append(m_xmlCache[currentBlockHash]);
        }
        //
        // В противном случае формируем xml
        //
        else {
            //
            // Определим тип текущего блока
            //
            ScenarioBlockStyle::Type currentType = ScenarioBlockStyle::forBlock(currentBlock);

            //
            // Получить текст под курсором
            //
            QString textToSave = TextEditHelper::toHtmlEscaped(currentBlock.text());

            //
            // Определить параметры текущего абзаца
            //
            bool needWrite = true; // пишем абзац?
            QString currentNode = ScenarioBlockStyle::typeName(currentType); // имя текущей ячейки
            bool canHaveColors = false; // может иметь цвета
            switch (currentType) {
                case ScenarioBlockStyle::SceneHeading: {
                    canHaveColors = true;
                    break;
                }

                case ScenarioBlockStyle::Parenthetical: {
                    needWrite = !textToSave.isEmpty();
                    break;
                }

                case ScenarioBlockStyle::FolderHeader: {
                    canHaveColors = true;
                    break;
                }

                default: {
                    break;
                }
            }

            //
            // Дописать xml
            //
            if (needWrite) {
                //
                // Если возможно, сохраним uuid, цвета элемента и его заголовок
                //
                QString uuidColorsAndTitle;
                if (canHaveColors) {
                    ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(currentBlock.userData());
                    if (info == nullptr) {
                        info = new ScenarioTextBlockInfo;
                        currentBlock.setUserData(info);
                    }
                    //
                    if (!info->uuid().isEmpty()) {
                        uuidColorsAndTitle = QString(" %1=\"%2\"").arg(ATTRIBUTE_UUID, info->uuid());
                    }
                    if (!info->colors().isEmpty()) {
                        uuidColorsAndTitle += QString(" %1=\"%2\"").arg(ATTRIBUTE_COLOR, info->colors());
                    }
                    if (!info->title().isEmpty()) {
                        uuidColorsAndTitle += QString(" %1=\"%2\"").arg(ATTRIBUTE_TITLE, TextEditHelper::toHtmlEscaped(info->title()));
                    }
                }

                //
                // Открыть ячейку текущего элемента
                //
                currentBlockXml.append(QString("<%1%2>\n").arg(currentNode, uuidColorsAndTitle));

                //
                // Пишем текст текущего элемента
                //
                currentBlockXml.append(QString("<%1><![CDATA[%2]]></%1>\n").arg(NODE_VALUE, textToSave));

                //
                // Пишем редакторские комментарии, если они есть в блоке
                //
                if (::hasReviewMarks(currentBlock)) {
                    currentBlockXml.append(QString("<%1>\n").arg(NODE_REVIEW_GROUP));
                    foreach (const QTextLayout::FormatRange& range, currentBlock.textFormats()) {
                        bool isReviewMark =
                            range.format.boolProperty(ScenarioBlockStyle::PropertyIsReviewMark);

                        //
                        // Все редакторские правки, и только, если выделен записываемый текст
                        //
                        if (isReviewMark) {
                            currentBlockXml.append(QString("<%1").arg(NODE_REVIEW));
                            currentBlockXml.append(QString(" %1=\"%2\"").arg(ATTRIBUTE_REVIEW_FROM, QString::number(range.start)));
                            currentBlockXml.append(QString(" %1=\"%2\"").arg(ATTRIBUTE_REVIEW_LENGTH, QString::number(range.length)));
                            if (range.format.hasProperty(QTextFormat::ForegroundBrush)) {
                                currentBlockXml.append(QString(" %1=\"%2\"").arg(ATTRIBUTE_REVIEW_COLOR, range.format.foreground().color().name()));
                            }
                            if (range.format.hasProperty(QTextFormat::BackgroundBrush)) {
                                currentBlockXml.append(QString(" %1=\"%2\"").arg(ATTRIBUTE_REVIEW_BGCOLOR, range.format.background().color().name()));
                            }
                            currentBlockXml.append(QString(" %1=\"%2\"").arg(ATTRIBUTE_REVIEW_IS_HIGHLIGHT,
                                range.format.boolProperty(ScenarioBlockStyle::PropertyIsHighlight) ? "true" : "false"));
                            currentBlockXml.append(QString(" %1=\"%2\"").arg(ATTRIBUTE_REVIEW_DONE,
                                range.format.boolProperty(ScenarioBlockStyle::PropertyIsDone) ? "true" : "false"));
                            currentBlockXml.append(">\n");
                            //
                            // ... комментарии
                            //
                            const QStringList comments = range.format.property(ScenarioBlockStyle::PropertyComments).toStringList();
                            const QStringList authors = range.format.property(ScenarioBlockStyle::PropertyCommentsAuthors).toStringList();
                            const QStringList dates = range.format.property(ScenarioBlockStyle::PropertyCommentsDates).toStringList();
                            for (int commentIndex = 0; commentIndex < comments.size(); ++commentIndex) {
                                currentBlockXml.append(QString("<%1").arg(NODE_REVIEW_COMMENT));
                                currentBlockXml.append(QString(" %1=\"%2\"").arg(ATTRIBUTE_REVIEW_COMMENT,
                                    TextEditHelper::toHtmlEscaped(comments.at(commentIndex))));
                                currentBlockXml.append(QString(" %1=\"%2\"").arg(ATTRIBUTE_REVIEW_AUTHOR, authors.at(commentIndex)));
                                currentBlockXml.append(QString(" %1=\"%2\"").arg(ATTRIBUTE_REVIEW_DATE, dates.at(commentIndex)));
                                currentBlockXml.append("/>\n");
                            }
                            //
                            currentBlockXml.append(QString("</%1>\n").arg(NODE_REVIEW));
                        }
                    }
                    currentBlockXml.append(QString("</%1>\n").arg(NODE_REVIEW_GROUP));
                }

                //
                // Закрываем текущий элемент
                //
                currentBlockXml.append(QString("</%1>\n").arg(currentNode));
            }

            m_xmlCache.insert(currentBlockHash, new QString(currentBlockXml));
            resultXml.append(currentBlockXml);
        }
        currentBlock = currentBlock.next();
    } while (currentBlock.isValid());

    return makeMimeFromXml(resultXml);
}

QString ScenarioXml::scenarioToXml(int _startPosition, int _endPosition, bool _correctLastMime)
{
    QString resultXml;

    //
    // Если необходимо обработать весь текст
    //
    bool isFullDocumentSave = false;
    if (_startPosition == 0
        && _endPosition == 0) {
        isFullDocumentSave = true;
        _endPosition = m_scenario->document()->characterCount();
    }

    //
    // Сохраним позиции
    //
    if (_correctLastMime) {
        m_lastMimeFrom = _startPosition;
        m_lastMimeTo = _endPosition;
    }

    //
    // Получим курсор для редактирования
    //
    QTextCursor cursor(m_scenario->document());

    //
    // Переместимся к началу текста для формирования xml
    //
    cursor.setPosition(_startPosition);

    //
    // Подсчитаем кол-во незакрытых папок, и закроем, если необходимо
    //
    int openedFolders = 0;

    QXmlStreamWriter writer(&resultXml);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(0);
    writer.writeStartDocument();
    writer.writeStartElement(NODE_SCENARIO);
    writer.writeAttribute(ATTRIBUTE_VERSION, "1.0");
    bool isFirstBlock = true;
    do {
        //
        // Для всего документа сохраняем блоками
        //
        if (isFullDocumentSave) {
            //
            // Если не первый блок, перейдём к следующему
            //
            if (!isFirstBlock) {
                cursor.movePosition(QTextCursor::NextBlock);
            } else {
                isFirstBlock = false;
            }
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        }
        //
        // Для некоторого текста, посимвольно
        //
        else {
            //
            // ... перебегаем на следующий блок, чтобы не захватыывать символ переноса строки
            //
            if (!cursor.hasSelection()
                && cursor.atBlockEnd()) {
                cursor.movePosition(QTextCursor::NextCharacter);
            }
            //
            // ... если текущий блок не пуст, выделяем его текст
            //
            if (!cursor.block().text().isEmpty()) {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            }
        }

        //
        // Курсор в конце текущего блока
        // или в конце выделения
        //
        if (cursor.atBlockEnd()
            || cursor.position() == _endPosition) {
            //
            // Текущий блок
            //
            QTextBlock currentBlock = cursor.block();

            //
            // Определим тип текущего блока
            //
            ScenarioBlockStyle::Type currentType = ScenarioBlockStyle::forBlock(currentBlock);

            //
            // Получить текст под курсором
            //
            QString textToSave = TextEditHelper::toHtmlEscaped(cursor.selectedText());

            //
            // Определить параметры текущего абзаца
            //
            bool needWrite = true; // пишем абзац?
            QString currentNode = ScenarioBlockStyle::typeName(currentType); // имя текущей ячейки
            bool canHaveUuidColorsAndTitle = false; // может иметь цвета
            switch (currentType) {
                case ScenarioBlockStyle::SceneHeading: {
                    canHaveUuidColorsAndTitle = true;
                    break;
                }

                case ScenarioBlockStyle::Parenthetical: {
                    needWrite = !textToSave.isEmpty();
                    break;
                }

                case ScenarioBlockStyle::FolderHeader: {
                    canHaveUuidColorsAndTitle = true;

                    ++openedFolders;

                    break;
                }

                case ScenarioBlockStyle::FolderFooter: {
                    //
                    // Закрываем папки, если были открыты, то просто корректируем счётчик,
                    // а если открытых нет, то не записываем и конец
                    //
                    if (openedFolders > 0) {
                        --openedFolders;
                    } else {
                        needWrite = false;
                    }
                    break;
                }

                default: {
                    break;
                }
            }

            //
            // Дописать xml
            //
            if (needWrite) {
                //
                // Открыть ячейку текущего элемента
                //
                writer.writeStartElement(currentNode);

                //
                // Если возможно, сохраним uuid, цвета элемента и его название
                //
                if (canHaveUuidColorsAndTitle) {
                    ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(currentBlock.userData());
                    if (info == nullptr) {
                        ScenarioTextBlockInfo* info = new ScenarioTextBlockInfo;
                        currentBlock.setUserData(info);
                    }
                    //
                    if (!info->uuid().isEmpty()) {
                        writer.writeAttribute(ATTRIBUTE_UUID, info->uuid());
                    }
                    if (!info->colors().isEmpty()) {
                        writer.writeAttribute(ATTRIBUTE_COLOR, info->colors());
                    }
                    if (!info->title().isEmpty()) {
                        writer.writeAttribute(ATTRIBUTE_TITLE, TextEditHelper::toHtmlEscaped(info->title()));
                    }
                }

                //
                // Пишем текст текущего элемента
                //
                writer.writeStartElement(NODE_VALUE);
                writer.writeCDATA(textToSave);
                writer.writeEndElement();

                //
                // Пишем редакторские комментарии, если они есть в блоке
                //
                if (::hasReviewMarks(currentBlock)) {
                    writer.writeStartElement(NODE_REVIEW_GROUP);
                    foreach (const QTextLayout::FormatRange& range, currentBlock.textFormats()) {
                        bool isReviewMark =
                            range.format.boolProperty(ScenarioBlockStyle::PropertyIsReviewMark);

                        //
                        // Корректируем позиции выделения
                        //
                        int start = range.start;
                        int length = range.length;
                        bool isSelected = false;
                        //
                        // Если выделение разрывает редакторские правки
                        //
                        if (cursor.selectionStart() < (cursor.block().position() + range.start + range.length)
                            || cursor.selectionEnd() > (cursor.block().position() + range.start)) {

                            isSelected = true;

                            if (cursor.selectionStart() > (cursor.block().position() + range.start)) {
                                start = 0;
                            } else {
                                start = range.start - (cursor.selectionStart() - currentBlock.position());
                            }

                            if (cursor.selectionEnd() < (cursor.block().position() + range.start + range.length)) {
                                if (cursor.selectionStart() > (cursor.block().position() + range.start)) {
                                    length = cursor.selectionEnd() - cursor.selectionStart();
                                } else {
                                    length -= cursor.block().position() + range.start + range.length - cursor.selectionEnd();
                                }
                            } else {
                                if (cursor.selectionStart() > (cursor.block().position() + range.start)) {
                                    length = currentBlock.position() + range.start + range.length - cursor.selectionStart();
                                }
                            }
                        }



                        //
                        // Все редакторские правки, и только, если выделен записываемый текст
                        //
                        if (isReviewMark && isSelected) {
                            writer.writeStartElement(NODE_REVIEW);
                            writer.writeAttribute(ATTRIBUTE_REVIEW_FROM, QString::number(start));
                            writer.writeAttribute(ATTRIBUTE_REVIEW_LENGTH, QString::number(length));
                            if (range.format.hasProperty(QTextFormat::ForegroundBrush)) {
                                writer.writeAttribute(ATTRIBUTE_REVIEW_COLOR, range.format.foreground().color().name());
                            }
                            if (range.format.hasProperty(QTextFormat::BackgroundBrush)) {
                                writer.writeAttribute(ATTRIBUTE_REVIEW_BGCOLOR, range.format.background().color().name());
                            }
                            writer.writeAttribute(ATTRIBUTE_REVIEW_IS_HIGHLIGHT,
                                range.format.boolProperty(ScenarioBlockStyle::PropertyIsHighlight) ? "true" : "false");
                            writer.writeAttribute(ATTRIBUTE_REVIEW_DONE,
                                range.format.boolProperty(ScenarioBlockStyle::PropertyIsDone) ? "true" : "false");
                            //
                            // ... комментарии
                            //
                            const QStringList comments = range.format.property(ScenarioBlockStyle::PropertyComments).toStringList();
                            const QStringList authors = range.format.property(ScenarioBlockStyle::PropertyCommentsAuthors).toStringList();
                            const QStringList dates = range.format.property(ScenarioBlockStyle::PropertyCommentsDates).toStringList();
                            for (int commentIndex = 0; commentIndex < comments.size(); ++commentIndex) {
                                writer.writeEmptyElement(NODE_REVIEW_COMMENT);
                                writer.writeAttribute(ATTRIBUTE_REVIEW_COMMENT, TextEditHelper::toHtmlEscaped(comments.at(commentIndex)));
                                writer.writeAttribute(ATTRIBUTE_REVIEW_AUTHOR, authors.at(commentIndex));
                                writer.writeAttribute(ATTRIBUTE_REVIEW_DATE, dates.at(commentIndex));
                            }
                            //
                            writer.writeEndElement();
                        }
                    }
                    writer.writeEndElement();
                }

                //
                // Закрываем текущий элемент
                //
                writer.writeEndElement();
            }

            //
            // Снимем выделение
            //
            cursor.clearSelection();
        }

    } while (cursor.position() < _endPosition
             && !cursor.atEnd());

    //
    // Закроем открытые папки
    //
    while (openedFolders > 0) {
        writer.writeStartElement("folder_footer");
        writer.writeCDATA(QObject::tr("END OF FOLDER", "ScenarioXml"));
        writer.writeEndElement();
        --openedFolders;
    }

    //
    // Добавим корневой элемент
    //
    writer.writeEndElement(); // scenario
    writer.writeEndDocument();

    return resultXml;
}

QString ScenarioXml::scenarioToXml(ScenarioModelItem* _fromItem, ScenarioModelItem* _toItem)
{
    //
    // Определить интервал текста из которого нужно создать xml-представление
    //
    // ... начало
    int startPosition = _fromItem->position();
    // ... конец
    int endPosition = _fromItem->endPosition();

    int toItemEndPosition = _toItem->endPosition();
    if (endPosition < toItemEndPosition) {
        endPosition = toItemEndPosition;
    }

    //
    // Сформировать xml-строку
    //
    return scenarioToXml(startPosition, endPosition);
}

void ScenarioXml::xmlToScenario(int _position, const QString& _xml)
{
    QXmlStreamReader reader(_xml);
    if (reader.readNextStartElement()
        && reader.name().toString() == NODE_SCENARIO) {
        const QString version = reader.attributes().value(ATTRIBUTE_VERSION).toString();
        if (version.isEmpty()) {
            xmlToScenarioV0(_position, _xml);
        } else if (version == "1.0") {
            xmlToScenarioV1(_position, _xml);
        }
    }
}

int ScenarioXml::xmlToScenario(ScenarioModelItem* _insertParent, ScenarioModelItem* _insertBefore, const QString& _xml, bool _removeLastMime)
{
    //
    // Определим позицию для вставки данных
    //
    int insertPosition = m_scenario->positionToInsertMime(_insertParent, _insertBefore);

    //
    // Если документ пуст
    // Если происходит копирование, или вставка из другого источника
    // Если пользователь не пытается вставить данные на своё же место
    //
    if (m_scenario->document()->isEmpty()
        || !_removeLastMime
        || (insertPosition != m_lastMimeFrom
            && insertPosition != m_lastMimeTo)) {
        //
        // Начинаем операцию вставки
        //
        QTextCursor cursor(m_scenario->document());
        cursor.beginEditBlock();

        //
        // Если необходимо удалить прошлое выделение
        //
        if (_removeLastMime) {
            bool needCorrectPosition = false;
            if (m_lastMimeFrom < insertPosition) {
                needCorrectPosition = true;
            }

            int removedSymbols = removeLastMime();
            if (needCorrectPosition) {
                insertPosition -= removedSymbols;
            }
        }

        //
        // Вставим пустой блок для нового элемента
        //
        m_scenario->document()->setCursorPosition(cursor, insertPosition);
        cursor.insertBlock();
        //
        // ... скорректируем позицию курсора
        //
        if (insertPosition != 0) {
            insertPosition = cursor.position();
        }
        //
        // ... перенесём данные оставшиеся в предыдущем блоке, в новое место
        //
        else {
            cursor.movePosition(QTextCursor::PreviousBlock);
            if (ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*> (cursor.block().userData())) {
                ScenarioTextBlockInfo* movedInfo = info->clone();
                cursor.block().setUserData(0);
                cursor.movePosition(QTextCursor::NextBlock);
                cursor.block().setUserData(movedInfo);
            }
        }

        //
        // Вставка данных
        //
        xmlToScenario(insertPosition, _xml);

        //
        // Завершаем операцию
        //
        cursor.endEditBlock();
    }

    return insertPosition;
}

int ScenarioXml::removeLastMime()
{
    int removedSymbols = 0;

    if (m_lastMimeFrom != m_lastMimeTo
        && m_lastMimeFrom < m_lastMimeTo) {
        const int documentCharactersCount = m_scenario->document()->characterCount();

        //
        // Расширим область чтобы не оставалось пустых строк
        //
        if (m_lastMimeTo != (documentCharactersCount - 1)){
            ++m_lastMimeTo;
        } else if (m_lastMimeFrom > 0) {
            --m_lastMimeFrom;
        }

        //
        // Проверяем, чтобы область не выходила за границы документа
        //
        if (m_lastMimeTo >= documentCharactersCount) {
            m_lastMimeTo = documentCharactersCount - 1;
        }

        QTextCursor cursor(m_scenario->document());
        cursor.setPosition(m_lastMimeFrom);
        cursor.setPosition(m_lastMimeTo, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();

        removedSymbols = m_lastMimeTo - m_lastMimeFrom;
    }

    m_lastMimeFrom = -1;
    m_lastMimeTo = -1;

    return removedSymbols;
}

void ScenarioXml::xmlToScenarioV0(int _position, const QString& _xml)
{
    //
    // Происходит ли обработка первого блока
    //
    bool firstBlockHandling = true;
    //
    // Необходимо ли изменить тип блока, в который вставляется текст
    //
    bool needChangeFirstBlockType = false;

    //
    // Начинаем операцию вставки
    //
    QTextCursor cursor(m_scenario->document());
    cursor.setPosition(_position);
    cursor.beginEditBlock();

    //
    // Если вставка в пустой блок, то изменим его тип
    //
    if (cursor.block().text().simplified().isEmpty()) {
        needChangeFirstBlockType = true;
    }

    //
    // Последний использемый тип блока при обработке загружаемого текста
    //
    ScenarioBlockStyle::Type lastTokenType = ScenarioBlockStyle::Undefined;

    QXmlStreamReader reader(_xml);
    while (!reader.atEnd()) {
        //
        // Даём возможность выполниться графическим операциям
        //
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        switch (reader.readNext()) {
            case QXmlStreamReader::StartElement: {
                //
                // Определить тип текущего блока
                //
                ScenarioBlockStyle::Type tokenType = ScenarioBlockStyle::Undefined;
                QString tokenName = reader.name().toString();
                tokenType = ScenarioBlockStyle::typeForName(tokenName);

                //
                // Если определён тип блока, то обработать его
                //
                if (tokenType != ScenarioBlockStyle::Undefined) {
                    ScenarioBlockStyle currentStyle = ScenarioTemplateFacade::getTemplate().blockStyle(tokenType);

                    if (!firstBlockHandling) {
                        cursor.insertBlock();
                    }

                    //
                    // Если нужно добавим заголовок стиля
                    //
                    if (currentStyle.hasHeader()) {
                        ScenarioBlockStyle headerStyle = ScenarioTemplateFacade::getTemplate().blockStyle(currentStyle.headerType());
                        cursor.setBlockFormat(headerStyle.blockFormat());
                        cursor.setBlockCharFormat(headerStyle.charFormat());
                        cursor.setCharFormat(headerStyle.charFormat());
                        cursor.insertText(currentStyle.header());
                        cursor.insertBlock();
                    }

                    //
                    // Если необходимо сменить тип блока
                    //
                    if ((firstBlockHandling && needChangeFirstBlockType)
                        || !firstBlockHandling) {

                        //
                        // Установим стиль блока
                        //
                        cursor.setBlockFormat(currentStyle.blockFormat());
                        cursor.setBlockCharFormat(currentStyle.charFormat());
                        cursor.setCharFormat(currentStyle.charFormat());
                    }

                    //
                    // Корректируем информацию о шаге
                    //
                    if (firstBlockHandling) {
                        firstBlockHandling = false;
                    }
                }

                //
                // Если необходимо, загрузить информацию о сцене
                //
                if (tokenType == ScenarioBlockStyle::SceneHeading
                    || tokenType == ScenarioBlockStyle::FolderHeader) {
                    QString synopsis = reader.attributes().value("synopsis").toString();
                    ScenarioTextBlockInfo* info = new ScenarioTextBlockInfo;
                    QTextDocument doc;
                    doc.setHtml(synopsis);
                    info->setDescription(doc.toPlainText());
                    cursor.block().setUserData(info);
                }

                //
                // Обновим последний использовавшийся тип блока
                //
                lastTokenType = tokenType;

                break;
            }

            case QXmlStreamReader::Characters: {
                if (!reader.isWhitespace()) {
                    QString textToInsert = reader.text().toString();

                    //
                    // Если необходимо так же вставляем префикс и постфикс стиля
                    //
                    ScenarioBlockStyle currentStyle = ScenarioTemplateFacade::getTemplate().blockStyle(lastTokenType);
                    if (!currentStyle.prefix().isEmpty()
                        && !textToInsert.startsWith(currentStyle.prefix())) {
                        textToInsert.prepend(currentStyle.prefix());
                    }
                    if (!currentStyle.postfix().isEmpty()
                        && !textToInsert.endsWith(currentStyle.postfix())) {
                        textToInsert.append(currentStyle.postfix());
                    }

                    //
                    // Пишем сам текст
                    //
                    cursor.insertText(textToInsert);
                }
                break;
            }

            default: {
                break;
            }
        }
    }

    //
    // Завершаем операцию
    //
    cursor.endEditBlock();
}

void ScenarioXml::xmlToScenarioV1(int _position, const QString& _xml)
{
    //
    // Происходит ли обработка первого блока
    //
    bool firstBlockHandling = true;
    //
    // Необходимо ли изменить тип блока, в который вставляется текст
    //
    bool needChangeFirstBlockType = false;

    //
    // Начинаем операцию вставки
    //
    QTextCursor cursor(m_scenario->document());
    cursor.setPosition(_position);
    cursor.beginEditBlock();

    //
    // Если вставка в пустой блок, то изменим его тип
    //
    if (cursor.block().text().simplified().isEmpty()) {
        needChangeFirstBlockType = true;
    }

    //
    // Собственно считываем данные
    //
    QXmlStreamReader reader(_xml);
    while (!reader.atEnd()) {
        //
        // Даём возможность выполниться графическим операциям
        //
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        switch (reader.readNext()) {
            case QXmlStreamReader::StartElement: {
                //
                // Определить тип текущего блока
                //
                ScenarioBlockStyle::Type tokenType = ScenarioBlockStyle::Undefined;
                QString tokenName = reader.name().toString();
                tokenType = ScenarioBlockStyle::typeForName(tokenName);

                //
                // Если определён тип блока, то обработать его
                //
                if (tokenType != ScenarioBlockStyle::Undefined) {
                    ScenarioBlockStyle currentStyle = ScenarioTemplateFacade::getTemplate().blockStyle(tokenType);

                    if (firstBlockHandling) {
                        cursor.block().setVisible(true);
                    } else {
                        cursor.insertBlock();
                    }

                    //
                    // Если необходимо сменить тип блока
                    //
                    if ((firstBlockHandling && needChangeFirstBlockType)
                        || !firstBlockHandling) {

                        //
                        // Установим стиль блока
                        //
                        cursor.setBlockFormat(currentStyle.blockFormat());
                        cursor.setBlockCharFormat(currentStyle.charFormat());
                        cursor.setCharFormat(currentStyle.charFormat());
                    }

                    //
                    // Корректируем информацию о шаге
                    //
                    if (firstBlockHandling) {
                        firstBlockHandling = false;
                    }

                    //
                    // Если необходимо, загрузить информацию о сцене
                    //
                    if (tokenType == ScenarioBlockStyle::SceneHeading
                        || tokenType == ScenarioBlockStyle::FolderHeader) {
                        ScenarioTextBlockInfo* info = new ScenarioTextBlockInfo;
                        if (reader.attributes().hasAttribute(ATTRIBUTE_UUID)) {
                            const QString uuid = reader.attributes().value(ATTRIBUTE_UUID).toString();
                            if (!isScenarioHaveUuid(uuid)) {
                                info->setUuid(uuid);
                            }
                        }
                        if (reader.attributes().hasAttribute(ATTRIBUTE_COLOR)) {
                            info->setColors(reader.attributes().value(ATTRIBUTE_COLOR).toString());
                        }
                        if (reader.attributes().hasAttribute(ATTRIBUTE_TITLE)) {
                            info->setTitle(TextEditHelper::fromHtmlEscaped(reader.attributes().value(ATTRIBUTE_TITLE).toString()));
                        }
                        cursor.block().setUserData(info);
                    }

                    //
                    // Скрываем блоки, которых не должно быть видно в текщем режиме сценария
                    //
                    if (!m_scenario->document()->visibleBlocksTypes().contains(tokenType)) {
                        cursor.block().setVisible(false);
                    }
                }
                //
                // Обработка остальных тэгов
                //
                else {
                    //
                    // Редакторские заметки
                    //
                    if (tokenName == NODE_REVIEW) {
                        const int start = reader.attributes().value(ATTRIBUTE_REVIEW_FROM).toInt();
                        const int length = reader.attributes().value(ATTRIBUTE_REVIEW_LENGTH).toInt();
                        const bool highlight = reader.attributes().value(ATTRIBUTE_REVIEW_IS_HIGHLIGHT).toString() == "true";
                        const bool done = reader.attributes().value(ATTRIBUTE_REVIEW_DONE).toString() == "true";
                        const QColor foreground(reader.attributes().value(ATTRIBUTE_REVIEW_COLOR).toString());
                        const QColor background(reader.attributes().value(ATTRIBUTE_REVIEW_BGCOLOR).toString());
                        //
                        // ... считываем комментарии
                        //
                        QStringList comments, authors, dates;
                        while (reader.readNextStartElement()) {
                            if (reader.name() == NODE_REVIEW_COMMENT) {
                                comments << TextEditHelper::fromHtmlEscaped(reader.attributes().value(ATTRIBUTE_REVIEW_COMMENT).toString());
                                authors << reader.attributes().value(ATTRIBUTE_REVIEW_AUTHOR).toString();
                                dates << reader.attributes().value(ATTRIBUTE_REVIEW_DATE).toString();

                                reader.skipCurrentElement();
                            }
                        }


                        //
                        // Собираем формат редакторской заметки
                        //
                        QTextCharFormat reviewFormat;
                        reviewFormat.setProperty(ScenarioBlockStyle::PropertyIsReviewMark, true);
                        if (foreground.isValid()) {
                            reviewFormat.setForeground(foreground);
                        }
                        if (background.isValid()) {
                            reviewFormat.setBackground(background);
                        }
                        reviewFormat.setProperty(ScenarioBlockStyle::PropertyIsHighlight, highlight);
                        reviewFormat.setProperty(ScenarioBlockStyle::PropertyIsDone, done);
                        reviewFormat.setProperty(ScenarioBlockStyle::PropertyComments, comments);
                        reviewFormat.setProperty(ScenarioBlockStyle::PropertyCommentsAuthors, authors);
                        reviewFormat.setProperty(ScenarioBlockStyle::PropertyCommentsDates, dates);


                        //
                        // Вставляем в документ
                        //
                        QTextCursor reviewCursor = cursor;
                        int startDelta = 0;
                        if (reviewCursor.block().position() < _position
                            && (reviewCursor.block().position() + reviewCursor.block().length()) > _position) {
                            startDelta = _position - reviewCursor.block().position();
                        }
                        reviewCursor.setPosition(reviewCursor.block().position() + start + startDelta);
                        reviewCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, length);
                        reviewCursor.mergeCharFormat(reviewFormat);
                    }
                }

                break;
            }

            case QXmlStreamReader::Characters: {
                if (!reader.isWhitespace()) {
                    QString textToInsert = TextEditHelper::fromHtmlEscaped(reader.text().toString());

                    //
                    // Пишем сам текст
                    //
                    cursor.insertText(textToInsert);
                }
                break;
            }

            default: {
                break;
            }
        }
    }

    //
    // Завершаем операцию
    //
    cursor.endEditBlock();
}

bool ScenarioXml::isScenarioHaveUuid(const QString& _uuid) const
{
    auto currentBlock = m_scenario->document()->begin();
    while (currentBlock.isValid()) {
        const ScenarioBlockStyle::Type currentBlockType = ScenarioBlockStyle::forBlock(currentBlock);
        if (currentBlockType == ScenarioBlockStyle::SceneHeading
            || currentBlockType == ScenarioBlockStyle::FolderHeader) {
            if (ScenarioTextBlockInfo* info = static_cast<ScenarioTextBlockInfo*>(currentBlock.userData())) {
                if (info->uuid() == _uuid) {
                    return true;
                }
            }
        }
        currentBlock = currentBlock.next();
    }
    return false;
}
