#include "ScenarioTextDocument.h"

#include "ScenarioReviewModel.h"
#include "ScenarioXml.h"

#include <Domain/ScenarioChange.h>

#include <DataLayer/Database/DatabaseHelper.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/ScenarioChangeStorage.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/PasswordStorage.h>

#include <3rd_party/Widgets/QLightBoxWidget/qlightboxprogress.h>

#include <QApplication>
#include <QCryptographicHash>
#include <QDomDocument>
#include <QTextBlock>

//
// Для отладки работы с патчами
//
//#define PATCH_DEBUG
#ifdef PATCH_DEBUG
#include <QDebug>
#endif

using namespace BusinessLogic;
using DatabaseLayer::DatabaseHelper;

namespace {
    /**
     * @brief Доступный размер изменений в редакторе
     */
    const int MAX_UNDO_REDO_STACK_SIZE = 50;

    /**
     * @brief Получить хэш текста
     */
    static QByteArray textMd5Hash(const QString& _text) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(_text.toUtf8());
        return hash.result();
    }

    /**
     * @brief Сохранить изменение
     */
    static Domain::ScenarioChange* saveChange(const QString& _undoPatch, const QString& _redoPatch) {
        const QString username = DataStorageLayer::StorageFacade::username();
        return DataStorageLayer::StorageFacade::scenarioChangeStorage()->append(username, _undoPatch, _redoPatch);
    }

    const QString SCENARIO_TAG = "scenario";

    /**
     * @brief Сконвертировать QDomNode в строку
     */
    static QString nodeToString(const QDomNode& _node) {
            QString ret;
            QTextStream textStream(&ret);
            _node.save(textStream, 0);
            return ret;
    }
}


void ScenarioTextDocument::updateBlockRevision(QTextCursor& _cursor)
{
    _cursor.block().setRevision(_cursor.block().revision() + 1);
}

ScenarioTextDocument::ScenarioTextDocument(QObject *parent, ScenarioXml* _xmlHandler) :
    QTextDocument(parent),
    m_xmlHandler(_xmlHandler),
    m_isPatchApplyProcessed(false),
    m_reviewModel(new ScenarioReviewModel(this)),
    m_outlineMode(false)
{
    connect(m_reviewModel, SIGNAL(reviewChanged()), this, SIGNAL(reviewChanged()));
}

void ScenarioTextDocument::updateScenarioXml()
{
    if (!m_isPatchApplyProcessed) {
        const QString newScenarioXml = m_xmlHandler->scenarioToXml();
        const QByteArray newScenarioXmlHash = ::textMd5Hash(newScenarioXml);

        //
        // Если текущий текст сценария отличается от последнего сохранённого
        //
        if (newScenarioXmlHash != m_scenarioXmlHash) {
            m_scenarioXml = newScenarioXml;
            m_scenarioXmlHash = newScenarioXmlHash;
        }
    }
}

QString ScenarioTextDocument::scenarioXml() const
{
    return m_scenarioXml;
}

QByteArray ScenarioTextDocument::scenarioXmlHash() const
{
    return m_scenarioXmlHash;
}

void ScenarioTextDocument::load(const QString& _scenarioXml)
{
    //
    // Если xml не задан сформируем его пустой аналог
    //
    QString scenarioXml = _scenarioXml;
    if (scenarioXml.isEmpty()) {
        scenarioXml = m_xmlHandler->defaultTextXml();
    }

    //
    // Сохраняем текущий режим, для последующего восстановления
    // FIXME: Так нужно делать, чтобы в режиме поэпизодника не скакал курсор, если этот режим активен
    //
    bool outlineMode = m_outlineMode;
    setOutlineMode(false);

    //
    // Загружаем проект
    //
    m_xmlHandler->xmlToScenario(0, scenarioXml);
    m_scenarioXml = scenarioXml;
    m_scenarioXmlHash = ::textMd5Hash(scenarioXml);
    m_lastSavedScenarioXml = m_scenarioXml;
    m_lastSavedScenarioXmlHash = m_scenarioXmlHash;

    //
    // Восстанавливаем режим
    //
    setOutlineMode(outlineMode);

    m_undoStack.clear();
    m_redoStack.clear();

#ifdef PATCH_DEBUG
    foreach (DomainObject* obj, DataStorageLayer::StorageFacade::scenarioChangeStorage()->all()->toList()) {
        ScenarioChange* ch = dynamic_cast<ScenarioChange*>(obj);
        if (!ch->isDraft()) {
            m_undoStack.append(ch);
        }
    }
    foreach (DomainObject* obj, DataStorageLayer::StorageFacade::scenarioChangeStorage()->all()->toList()) {
        ScenarioChange* ch = dynamic_cast<ScenarioChange*>(obj);
        if (!ch->isDraft()) {
            m_redoStack.prepend(ch);
        }
    }
#endif
}

QString ScenarioTextDocument::mimeFromSelection(int _startPosition, int _endPosition) const
{
    QString mime;

    if (m_xmlHandler != 0) {
        //
        // Скорректируем позиции в случае необходимости
        //
        if (_startPosition > _endPosition) {
            qSwap(_startPosition, _endPosition);
        }

        mime = m_xmlHandler->scenarioToXml(_startPosition, _endPosition);
    }

    return mime;
}

void ScenarioTextDocument::insertFromMime(int _insertPosition, const QString& _mimeData)
{
    if (m_xmlHandler != 0) {
        m_xmlHandler->xmlToScenario(_insertPosition, _mimeData);
    }
}

void ScenarioTextDocument::applyPatch(const QString& _patch)
{
    updateScenarioXml();
    saveChanges();

    m_isPatchApplyProcessed = true;

    //
    // Определим xml для применения патча
    //
    QPair<DiffMatchPatchHelper::ChangeXml, DiffMatchPatchHelper::ChangeXml> xmlsForUpdate;
    const QString patchUncopressed = DatabaseHelper::uncompress(_patch);
    xmlsForUpdate = DiffMatchPatchHelper::changedXml(m_scenarioXml, patchUncopressed);

    xmlsForUpdate.first.xml = ScenarioXml::makeMimeFromXml(xmlsForUpdate.first.xml);
    xmlsForUpdate.second.xml = ScenarioXml::makeMimeFromXml(xmlsForUpdate.second.xml);

    //
    // Удалим одинаковые первые и последние символы
    //
    removeIdenticalParts(xmlsForUpdate, false);
    removeIdenticalParts(xmlsForUpdate, true);

    //
    // Выделяем текст сценария, соответствующий xml для обновления
    //
    QTextCursor cursor(this);
    cursor.beginEditBlock();
    const int selectionStartPos = xmlsForUpdate.first.plainPos;
    const int selectionEndPos = selectionStartPos + xmlsForUpdate.first.plainLength;
    //
    // ... собственно выделение
    //
    setCursorPosition(cursor, selectionStartPos);
    setCursorPosition(cursor, selectionEndPos, QTextCursor::KeepAnchor);

#ifdef PATCH_DEBUG
    qDebug() << "===================================================================";
    qDebug() << cursor.selectedText();
    qDebug() << "###################################################################";
    qDebug() << qPrintable(xmlsForUpdate.first.xml);
    qDebug() << "###################################################################";
    qDebug() << qPrintable(QByteArray::fromPercentEncoding(patchUncopressed.toUtf8()));
    qDebug() << "###################################################################";
    qDebug() << qPrintable(xmlsForUpdate.second.xml);
#endif

    //
    // Замещаем его обновлённым
    //
    cursor.removeSelectedText();
    m_xmlHandler->xmlToScenario(selectionStartPos, xmlsForUpdate.second.xml);

    //
    // Запомним новый текст
    //
    m_scenarioXml = m_xmlHandler->scenarioToXml();
    m_scenarioXmlHash = ::textMd5Hash(m_scenarioXml);
    m_lastSavedScenarioXml = m_scenarioXml;
    m_lastSavedScenarioXmlHash = m_scenarioXmlHash;

    //
    // Патч применён
    //
    m_isPatchApplyProcessed = false;

    //
    // Завершаем изменение документа
    // если завершить изменение сразу после вставки текста, но до запоминания его хэша, то
    // может случиться проблема, что текущий хэш и хэш документа совпадут, что приведёт к
    // отсутствию одного патча в истории изменений
    //
    cursor.endEditBlock();
}

void ScenarioTextDocument::applyPatches(const QList<QString>& _patches)
{
    m_isPatchApplyProcessed = true;


    //
    // Применяем патчи
    //
    QString newXml = m_scenarioXml;
    int currentIndex = 0, max = _patches.size();
    foreach (const QString& patch, _patches) {
        const QString patchUncopressed = DatabaseHelper::uncompress(patch);
        newXml = DiffMatchPatchHelper::applyPatchXml(newXml, patchUncopressed);
        QLightBoxProgress::setProgressValue(++currentIndex * 100 / max);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    //
    // Начинаем изменение текста
    //
    QTextCursor cursor(this);
    cursor.beginEditBlock();

    //
    // Загружаем текст документа
    //
    cursor.select(QTextCursor::Document);
    cursor.removeSelectedText();
    m_xmlHandler->xmlToScenario(0, ScenarioXml::makeMimeFromXml(newXml));

    //
    // Запомним новый текст
    //
    m_scenarioXml = m_xmlHandler->scenarioToXml();
    m_scenarioXmlHash = ::textMd5Hash(m_scenarioXml);
    m_lastSavedScenarioXml = m_scenarioXml;
    m_lastSavedScenarioXmlHash = m_scenarioXmlHash;

    //
    // Патч применён
    //
    m_isPatchApplyProcessed = false;

    //
    // Завершаем изменение документа
    // если завершить изменение сразу после вставки текста, но до запоминания его хэша, то
    // может случиться проблема, что текущий хэш и хэш документа совпадут, что приведёт к
    // отсутствию одного патча в истории изменений
    //
    cursor.endEditBlock();
}

Domain::ScenarioChange* ScenarioTextDocument::saveChanges()
{
    Domain::ScenarioChange* change = 0;

    if (!m_isPatchApplyProcessed) {
        //
        // Если текущий текст сценария отличается от последнего сохранённого
        //
        if (m_scenarioXmlHash != m_lastSavedScenarioXmlHash) {
            //
            // Сформируем изменения
            //
            const QString undoPatch = DiffMatchPatchHelper::makePatchXml(m_scenarioXml, m_lastSavedScenarioXml);
            const QString undoPatchCompressed = DatabaseHelper::compress(undoPatch);
            const QString redoPatch = DiffMatchPatchHelper::makePatchXml(m_lastSavedScenarioXml, m_scenarioXml);
            const QString redoPatchCompressed = DatabaseHelper::compress(redoPatch);

            if (undoPatchCompressed == "AAAAAA==" || redoPatchCompressed == "AAAAAA==") {
                qDebug() << "Shit!";
            }

            //
            // Сохраним изменения
            //
            change = ::saveChange(undoPatchCompressed, redoPatchCompressed);

            //
            // Запомним новый текст
            //
            m_lastSavedScenarioXml = m_scenarioXml;
            m_lastSavedScenarioXmlHash = m_scenarioXmlHash;

            //
            // Корректируем стеки последних действий
            //
            if (m_undoStack.size() == MAX_UNDO_REDO_STACK_SIZE)  {
                m_undoStack.takeFirst();
            }
            m_undoStack.append(change);
            m_redoStack.clear();

#ifdef PATCH_DEBUG
    qDebug() << "-------------------------------------------------------------------";
    qDebug() << qPrintable(QByteArray::fromPercentEncoding(undoPatch.toUtf8()));
    qDebug() << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
    qDebug() << qPrintable(QByteArray::fromPercentEncoding(redoPatch.toUtf8()));
#endif
        }
    }

    return change;
}

void ScenarioTextDocument::undoReimpl()
{
    saveChanges();

    if (!m_undoStack.isEmpty()) {
        Domain::ScenarioChange* change = m_undoStack.takeLast();

#ifdef PATCH_DEBUG
        qDebug() << "*******************************************************************";
        qDebug() << change->uuid().toString() << change->user() << characterCount();
#endif

        m_redoStack.append(change);
        applyPatch(change->undoPatch());

        //
        // Сохраним изменения
        //
        Domain::ScenarioChange* newChange = ::saveChange(change->redoPatch(), change->undoPatch());
        newChange->setIsDraft(change->isDraft());
    }
}

void ScenarioTextDocument::redoReimpl()
{
    if (!m_redoStack.isEmpty()) {
        Domain::ScenarioChange* change = m_redoStack.takeLast();

#ifdef PATCH_DEBUG
        qDebug() << "*******************************************************************";
        qDebug() << change->uuid().toString() << change->user() << characterCount();
#endif

        m_undoStack.append(change);
        applyPatch(change->redoPatch());

        //
        // Сохраним изменения
        //
        Domain::ScenarioChange* newChange = ::saveChange(change->undoPatch(), change->redoPatch());
        newChange->setIsDraft(change->isDraft());
    }
}

bool ScenarioTextDocument::isUndoAvailableReimpl() const
{
    return !m_undoStack.isEmpty();
}

bool ScenarioTextDocument::isRedoAvailableReimpl() const
{
    return !m_redoStack.isEmpty();
}

void ScenarioTextDocument::setCursorPosition(QTextCursor& _cursor, int _position, QTextCursor::MoveMode _moveMode)
{
    //
    // Нормальное позиционирование
    //
    if (_position >= 0 && _position < characterCount()) {
        _cursor.setPosition(_position, _moveMode);
    }
    //
    // Для отрицательного ни чего не делаем, оставляем курсор в нуле
    //
    else if (_position < 0) {
        _cursor.movePosition(QTextCursor::Start, _moveMode);
    }
    //
    // Для очень большого, просто помещаем в конец документа
    //
    else {
        _cursor.movePosition(QTextCursor::End, _moveMode);
    }
}

ScenarioReviewModel*ScenarioTextDocument::reviewModel() const
{
    return m_reviewModel;
}

bool ScenarioTextDocument::outlineMode() const
{
    return m_outlineMode;
}

void ScenarioTextDocument::setOutlineMode(bool _outlineMode)
{
    if (m_outlineMode != _outlineMode) {
        m_outlineMode = _outlineMode;

        //
        // Сформируем список типов блоков для отображения
        //
        QList<ScenarioBlockStyle::Type> visibleBlocksTypes = this->visibleBlocksTypes();

        //
        // Пробегаем документ и настраиваем видимые и невидимые блоки
        //
        QTextCursor cursor(this);
        while (!cursor.atEnd()) {
            QTextBlock block = cursor.block();
            block.setVisible(visibleBlocksTypes.contains(ScenarioBlockStyle::forBlock(block)));
            cursor.movePosition(QTextCursor::EndOfBlock);
            cursor.movePosition(QTextCursor::NextBlock);
        }
    }
}

QList<ScenarioBlockStyle::Type> ScenarioTextDocument::visibleBlocksTypes() const
{
    static QList<ScenarioBlockStyle::Type> s_outlineVisibleBlocksTypes =
        QList<ScenarioBlockStyle::Type>()
        << ScenarioBlockStyle::SceneHeading
        << ScenarioBlockStyle::SceneCharacters
        << ScenarioBlockStyle::FolderHeader
        << ScenarioBlockStyle::FolderFooter
        << ScenarioBlockStyle::SceneDescription;

    static QList<ScenarioBlockStyle::Type> s_scenarioVisibleBlocksTypes =
        QList<ScenarioBlockStyle::Type>()
            << ScenarioBlockStyle::SceneHeading
            << ScenarioBlockStyle::SceneCharacters
            << ScenarioBlockStyle::Action
            << ScenarioBlockStyle::Character
            << ScenarioBlockStyle::Dialogue
            << ScenarioBlockStyle::Parenthetical
            << ScenarioBlockStyle::TitleHeader
            << ScenarioBlockStyle::Title
            << ScenarioBlockStyle::Note
            << ScenarioBlockStyle::Transition
            << ScenarioBlockStyle::NoprintableText
            << ScenarioBlockStyle::FolderHeader
            << ScenarioBlockStyle::FolderFooter;

    return m_outlineMode ? s_outlineVisibleBlocksTypes : s_scenarioVisibleBlocksTypes;
}

void ScenarioTextDocument::removeIdenticalParts(QPair<DiffMatchPatchHelper::ChangeXml, DiffMatchPatchHelper::ChangeXml>& _xmls, bool _reversed)
{
    //
    // Распарсим документы
    //
    QDomDocument sourceDocument;
    sourceDocument.setContent(_xmls.first.xml);

    QDomDocument targetDocument;
    targetDocument.setContent(_xmls.second.xml);

    //
    // Получим список обрабатываемых тегов
    //
    QDomNodeList sourceNodes = sourceDocument.firstChildElement(SCENARIO_TAG).childNodes();
    QDomNodeList targetNodes = targetDocument.firstChildElement(SCENARIO_TAG).childNodes();

    //
    // Позиции первых/последних (в зависимости от _reversed) тегов из childs, содержащих тег <v>
    //
    int sourceCurrentNodePosition = _reversed ? getPrevChild(sourceNodes, sourceNodes.size())
                                              : getNextChild(sourceNodes, -1);
    int targetCurrentNodePosition = _reversed ? getPrevChild(targetNodes, targetNodes.size())
                                              : getNextChild(targetNodes, -1);

    //
    // Предыдущие значения i1 и i2. Необходимы, поскольку последний удаляемые тег удалять не нужно.
    // Нужно заменить его текст пустой строкой
    //
    int sourcePreviousNodePosition = -1;
    int targetPreviousNodePosition = -1;

    //
    // Разбираем текст
    //
    while (((!_reversed                                                        // пока не дошли до конца, для прохода вперёд
             && sourceCurrentNodePosition < sourceNodes.size() - 1
             && targetCurrentNodePosition < targetNodes.size() - 1)
            || (_reversed                                                      // или пока не дошли до начала, для прохода назад
                && sourceCurrentNodePosition > 0
                && targetCurrentNodePosition > 0))
           && (nodeToString(sourceNodes.at(sourceCurrentNodePosition))         // и текст элементов совпадает
               == nodeToString(targetNodes.at(targetCurrentNodePosition)))) {

        //
        // Получим текущие обрабатываемые строки
        //
        const QString sourceCurrentNodeText =
                TextEditHelper::fromHtmlEscaped(
                    sourceNodes
                    .at(sourceCurrentNodePosition)
                    .firstChildElement("v")
                    .childNodes()
                    .at(0).toCDATASection().data());
        const QString targetCurrentNodeText =
                TextEditHelper::fromHtmlEscaped(
                    targetNodes
                    .at(targetCurrentNodePosition)
                    .firstChildElement("v")
                    .childNodes()
                    .at(0).toCDATASection().data());

        //
        // Если идёт проход вперёд, то
        //
        if (!_reversed) {
            //
            // ... удаляем предыдущую пустую ячейку, если есть
            //
            if (sourcePreviousNodePosition != -1
                && targetPreviousNodePosition != -1) {
                sourceDocument.firstChildElement(SCENARIO_TAG).removeChild(sourceNodes.at(sourcePreviousNodePosition));
                --sourceCurrentNodePosition;
                //
                targetDocument.firstChildElement(SCENARIO_TAG).removeChild(targetNodes.at(targetPreviousNodePosition));
                --targetCurrentNodePosition;

                //
                // ... скорректируем позицию затрагиваемую патчем на символ переноса строки
                //
                _xmls.first.plainPos += 1;
                _xmls.first.plainLength -= 1;
            }

            //
            // ... затираем текст ячеек
            //
            sourceNodes.at(sourceCurrentNodePosition).firstChildElement("v").firstChild().toCDATASection().setData("");
            QDomElement sourceNodeReviews = sourceNodes.at(sourceCurrentNodePosition).firstChildElement("reviews");
            sourceNodes.at(sourceCurrentNodePosition).removeChild(sourceNodeReviews);
            //
            targetNodes.at(targetCurrentNodePosition).firstChildElement("v").firstChild().toCDATASection().setData("");
            QDomElement targetNodeReviews = targetNodes.at(targetCurrentNodePosition).firstChildElement("reviews");
            targetNodes.at(targetCurrentNodePosition).removeChild(targetNodeReviews);

            //
            // ... скорректируем позицию затрагиваемую патчем на длину стёртой строки
            //
            _xmls.first.plainPos += sourceCurrentNodeText.size();
            _xmls.first.plainLength -= sourceCurrentNodeText.size();
        }
        //
        // Если идёт проход назад, то
        //
        else {
            //
            // ... убираем блок полностью
            //
            sourceDocument.firstChildElement(SCENARIO_TAG).removeChild(sourceNodes.at(sourceCurrentNodePosition));
            targetDocument.firstChildElement(SCENARIO_TAG).removeChild(targetNodes.at(targetCurrentNodePosition));

            //
            // ... скорректируем позицию затрагиваемую патчем на длину строки + символ переноса строки
            //
            _xmls.first.plainLength -= sourceCurrentNodeText.size() + 1;
        }

        //
        // Запомним предыдущие значения
        //
        sourcePreviousNodePosition = sourceCurrentNodePosition;
        targetPreviousNodePosition = targetCurrentNodePosition;

        //
        // Получим новые
        //
        sourceCurrentNodePosition = _reversed ? getPrevChild(sourceNodes, sourceCurrentNodePosition)
                                              : getNextChild(sourceNodes, sourceCurrentNodePosition);
        targetCurrentNodePosition = _reversed ? getPrevChild(targetNodes, targetCurrentNodePosition)
                                              : getNextChild(targetNodes, targetCurrentNodePosition);
    }

    //
    // Результаты
    //
    _xmls.first.xml = sourceDocument.toString();
    _xmls.second.xml = targetDocument.toString();
}

void ScenarioTextDocument::processLenghtPos(DiffMatchPatchHelper::ChangeXml& _xmls, int _k, bool _reversed)
{
    _xmls.plainLength -= _k;
    if (!_reversed) {
        _xmls.plainPos += _k;
    }
}

int ScenarioTextDocument::getNextChild(QDomNodeList& list, int prev) {
    for(int i = prev + 1; i < list.size(); ++i) {
        QDomElement elem = list.at(i).firstChildElement("v");
        if (!elem.isNull()) {
            return i;
        }
    }
    return list.size();
}

int ScenarioTextDocument::getPrevChild(QDomNodeList& list, int prev) {
    for(int i = prev - 1; i >= 0; --i) {
        QDomElement elem = list.at(i).firstChildElement("v");
        if (!elem.isNull()) {
            return i;
        }
    }
    return -1;
}

