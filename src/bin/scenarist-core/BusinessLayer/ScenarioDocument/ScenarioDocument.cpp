#include "ScenarioDocument.h"

#include "ScenarioXml.h"
#include "ScenarioTextDocument.h"
#include "ScenarioModel.h"
#include "ScenarioModelItem.h"
#include "ScenarioTemplate.h"
#include "ScenarioTextBlockInfo.h"
#include "ScenarioTextBlockParsers.h"

#include <BusinessLayer/Chronometry/ChronometerFacade.h>
#include <BusinessLayer/Counters/CountersFacade.h>

#include <Domain/Scenario.h>

#include <QCryptographicHash>
#include <QRegularExpression>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QTimer>

using namespace BusinessLogic;


QString ScenarioDocument::MIME_TYPE = "application/x-scenarist/scenario";

ScenarioDocument::ScenarioDocument(QObject* _parent) :
    QObject(_parent),
    m_scenario(0),
    m_xmlHandler(new ScenarioXml(this)),
    m_document(new ScenarioTextDocument(this, m_xmlHandler)),
    m_model(new ScenarioModel(this, m_xmlHandler)),
    m_inSceneDescriptionUpdate(false),
    m_lastChangeStartPosition(0)
{
    initConnections();
}

ScenarioTextDocument* ScenarioDocument::document() const
{
    return m_document;
}

ScenarioModel* ScenarioDocument::model() const
{
    return m_model;
}

int ScenarioDocument::scenesCount() const
{
    return m_model->scenesCount();
}

qreal ScenarioDocument::durationAtPosition(int _position) const
{
    qreal duration = 0;

    if (!m_modelItems.isEmpty()) {
        //
        // Определим сцену, в которой находится курсор
        //
        QMap<int, ScenarioModelItem*>::const_iterator iter = m_modelItems.lowerBound(_position);
        if (iter == m_modelItems.end()
            || (iter.key() > _position
                && iter != m_modelItems.begin())) {
            --iter;
        }

        //
        // Запомним позицию начала сцены
        //
        int startPositionInLastScene = iter.key();

        //
        // Посчитаем хронометраж всех предыдущих сцен
        //
        if (iter.value()->type() == ScenarioModelItem::Scene) {
            iter.value()->duration();
        }
        while (iter != m_modelItems.begin()) {
            --iter;
            if (iter.value()->type() == ScenarioModelItem::Scene) {
                duration += iter.value()->duration();
            }
        }

        //
        // Добавим к суммарному хрономертажу хронометраж от начала сцены
        //
        duration += ChronometerFacade::calculate(m_document, startPositionInLastScene, _position);
    }

    return duration;
}

int ScenarioDocument::fullDuration() const
{
    return m_model->duration();
}

QString ScenarioDocument::countersInfo() const
{
    const int pageCount = m_document->pageCount();
    return BusinessLogic::CountersFacade::countersInfo(pageCount, m_model->counter());
}

QModelIndex ScenarioDocument::itemIndexAtPosition(int _position) const
{
    ScenarioModelItem* item = itemForPosition(_position, true);
    return m_model->indexForItem(item);
}

int ScenarioDocument::itemStartPosition(const QModelIndex& _index) const
{
    ScenarioModelItem* item = m_model->itemForIndex(_index);
    return item->position();
}

int ScenarioDocument::itemMiddlePosition(const QModelIndex& _index) const
{
    ScenarioModelItem* item = m_model->itemForIndex(_index);
    if (item->type() != ScenarioModelItem::Folder) {
        return item->endPosition();
    }

    return item->endPosition() - item->footer().length() - 1;
}

int ScenarioDocument::itemEndPosition(const QModelIndex& _index) const
{
    ScenarioModelItem* item = m_model->itemForIndex(_index);
    return item->endPosition();
}

QString ScenarioDocument::itemHeaderAtPosition(int _position) const
{
    QString header;
    if (ScenarioModelItem* item = itemForPosition(_position, true)) {
        header = item->header();
    }
    return header;
}

QString ScenarioDocument::itemUuid(ScenarioModelItem* _item) const
{
    QTextCursor cursor(m_document);
    cursor.setPosition(_item->position());
    QTextBlockUserData* textBlockData = cursor.block().userData();
    ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
    if (info == 0) {
        info = new ScenarioTextBlockInfo;
    }
    cursor.block().setUserData(info);

    return info->uuid();
}

QString ScenarioDocument::itemColors(ScenarioModelItem* _item) const
{
    QTextCursor cursor(m_document);
    cursor.setPosition(_item->position());

    QString colors;
    QTextBlockUserData* textBlockData = cursor.block().userData();
    if (ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData)) {
        colors = info->colors();
    }
    return colors;
}

void ScenarioDocument::setItemColorsAtPosition(int _position, const QString& _colors)
{
    if (ScenarioModelItem* item = itemForPosition(_position, true)) {
        //
        // Установить цвет в элемент
        //
        item->setColors(_colors);
        m_model->updateItem(item);

        //
        // Установить цвет в документ
        //
        QTextCursor cursor(m_document);
        cursor.setPosition(item->position());

        QTextBlockUserData* textBlockData = cursor.block().userData();
        ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
        if (info == 0) {
            info = new ScenarioTextBlockInfo;
        }
        info->setColors(_colors);
        cursor.block().setUserData(info);

        ScenarioTextDocument::updateBlockRevision(cursor);
        aboutContentsChange(cursor.block().position(), 0, 0);
    }
}

QString ScenarioDocument::itemTitleAtPosition(int _position) const
{
    QString title;
    if (ScenarioModelItem* item = itemForPosition(_position, true)) {
        title = itemTitle(item);
    }
    return title;
}

QString ScenarioDocument::itemTitle(ScenarioModelItem* _item) const
{
    QTextCursor cursor(m_document);
    cursor.setPosition(_item->position());

    QString title;
    QTextBlockUserData* textBlockData = cursor.block().userData();
    if (ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData)) {
        title = info->title();
    }
    return title;
}

void ScenarioDocument::setItemTitleAtPosition(int _position, const QString& _title)
{
    if (ScenarioModelItem* item = itemForPosition(_position, true)) {
        //
        // Установить название в элемент
        //
        item->setTitle(_title);
        m_model->updateItem(item);

        //
        // Установить название в документ
        //
        QTextCursor cursor(m_document);
        cursor.setPosition(item->position());

        QTextBlockUserData* textBlockData = cursor.block().userData();
        ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
        if (info == 0) {
            info = new ScenarioTextBlockInfo;
        }
        info->setTitle(_title);
        cursor.block().setUserData(info);

        ScenarioTextDocument::updateBlockRevision(cursor);
        aboutContentsChange(cursor.block().position(), 0, 0);
    }
}

QString ScenarioDocument::itemDescriptionAtPosition(int _position) const
{
    QString description;
    if (ScenarioModelItem* item = itemForPosition(_position, true)) {
        description = itemDescription(item);
    }
    return description;
}

QString ScenarioDocument::itemDescription(ScenarioModelItem* _item) const
{
    QTextCursor cursor(m_document);
    cursor.setPosition(_item->position());

    QString description;
    QTextBlockUserData* textBlockData = cursor.block().userData();
    if (ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData)) {
        description = info->description();
    }
    return description;
}

void ScenarioDocument::setItemDescriptionAtPosition(int _position, const QString& _description)
{
    if (!m_inSceneDescriptionUpdate) {
        m_inSceneDescriptionUpdate = true;

        if (ScenarioModelItem* item = itemForPosition(_position, true)) {
            //
            // Установить описание в элемент
            //
            item->setDescription(!_description.isEmpty() ? _description : QString::null);
            m_model->updateItem(item);

            //
            // Установить описание в документ
            //
            QTextCursor cursor(m_document);
            cursor.setPosition(item->position());

            QTextBlockUserData* textBlockData = cursor.block().userData();
            ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
            if (info == 0) {
                info = new ScenarioTextBlockInfo;
            }
            info->setDescription(_description);
            cursor.block().setUserData(info);

            //
            // Обновить описание внутри текста
            //
            cursor.beginEditBlock();
            ScenarioBlockStyle descriptionBlockStyle = ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::SceneDescription);
            //
            // ... если это не последний блок в документе проверяем следующие блоки
            //
            if (cursor.movePosition(QTextCursor::NextBlock)) {
                int descriptionStartPosition = cursor.position() - 1;
                ScenarioBlockStyle::Type currentBlockType = ScenarioBlockStyle::forBlock(cursor.block());

                //
                // ... если после заголовка идёт блок со списком персонажей, переносим курсор за этот блок
                //
                if (currentBlockType == ScenarioBlockStyle::SceneCharacters) {
                    cursor.movePosition(QTextCursor::EndOfBlock);
                    descriptionStartPosition = cursor.position();

                    if (cursor.movePosition(QTextCursor::NextBlock)) {
                        currentBlockType = ScenarioBlockStyle::forBlock(cursor.block());
                    }
                }

                //
                // ... затираем старое описание до тех пор пока не дойдём до новой сцены или конца документа
                //
                while (!cursor.atEnd()
                       && currentBlockType != ScenarioBlockStyle::SceneHeading
                       && currentBlockType != ScenarioBlockStyle::FolderHeader
                       && currentBlockType != ScenarioBlockStyle::FolderFooter) {
                    //
                    // ... обнаружили описание сцены - удалим его
                    //
                    if (currentBlockType == ScenarioBlockStyle::SceneDescription) {
                        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                        cursor.removeSelectedText();
                        cursor.deleteChar();
                    }
                    //
                    // ... нет - идём к следующему блоку
                    //
                    else {
                        cursor.movePosition(QTextCursor::EndOfBlock);
                        cursor.movePosition(QTextCursor::NextBlock);
                    }

                    currentBlockType = ScenarioBlockStyle::forBlock(cursor.block());
                }

                //
                // ... после того, как стёрли все описания действия, возвращаем курсор в позицию,
                //     куда будем вставлять описание действия
                //
                cursor.setPosition(descriptionStartPosition);
            }
            //
            // ... если это последний блок в документе, просто переходим в конец
            //
            else {
                cursor.movePosition(QTextCursor::EndOfBlock);
            }
            cursor.insertBlock(descriptionBlockStyle.blockFormat(), descriptionBlockStyle.charFormat());
            //
            // ... вставляем новый
            //
            if (_description.isEmpty()) {
                cursor.deletePreviousChar();
            } else {
                foreach (const QString& descriptionLine, _description.split("\n")) {
                    if (!cursor.block().text().isEmpty()) {
                        cursor.insertBlock(descriptionBlockStyle.blockFormat(), descriptionBlockStyle.charFormat());
                    }
                    cursor.block().setVisible(m_document->outlineMode());
                    cursor.insertText(descriptionLine);
                }
            }
            cursor.setPosition(_position);
            cursor.endEditBlock();
        }

        m_inSceneDescriptionUpdate = false;
    }
}

void ScenarioDocument::load(Domain::Scenario* _scenario)
{
    m_scenario = _scenario;

    if (m_scenario != 0) {
        load(m_scenario->text());
    }
}

Domain::Scenario* ScenarioDocument::scenario() const
{
    return m_scenario;
}

void ScenarioDocument::setScenario(Domain::Scenario* _scenario)
{
    if (m_scenario != _scenario) {
        m_scenario = _scenario;
    }
}

QString ScenarioDocument::save() const
{
    return m_document->scenarioXml();
}

void ScenarioDocument::clear()
{
    QTextCursor cursor(m_document);
    cursor.select(QTextCursor::Document);
    cursor.removeSelectedText();
}

void ScenarioDocument::refresh()
{
    aboutContentsChange(0, 0, m_document->characterCount());
}

QStringList ScenarioDocument::findCharacters() const
{
    //
    // Найти персонажей во всём тексте
    //
    QSet<QString> characters;
    QTextCursor cursor(document());
    while (!cursor.atEnd()) {
        cursor.movePosition(QTextCursor::EndOfBlock);
        if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::Character) {
            cursor.select(QTextCursor::BlockUnderCursor);
            QString character =
                    BusinessLogic::CharacterParser::name(cursor.selectedText().toUpper().trimmed());
            characters.insert(character);
        } else if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneCharacters) {
            cursor.select(QTextCursor::BlockUnderCursor);
            QStringList blockCharacters = BusinessLogic::SceneCharactersParser::characters(cursor.selectedText());
            foreach (const QString& characterName, blockCharacters) {
                QString character = BusinessLogic::CharacterParser::name(characterName.toUpper().trimmed());
                characters.insert(character);
            }
        }
        cursor.movePosition(QTextCursor::NextBlock);
    }

    return characters.toList();
}

QStringList ScenarioDocument::findLocations() const
{
    //
    // Найти локации во всём тексте
    //
    QSet<QString> locations;
    QTextCursor cursor(document());
    while (!cursor.atEnd()) {
        cursor.movePosition(QTextCursor::EndOfBlock);
        if (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneHeading) {
            QString location =
                    BusinessLogic::SceneHeadingParser::locationName(cursor.block().text().toUpper().trimmed());
            locations.insert(location);
        }
        cursor.movePosition(QTextCursor::NextBlock);
    }

    return locations.toList();
}

int ScenarioDocument::positionToInsertMime(ScenarioModelItem* _insertParent, ScenarioModelItem* _insertBefore) const
{
    int insertPosition = 0;

    //
    // Если необходимо вставить перед заданным элементом
    //
    if (_insertBefore != 0) {
        int insertBeforeItemStartPos = m_modelItems.key(_insertBefore);

        //
        // Шаг назад
        //
        insertPosition = insertBeforeItemStartPos - 1;
    }
    //
    // Если необходимо вставить в конец родительского элемента
    //
    else {
        if (_insertParent->hasChildren()) {
            ScenarioModelItem* lastChild = _insertParent->childAt(_insertParent->childCount() - 1);
            insertPosition = lastChild->endPosition();
        } else {
            int parentStartPosition = _insertParent->position();
            QTextCursor cursor(m_document);
            cursor.setPosition(parentStartPosition);
            //
            // ... переходим к концу блока заголовка
            //
            cursor.movePosition(QTextCursor::NextBlock);
            //
            // ... переходим к концу описания элемента
            //
            while (ScenarioBlockStyle::forBlock(cursor.block()) == ScenarioBlockStyle::SceneDescription) {
                cursor.movePosition(QTextCursor::NextBlock);
            }
            //
            // ... возвращаемся в конец последнего блока
            //
            cursor.movePosition(QTextCursor::PreviousCharacter);

            insertPosition = cursor.position();
        }
    }

    //
    // Если необходимо вставить в начало документа
    //
    if (insertPosition < 0) {
        insertPosition = 0;
    }

    return insertPosition;
}

void ScenarioDocument::aboutContentsChange(int _position, int _charsRemoved, int _charsAdded)
{
    //
    // Сохраняем изменённый xml и его хэш
    //
    m_document->updateScenarioXml();

    //
    // Прерываем ситуацию с ложным срабатыванием изменения документа
    //
    const QByteArray currentTextMd5Hash = m_document->scenarioXmlHash();
    if (_charsRemoved == _charsAdded) {
        //
        // ... на самом ли деле текст изменился?
        //
        if (currentTextMd5Hash == m_lastTextMd5Hash) {
            //
            // ... даже если текст не изменился, обновляем номера сцен, т.к. могла измениться
            //     модель документа, например добавился комментарий от редактора
            //
            updateDocumentScenesNumbers();
            return;
        }
    }

    //
    // Сохранить md5 хэш текста документа
    //
    m_lastTextMd5Hash = currentTextMd5Hash;

    //
    // Сохраняем позицию начала правок для последующей корректировки
    //
    m_lastChangeStartPosition = _position;

    //
    // Если были удалены данные
    //
    if (_charsRemoved > 0) {
        int position = _position;

        //
        // Если бэкспейс или делит, то нужно сместить все элементы, включая тот, после блока в
        // котором он нажат
        //
        if (_charsAdded == 0 && _charsRemoved == 1 && _position > 0) {
            if (m_document->characterAt(_position) == QChar(QChar::ParagraphSeparator)) {
                ++position;
            }
        }

        //
        // Удаляем элементы начиная с того, который находится под курсором, если курсор в начале
        // строки, или со следующего за курсором, если курсор не в начале строки
        //
        QMap<int, ScenarioModelItem*>::iterator iter = m_modelItems.lowerBound(position);
        const int charsAddedDelta = _charsAdded - _charsRemoved;
        const int charsRemovedDelta = _charsRemoved - _charsAdded;
        while (iter != m_modelItems.end()
               && iter.key() >= position
               && iter.key() < (position + _charsRemoved)) {
            //
            // Элемент для удаления
            //
            ScenarioModelItem* itemToDelete = iter.value();

            if (itemToDelete != 0) {
                //
                // Расширяем диапозон последующего построения дерева, для включения в него всех
                // кто был удалён тут по причине не самого оптимального алгоритма
                //
                if (itemToDelete->hasChildren()) {
                    const int charsModified = itemToDelete->endPosition() - position;
                    if (_charsAdded < charsModified + charsAddedDelta) {
                        _charsAdded = charsModified + charsAddedDelta;
                    }
                    if (_charsRemoved < charsModified - charsRemovedDelta) {
                        _charsRemoved = charsModified - charsRemovedDelta;
                    }
                }

                //
                // Удалим элемент из модели
                //
                m_model->removeItem(iter.value());
            }

            //
            // Удалим элемент из кэша
            //
            iter = m_modelItems.erase(iter);
        }
    }

    //
    // Скорректируем позицию
    //
    if (_charsRemoved > _charsAdded
        && _position > 0) {
        ++_position;
    }

    //
    // Сместить позиции всех сохранённых в кэше элементов после текущего на _charsRemoved и _charsAdded
    //
    // ... исключаем ситуацию повторного сигнала загрузки документа
    //
    if (_charsAdded != m_document->characterCount()) {
        int position = _position;

        //
        // Если нажат энтер, то нужно сместить все элементы, включая тот, перед блоком
        // которого он нажат, а если не энтер, то все, после нажатого символа
        //
        if (_charsAdded > 0 && _charsRemoved == 0) {
            if (_position > 0
                && m_document->characterAt(_position) == QChar(QChar::ParagraphSeparator)) {
                --position;
            } else {
                ++position;
            }
        }

        QMutableMapIterator<int, ScenarioModelItem*> removeIter(m_modelItems);
        QMap<int, ScenarioModelItem*> updatedItems;
        //
        // Изымаем элементы из хэша и формируем обновлённый список
        //
        while (removeIter.hasNext()) {
            removeIter.next();
            if (removeIter.key() >= position) {
                ScenarioModelItem* item = removeIter.value();
                item->setPosition(removeIter.key() - _charsRemoved + _charsAdded);
                updatedItems.insert(item->position(), item);
                removeIter.remove();
            }
        }
        //
        // Переносим элементы из обновлённого списка в хэш
        //
        QMapIterator<int, ScenarioModelItem*> updateIter(updatedItems);
        while (updateIter.hasNext()) {
            updateIter.next();
            m_modelItems.insert(updateIter.key(), updateIter.value());
        }
    }

    //
    // Если были изменены данные
    //
    if (_charsAdded > 0 || _charsRemoved > 0) {
        //
        // получить первый блок и обновить/создать его
        // идти по документу, до конца вставленных символов и добавлять блоки
        //
        QMap<int, ScenarioModelItem*>::iterator iter = m_modelItems.lowerBound(_position);
        if (iter != m_modelItems.begin()
            && iter.key() > _position) {
            --iter;
        }

        //
        // Обновляем структуру
        //

        ScenarioModelItem* currentItem = 0;
        int currentItemStartPos = 0;

        //
        // Если в документе нет ни одного элемента, создадим первый
        //
        if (iter == m_modelItems.end()) {
            currentItem = itemForPosition(0);
            m_model->addItem(currentItem);
            m_modelItems.insert(0, currentItem);
        }
        //
        // Или если вставляется новый элемент в начале текста
        //
        else if (_position == 0 && iter == m_modelItems.begin() && iter.key() > 0) {
            currentItem = itemForPosition(0);
            m_model->prependItem(currentItem);
            m_modelItems.insert(0, currentItem);
        }
        //
        // В противном случае получим необходимый к обновлению элемент
        //
        else {
            currentItem = iter.value();
            currentItemStartPos = iter.key();
        }

        //
        // Текущий родитель
        //
        ScenarioModelItem* currentParent = 0;

        QTextCursor cursor(m_document);
        if (currentItemStartPos > 0) {
            cursor.setPosition(currentItemStartPos);
        }

        do {
            //
            // Тип текущего элемента
            //
            const ScenarioBlockStyle::Type itemType = ScenarioBlockStyle::forBlock(cursor.block());

            //
            // Идём до конца элемента
            //
            ScenarioBlockStyle::Type currentType = ScenarioBlockStyle::Undefined;
            do {
                cursor.movePosition(QTextCursor::NextBlock);
                cursor.movePosition(QTextCursor::EndOfBlock);
                currentType = ScenarioBlockStyle::forBlock(cursor.block());
            } while (!cursor.atEnd()
                     && currentType != ScenarioBlockStyle::SceneHeading
                     && currentType != ScenarioBlockStyle::FolderHeader
                     && currentType != ScenarioBlockStyle::FolderFooter);

            //
            // Тип следующего за элементом блока
            //
            ScenarioBlockStyle::Type nextBlockType = ScenarioBlockStyle::Undefined;
            //
            // Если не конец документа, то получить стиль следующего за элементом блока
            // и оступить на один блок назад в виду того, что мы зашли на следующий элемент
            //
            if (!cursor.atEnd()
                || currentType == ScenarioBlockStyle::SceneHeading
                || currentType == ScenarioBlockStyle::FolderHeader
                || currentType == ScenarioBlockStyle::FolderFooter) {
                nextBlockType = currentType;
                cursor.movePosition(QTextCursor::PreviousBlock);
                cursor.movePosition(QTextCursor::EndOfBlock);

                //
                // Если курсор вышел за начало текущего блока, то отменим предыдущее действие
                // это может случиться когда обрабатывается последний блок текста,
                // который является заголовочным
                //
                if (cursor.position() < currentItemStartPos) {
                    cursor.movePosition(QTextCursor::NextBlock);
                    cursor.movePosition(QTextCursor::EndOfBlock);
                }
            }

            int currentItemEndPos = cursor.position();
            //
            // ... если текущий элемент является группирующим, то нужно включить
            //     и все входящие в него группирующие элементы
            //
            if (itemType == ScenarioBlockStyle::FolderHeader) {
                int openedFolders = 1;
                QTextCursor endCursor = cursor;
                endCursor.movePosition(QTextCursor::NextBlock);
                while (!endCursor.atEnd()
                       && openedFolders != 0) {
                    endCursor.movePosition(QTextCursor::EndOfBlock);
                    switch (ScenarioBlockStyle::forBlock(endCursor.block())) {
                        case ScenarioBlockStyle::FolderHeader: {
                            ++openedFolders;
                            break;
                        }

                        case ScenarioBlockStyle::FolderFooter: {
                            --openedFolders;
                            break;
                        }

                        default: break;
                    }
                    endCursor.movePosition(QTextCursor::NextBlock);
                }

                currentItemEndPos = endCursor.position();
            }

            //
            // Сформируем элемент, если это не конец группы
            //
            {
                QTextCursor cursorForCheck(m_document);
                cursorForCheck.setPosition(currentItemStartPos);
                ScenarioBlockStyle::Type checkType = ScenarioBlockStyle::forBlock(cursorForCheck.block());
                if (checkType != ScenarioBlockStyle::FolderFooter) {
                    updateItem(currentItem, currentItemStartPos, currentItemEndPos);
                    m_model->updateItem(currentItem);
                }
            }

            //
            // Определим родителя если ещё не определён
            //
            if (currentParent == 0) {
                if (currentItem->type() == ScenarioModelItem::Scene
                    || currentItem->type() == ScenarioModelItem::Undefined) {
                    currentParent = currentItem->parent();
                } else {
                    currentParent = currentItem;
                }
            }

            cursor.movePosition(QTextCursor::NextBlock);
            //
            // Если не конец документа и всё ещё можно строить структуру
            //
            if (!cursor.atEnd()
                && cursor.position() < (_position + _charsAdded)) {
                //
                // Обновим позицию начала следующего элемента
                //
                currentItemStartPos = cursor.position();

                //
                // Действуем в зависимости от последующего за текущим элементом блока
                //
                switch (nextBlockType) {
                    case ScenarioBlockStyle::SceneHeading: {
                        //
                        // Создать новый элемент
                        //
                        ScenarioModelItem* newItem = itemForPosition(cursor.position());
                        //
                        // Вставить в группирующий элемент
                        //
                        if (currentItem == currentParent) {
                            m_model->addItem(newItem, currentParent);
                        }
                        //
                        // Вставить после текущего элемента
                        //
                        else {
                            m_model->insertItem(newItem, currentItem);
                        }
                        //
                        // Сделать новый элемент текущим
                        //
                        currentItem = newItem;

                        //
                        // Сохраним новый элемент в кэше
                        //
                        m_modelItems.insert(currentItemStartPos, currentItem);
                        break;
                    }

                    case ScenarioBlockStyle::FolderHeader: {
                        //
                        // Создать новый элемент
                        //
                        ScenarioModelItem* newItem = itemForPosition(cursor.position());
                        //
                        // Вставить в группирующий элемент
                        //
                        if (currentItem == currentParent) {
                            m_model->addItem(newItem, currentParent);
                        }
                        //
                        // Вставить после текущего элемента
                        //
                        else {
                            m_model->insertItem(newItem, currentItem);
                        }
                        //
                        // Сделать новый элемент текущим
                        //
                        currentItem = newItem;
                        //
                        // Сделать текущего родителя собой, т.к. последующие элементы должны вкладываться внутрь
                        //
                        currentParent = newItem;

                        //
                        // Сохраним новый элемент в кэше
                        //
                        m_modelItems.insert(currentItemStartPos, currentItem);
                        break;
                    }

                    case ScenarioBlockStyle::FolderFooter: {
                        //
                        // Делаем текущим родителем родителя группирующего элемента, чтобы последующие
                        // элементы уже не вкладывались, а создавались рядом
                        //
                        if (currentItem != currentParent) {
                            currentItem = currentItem->parent();
                        }
                        currentParent = currentItem->parent();
                        break;
                    }

                    default:
                        break;
                }
            }

        } while (!cursor.atEnd()
                 && cursor.position() < (_position + _charsAdded));
    }

    updateDocumentScenesNumbers();
}

void ScenarioDocument::initConnections()
{
    connect(m_document, &ScenarioTextDocument::contentsChange, this, &ScenarioDocument::aboutContentsChange);
}

void ScenarioDocument::removeConnections()
{
    disconnect(m_document, &ScenarioTextDocument::contentsChange, this, &ScenarioDocument::aboutContentsChange);
}

void ScenarioDocument::updateItem(ScenarioModelItem* _item, int _itemStartPos, int _itemEndPos)
{
    //
    // Получим данные элемента
    //
    QTextCursor cursor(m_document);
    cursor.setPosition(_itemStartPos);
    // ... тип
    ScenarioModelItem::Type itemType = ScenarioModelItem::Undefined;
    ScenarioBlockStyle::Type blockType = ScenarioBlockStyle::forBlock(cursor.block());
    if (blockType == ScenarioBlockStyle::SceneHeading) {
        itemType = ScenarioModelItem::Scene;
    } else if (blockType == ScenarioBlockStyle::FolderHeader
               || blockType == ScenarioBlockStyle::FolderFooter) {
        itemType = ScenarioModelItem::Folder;
    }
    // ... заголовок
    const QString itemHeader = cursor.block().text();
    // ... название
    const QString title = itemTitle(_item);
    // ... цвет
    const QString colors = itemColors(_item);
    // ... текст и описание
    QString itemText;
    QString description;
    // ... подвал
    QString footer;
    //
    bool isFirstDescriptionBlock = true; // первый блок описания сцены
    bool isFirstTextBlock = true; // первый блок текста сцены
    bool isNeedIncludeBlock = true; // нужно ли включать текущий блок
    int openedScenesGroups = 0; // кол-во открытых групп
    int openedFolders = 0; // кол-во открытых папок
    cursor.movePosition(QTextCursor::NextBlock);
    while (!cursor.atEnd()
           && cursor.position() < _itemEndPos) {
        cursor.movePosition(QTextCursor::EndOfBlock);

        ScenarioBlockStyle::Type blockType = ScenarioBlockStyle::forBlock(cursor.block());
        //
        // ... исключаем из текста заголовки и описание
        //
        switch (blockType) {
            //
            // Заголовки никуда не включаем
            //
            case ScenarioBlockStyle::SceneHeading: {
                isNeedIncludeBlock = false;
                break;
            }

            //
            // Не включаем тект папок
            //
            case ScenarioBlockStyle::FolderHeader: {
                ++openedFolders;
                isNeedIncludeBlock = false;
                break;
            }
            case ScenarioBlockStyle::FolderFooter: {
                if (openedScenesGroups == 0
                    && openedFolders == 0) {
                    footer = cursor.block().text();
                } else {
                    --openedFolders;
                    if (openedScenesGroups == 0
                        && openedFolders == 0) {
                        isNeedIncludeBlock = true;
                    }
                }
                break;
            }

            //
            // Описание сохраняем в описание
            //
            case ScenarioBlockStyle::SceneDescription: {
                if (isNeedIncludeBlock) {
                    if (!isFirstDescriptionBlock) {
                        description.append("\n");
                    } else {
                        description = "";
                        isFirstDescriptionBlock = false;
                    }
                    description.append(cursor.block().text());
                }
                break;
            }

            //
            // Весь остальной текст - текст сцены
            //
            default: {
                if (isNeedIncludeBlock) {
                    if (!isFirstTextBlock) {
                        itemText.append("\n");
                    } else {
                        itemText = "";
                        isFirstTextBlock = false;
                    }
                    ScenarioBlockStyle blockStyle = ScenarioTemplateFacade::getTemplate().blockStyle(blockType);
                    itemText +=
                            blockStyle.charFormat().fontCapitalization() == QFont::AllUppercase
                            ? cursor.block().text().toUpper()
                            : cursor.block().text();
                }
                break;
            }
        }

        cursor.movePosition(QTextCursor::NextBlock);
    }
    //
    // ... обновляем описание
    //
    cursor.setPosition(_itemStartPos);
    ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(cursor.block().userData());
    if (info == nullptr) {
        info = new ScenarioTextBlockInfo;
    }
    info->setDescription(description);
    cursor.block().setUserData(info);
    // ... длительность
    qreal itemDuration = 0;
    if (itemType == ScenarioModelItem::Scene) {
        itemDuration = ChronometerFacade::calculate(m_document, _itemStartPos, _itemEndPos);
    }
    // ... содержит ли примечания
    bool hasNote = false;
    cursor.setPosition(_itemStartPos);
    while (cursor.position() < _itemEndPos) {
        cursor.movePosition(QTextCursor::EndOfBlock);
        if (ScenarioBlockStyle::forBlock(cursor.block())
            == ScenarioBlockStyle::NoprintableText) {
            hasNote = true;
            break;
        }
        cursor.movePosition(QTextCursor::NextBlock);
    }
    // ... счётчик слов и символов
    Counter counter = CountersFacade::calculate(m_document, _itemStartPos, _itemEndPos);

    //
    // Обновим данные элемента
    //
    _item->setType(itemType);
    _item->setHeader(itemHeader);
    _item->setColors(colors);
    _item->setTitle(title);
    _item->setText(itemText);
    _item->setDescription(description);
    _item->setDuration(itemDuration);
    _item->setHasNote(hasNote);
    _item->setCounter(counter);
    _item->setFooter(footer);
}

ScenarioModelItem* ScenarioDocument::itemForPosition(int _position, bool _findNear) const
{
    ScenarioModelItem* item = m_modelItems.value(_position, 0);
    if (item == 0) {
        //
        // Если необходимо ищем ближайшего
        //
        if (_findNear) {
            QMap<int, ScenarioModelItem*>::const_iterator i = m_modelItems.lowerBound(_position);
            if (i != m_modelItems.begin()
                || i != m_modelItems.end()) {
                if (i != m_modelItems.begin()) {
                    --i;
                }
                item = i.value();
            } else {
                //
                // не найден, т.к. в модели нет элементов
                //
            }
        }
        //
        // В противном случае создаём новый элемент
        //
        else {
            item = new ScenarioModelItem(_position);
        }
    }
    //
    // Обновим идентификатор элемента
    //
    item->setUuid(itemUuid(item));

    return item;
}

void ScenarioDocument::updateDocumentScenesNumbers()
{
    m_model->updateSceneNumbers();

    //
    // Проходим документ и обновляем номера сцен
    //
    QTextBlock block = document()->begin();
    while (block.isValid()) {
        if (ScenarioBlockStyle::forBlock(block) == ScenarioBlockStyle::SceneHeading) {
            if (ScenarioModelItem* item = itemForPosition(block.position())) {
                //
                // Обновим данные документа
                //
                QTextBlockUserData* textBlockData = block.userData();
                ScenarioTextBlockInfo* info = dynamic_cast<ScenarioTextBlockInfo*>(textBlockData);
                if (info == 0) {
                    info = new ScenarioTextBlockInfo;
                }
                info->setSceneNumber(item->sceneNumber());
                block.setUserData(info);
            }
        }

        block = block.next();
    }
}

void ScenarioDocument::load(const QString& _scenario)
{
    //
    // Отключаем всё от документа
    //
    removeConnections();

    //
    // Очищаем модель и документ
    //
    {
        aboutContentsChange(0, m_document->characterCount(), 0);
        m_document->clear();
    }

    //
    // Загружаем сценарий
    //
    {
        m_document->load(_scenario);
        aboutContentsChange(0, 0, m_document->characterCount());
    }

    //
    // Подключаем необходимые сигналы
    //
    initConnections();
}
