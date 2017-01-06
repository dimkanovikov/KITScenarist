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
#include <QTextBlock>
#include <QDomDocument>

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
    emit beforePatchApply();
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
    cursor.endEditBlock();

    //
    // Запомним новый текст
    //
    m_scenarioXml = m_xmlHandler->scenarioToXml();
    m_scenarioXmlHash = ::textMd5Hash(m_scenarioXml);
    m_lastSavedScenarioXml = m_scenarioXml;
    m_lastSavedScenarioXmlHash = m_scenarioXmlHash;


    m_isPatchApplyProcessed = false;
    emit afterPatchApply();
}

void ScenarioTextDocument::applyPatches(const QList<QString>& _patches)
{
    emit beforePatchApply();
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
    // Перезагружаем текст документа
    //
    QTextCursor cursor(this);
    cursor.beginEditBlock();
    cursor.select(QTextCursor::Document);
    cursor.removeSelectedText();
    m_xmlHandler->xmlToScenario(0, ScenarioXml::makeMimeFromXml(newXml));
    cursor.endEditBlock();

    //
    // Запомним новый текст
    //
    m_scenarioXml = m_xmlHandler->scenarioToXml();
    m_scenarioXmlHash = ::textMd5Hash(m_scenarioXml);
    m_lastSavedScenarioXml = m_scenarioXml;
    m_lastSavedScenarioXmlHash = m_scenarioXmlHash;


    m_isPatchApplyProcessed = false;
    emit afterPatchApply();
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
        << ScenarioBlockStyle::SceneGroupHeader
        << ScenarioBlockStyle::SceneGroupFooter
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
            << ScenarioBlockStyle::SceneGroupHeader
            << ScenarioBlockStyle::SceneGroupFooter
            << ScenarioBlockStyle::FolderHeader
            << ScenarioBlockStyle::FolderFooter;

    return m_outlineMode ? s_outlineVisibleBlocksTypes : s_scenarioVisibleBlocksTypes;
}

void ScenarioTextDocument::removeIdenticalParts(QPair<DiffMatchPatchHelper::ChangeXml, DiffMatchPatchHelper::ChangeXml>& _xmls, bool _reversed)
{
    //
    // Суть происходящего следующая (рассмотрим _reversed = false, для true аналогично).
    // Последовательно обрабатываем теги, содержащие <v> (а значит и CDATA). Если содержимое двух тего идентично,
    // то по сути можно удалить. Но! Если самое изменение суть вставка нового блока, то необходимо, чтобы перед ним
    // был пустой старый блок, иначе новый блок будет обработан как старый. Поэтому, последний тег мы не удаляем, а храним.
    //


    //
    // Распарсим документы
    //
    QDomDocument d1;
    d1.setContent(_xmls.first.xml);

    QDomDocument d2;
    d2.setContent(_xmls.second.xml);

    //
    // Получим список обрабатываемых тегов
    //
    QDomNodeList childs1 = d1.childNodes().at(0).childNodes();
    QDomNodeList childs2 = d2.childNodes().at(0).childNodes();

    //
    // Позиции первых/последних (в зависимости от _reversed) тегов из childs, содержащих тег <v>
    //
    int i1 = _reversed ? getPrevChild(childs1, childs1.size()) : getNextChild(childs1, -1);
    int i2 = _reversed ? getPrevChild(childs2, childs2.size()) : getNextChild(childs2, -1);

    //
    // Предыдущие значения i1 и i2. Необходимы, поскольку последний удаляемые тег удалять не нужно.
    // Нужно заменить его текст пустой строкой
    //
    int prevNode1 = -1;
    int prevNode2 = -1;

    //
    // В заголовке цикла мы идем до тех пор, пока, либо не прошли все теги хотя бы одного документа,
    // либо обнаружили разные теги у обрабатываемых
    //
    while (((!_reversed && i1 != childs1.size() && i2 != childs2.size()) ||
           (_reversed && i1 >= 0 && i2 >= 0)) && childs1.at(i1).nodeName() == childs2.at(i2).nodeName()) {

        //
        // Получим текущие обрабатываемые строки
        //
        QString str1 = childs1.at(i1).firstChildElement("v").childNodes().at(0).toCDATASection().data();
        QString str2 = childs2.at(i2).firstChildElement("v").childNodes().at(0).toCDATASection().data();

        //
        // Если строки оказались равны
        //
        if (str1 == str2) {

            //
            // Можем удалить предыдущие теги, если они у нас есть
            //
            if (prevNode1 != -1) {
                d1.childNodes().at(0).removeChild(childs1.at(prevNode1));
                //
                // Раз мы удалили тег, то все справа сдвинулось влево на 1
                //
                if (!_reversed) {
                    --i1;
                }
            }

            //
            // Аналогично
            //
            if (prevNode2 != -1) {
                d2.childNodes().at(0).removeChild(childs2.at(prevNode2));
                if (!_reversed) {
                    --i2;
                }
            }

            //
            // Обработаем длину и позицию вставки
            // Если какой-то тег удалили, то удалили еще один символ (\n)
            //
            processLenghtPos(_xmls.first, str1.size() + (prevNode1 == -1 ? 0 : 1), _reversed);
            processLenghtPos(_xmls.second, str2.size() + (prevNode2 == -1 ? 0 : 1), _reversed);

            //
            // Запомним предыдущие значения
            //
            prevNode1 = i1;
            prevNode2 = i2;

            //
            // Получим новые
            //
            i1 = _reversed ? getPrevChild(childs1, i1) : getNextChild(childs1, i1);
            i2 = _reversed ? getPrevChild(childs2, i2) : getNextChild(childs2, i2);
            continue;
        }

        //
        // Если строки оказались не равны
        //

        //
        // Тогда предыдущий тег нам хранить не зачем, даже пустой
        //
        if (prevNode1 != -1) {
            //
            // Удалим его (так же как и ранее)
            //
            d1.childNodes().at(0).removeChild(childs1.at(prevNode1));
            processLenghtPos(_xmls.first, 1, _reversed);
            prevNode1 = -1;
            if (!_reversed){
                --i1;
            }
        }

        //
        // Аналогично и со вторым
        //
        if (prevNode2 != -1) {
            d2.childNodes().at(0).removeChild(childs2.at(prevNode2));
            processLenghtPos(_xmls.second, 1, _reversed);
            prevNode2 = -1;
            if (!_reversed) {
                --i2;
            }
        }

        //
        // Извлечем максимальную общую длину
        //
        int k;
        for (k = 0; k != qMin(str1.size(), str2.size()); ++k) {
            if ((!_reversed && str1[k] != str2[k]) ||
                    (_reversed && str1[str1.size() - k - 1] != str2[str2.size() - k - 1])) {
                break;
            }
        }

        //
        // Нулевая - неинтересный случай
        //
        if (k == 0) {
            break;
        }

        //
        // Обработаем первую строку
        //
        if (k != str1.size()) {
            //
            // Обработали не всю строку. Удалим часть, совпадающую со второй строкой
            //
            QString res1 = _reversed ? str1.left(str1.size() - k) : str1.right(str1.size() - k);
            childs1.at(i1).firstChildElement("v").childNodes().at(0).toCDATASection().setData(res1);

            //
            // Не забудем обновить длину и позицию вставки
            //
            processLenghtPos(_xmls.first, k, _reversed);
        } else {
            //
            // Обработали всю строку. Удалим текущий тег
            //
            d1.childNodes().at(0).removeChild(childs1.at(i1));
            if (!_reversed) {
                --i1;
            }
            processLenghtPos(_xmls.first, k, _reversed);
        }

        //
        // Аналогично обработаем вторую строку
        if (k != str2.size()) {
            QString res2 = _reversed ? str2.left(str2.size() - k) : str2.right(str2.size() - k);
            childs2.at(i2).firstChildElement("v").childNodes().at(0).toCDATASection().setData(res2);
            processLenghtPos(_xmls.second, k, _reversed);
        } else {
            d2.childNodes().at(0).removeChild(childs2.at(i2));
            if (!_reversed) {
                --i2;
            }
            processLenghtPos(_xmls.second, k, _reversed);
        }

        break;
    }

    //
    // Последний тег, подлежащий удалению не удаляем, а делаем его текст пустым
    //
    if (prevNode1 != -1) {
        childs1.at(prevNode1).firstChildElement("v").childNodes().at(0).toCDATASection().setData("");
    }
    if (prevNode2 != -1) {
        childs2.at(prevNode2).firstChildElement("v").childNodes().at(0).toCDATASection().setData("");
    }

    //
    // Результаты
    //
    _xmls.first.xml = d1.toString();
    _xmls.second.xml = d2.toString();
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

