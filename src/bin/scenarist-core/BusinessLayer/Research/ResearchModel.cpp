#include "ResearchModel.h"

#include "ResearchModelItem.h"

#include <Domain/Research.h>

#include <QMimeData>
#include <QSet>

using BusinessLogic::ResearchModel;
using BusinessLogic::ResearchModelItem;
using Domain::Research;

namespace {
    /**
     * @brief Пустой фильтр
     */
    const QSet<Research::Type> emptyFilter;

    /**
     * @brief Построить ветвь дерева разработки с заданным фильтром по элементам
     */
    static void populateTree(const QMap<Research*, QList<Research*>>& _researchMap,
        const QSet<Research::Type>& _filter, ResearchModelItem* _parent, Research* _research) {
        //
        // Добавим элемент в дерево разработки, если фильтр не задан, или в фильтре нет типа текущего элемента
        //
        if (_filter.isEmpty()
            || _filter.contains(_research->type())) {
            //
            // Добавляем очередной корневой элемент разработки в дерево
            //
            ResearchModelItem* researchItem = new ResearchModelItem(_research);
            _parent->appendItem(researchItem);

            //
            // Прорабатываем всех его детей
            //
            foreach (Research* researchChild, _researchMap.value(_research)) {
                populateTree(_researchMap, emptyFilter, researchItem, researchChild);
            }
        }
    }

    /**
     * @brief Построить дерево персонажей
     */
    static void populateCharactersTree(const QMap<Research*, QList<Research*>>& _researchMap,
        ResearchModelItem* _parent, Research* _research) {
        QSet<Research::Type> filter = { Research::Character };
        populateTree(_researchMap, filter, _parent, _research);
    }

    /**
     * @brief Построить дерево локаций
     */
    static void populateLocationsTree(const QMap<Research*, QList<Research*>>& _researchMap,
        ResearchModelItem* _parent, Research* _research) {
        QSet<Research::Type> filter = { Research::Location };
        populateTree(_researchMap, filter, _parent, _research);
    }

    /**
     * @brief Построить дерево данных разработки
     */
    static void populateResearchTree(const QMap<Research*, QList<Research*>>& _researchMap,
        ResearchModelItem* _parent, Research* _research) {
        QSet<Research::Type> filter = { Research::Folder,
                                        Research::Text,
                                        Research::Url,
                                        Research::ImagesGallery,
                                        Research::Image,
                                        Research::MindMap };
        populateTree(_researchMap, filter, _parent, _research);
    }

    /**
     * @brief Скопировать дочерние элементы из одного родителя в другой
     */
    static void copyChildItems(ResearchModelItem* _parent, ResearchModelItem* _parentOldCopy) {
        for (int childRow = 0; childRow < _parentOldCopy->childCount(); ++childRow) {
            ResearchModelItem* childItemOldCopy = _parentOldCopy->childAt(childRow);
            ResearchModelItem* childItem = new ResearchModelItem(childItemOldCopy->research());
            _parent->appendItem(childItem);
            copyChildItems(childItem, childItemOldCopy);
        }
    }
}


QString ResearchModel::MIME_TYPE = "application/x-scenarist/research-tree";

ResearchModel::ResearchModel(QObject* _parent) :
    QAbstractItemModel(_parent),
    m_rootItem(new ResearchModelItem),
    m_researchRoot(0),
    m_researchData(0)
{
    //
    // Сценарий
    //
    ResearchModelItem* scenarioItem =
        new ResearchModelItem(
            new Research(Domain::Identifier(), 0, Research::Scenario, 0, tr("Scenario"))
        );
    m_rootItem->appendItem(scenarioItem);
    //
    // Титульная страница
    //
    ResearchModelItem* titlePageItem =
        new ResearchModelItem(
            new Research(Domain::Identifier(), 0, Research::TitlePage, 0, tr("Title Page"))
        );
    scenarioItem->appendItem(titlePageItem);
    //
    // Синопсис сценария
    //
    ResearchModelItem* synopsisItem =
        new ResearchModelItem(
            new Research(Domain::Identifier(), 0, Research::Synopsis, 1, tr("Synopsis"))
        );
    scenarioItem->appendItem(synopsisItem);

    //
    // Персонажи
    //
    m_charactersRoot =
        new ResearchModelItem(
            new Research(Domain::Identifier(), 0, Research::CharactersRoot, 1, tr("Characters"))
        );
    m_rootItem->appendItem(m_charactersRoot);

    //
    // Локации
    //
    m_locationsRoot =
        new ResearchModelItem(
            new Research(Domain::Identifier(), 0, Research::LocationsRoot, 2, tr("Locations"))
        );
    m_rootItem->appendItem(m_locationsRoot);

    //
    // Корневая папка для разработки
    //
    m_researchRoot =
        new ResearchModelItem(
            new Research(Domain::Identifier(), 0, Research::ResearchRoot, 3, tr("Research"))
        );
    m_rootItem->appendItem(m_researchRoot);
}

ResearchModel::~ResearchModel()
{
    delete m_rootItem;
    m_rootItem = 0;
}

void ResearchModel::load(Domain::ResearchTable* _data)
{
    if (m_researchData != _data) {
        m_researchData = _data;

        //
        // Делаем обновления модели разработки, при изменении данных таблицы с данными
        //

        if (m_researchData != 0) {
            connect(m_researchData, &Domain::ResearchTable::rowsInserted, this, &ResearchModel::researchRowsInserted);
            connect(m_researchData, &Domain::ResearchTable::rowsAboutToBeRemoved, this, &ResearchModel::researchRowsRemoved);
            connect(m_researchData, &Domain::ResearchTable::dataChanged, this, &ResearchModel::researchDataChanged);
        }
    }

    clear();

    if (m_researchData != 0) {
        //
        // Формируем карту разработок
        // первыми в ней будут идти корневые элементы
        //
        QMap<Research*, QList<Research*> > researchMap;
        foreach (Domain::DomainObject* domainObject, m_researchData->toList()) {
            if (Research* research = dynamic_cast<Research*>(domainObject)) {
                if (researchMap.contains(research->parent())) {
                    QList<Research*> childs = researchMap.value(research->parent());
                    childs.append(research);
                    researchMap[research->parent()] = childs;
                } else {
                    QList<Research*> childs;
                    childs.append(research);
                    researchMap[research->parent()] = childs;
                }
            }
        }

        //
        // Обходим только корневые элементы
        //
        // ... для дерева персонажей
        //
        foreach (Research* research, researchMap.value(0)) {
            ::populateCharactersTree(researchMap, m_charactersRoot, research);
        }
        //
        // ... для дерева локаций
        //
        foreach (Research* research, researchMap.value(0)) {
            ::populateLocationsTree(researchMap, m_locationsRoot, research);
        }
        //
        // ... для дерева данных разработки
        //
        foreach (Research* research, researchMap.value(0)) {
            ::populateResearchTree(researchMap, m_researchRoot, research);
        }
    }
}

void ResearchModel::clear()
{
    //
    // Пересоздаём корень разработки
    //
    emit beginRemoveRows(QModelIndex(), 1, 3);
    m_rootItem->removeItem(m_charactersRoot);
    m_rootItem->removeItem(m_locationsRoot);
    m_rootItem->removeItem(m_researchRoot);
    emit endRemoveRows();
    //
    m_charactersRoot =
        new ResearchModelItem(
            new Research(Domain::Identifier(), 0, Research::CharactersRoot, 1, tr("Characters"))
        );
    //
    m_locationsRoot =
        new ResearchModelItem(
            new Research(Domain::Identifier(), 0, Research::LocationsRoot, 2, tr("Locations"))
        );
    //
    m_researchRoot =
        new ResearchModelItem(
            new Research(Domain::Identifier(), 0, Research::ResearchRoot, 1, tr("Research"))
        );
    //
    emit beginInsertRows(QModelIndex(), 1, 3);
    m_rootItem->appendItem(m_charactersRoot);
    m_rootItem->appendItem(m_locationsRoot);
    m_rootItem->appendItem(m_researchRoot);
    emit endInsertRows();
}

void ResearchModel::prependItem(ResearchModelItem* _item, ResearchModelItem* _parentItem)
{
    //
    // Если родитель не задан им становится сам сценарий
    //
    if (_parentItem == 0) {
        _parentItem = m_rootItem;
    }

    //
    // Если такого элемента ещё нет у родителя
    //
    if (_parentItem->rowOfChild(_item) == -1) {
        QModelIndex parentIndex = indexForItem(_parentItem);
        int itemRowIndex = 0; // т.к. в самое начало

        beginInsertRows(parentIndex, itemRowIndex, itemRowIndex);
        _parentItem->prependItem(_item);
        endInsertRows();
    }
}

void ResearchModel::appendItem(ResearchModelItem* _item, ResearchModelItem* _parentItem)
{
    //
    // Если родитель не задан им становится сам сценарий
    //
    if (_parentItem == 0) {
        _parentItem = m_rootItem;
    }

    //
    // Если такого элемента ещё нет у родителя
    //
    if (_parentItem->rowOfChild(_item) == -1) {
        QModelIndex parentIndex = indexForItem(_parentItem);

        //
        // Определим позицию вставки
        //
        int itemRowIndex = _parentItem->childCount();

        beginInsertRows(parentIndex, itemRowIndex, itemRowIndex);
        _parentItem->insertItem(itemRowIndex, _item);
        endInsertRows();
    }
}

void ResearchModel::insertItem(ResearchModelItem* _item, ResearchModelItem* _afterSiblingItem)
{
    ResearchModelItem* parent = _afterSiblingItem->parent();

    //
    // Если такого элемента ещё нет у родителя
    //
    if (parent->rowOfChild(_item) == -1) {
        QModelIndex parentIndex = indexForItem(parent);
        int itemRowIndex = parent->rowOfChild(_afterSiblingItem) + 1;

        beginInsertRows(parentIndex, itemRowIndex, itemRowIndex);
        parent->insertItem(itemRowIndex, _item);
        endInsertRows();
    }
}

void ResearchModel::removeItem(ResearchModelItem* _item)
{
    ResearchModelItem* itemParent = _item->parent();
    QModelIndex itemParentIndex = indexForItem(_item).parent();
    int itemRowIndex = itemParent->rowOfChild(_item);

    beginRemoveRows(itemParentIndex, itemRowIndex, itemRowIndex);
    itemParent->removeItem(_item);
    endRemoveRows();
}

void ResearchModel::updateItem(ResearchModelItem* _item)
{
    //
    // Если элемент уже в списке, то обновим, в противном случае просто игнорируем
    //
    if (_item->parent() != 0) {
        const QModelIndex indexForUpdate = indexForItem(_item);
        emit dataChanged(indexForUpdate, indexForUpdate);
    }
}

QModelIndex ResearchModel::index(int _row, int _column, const QModelIndex& _parent) const
{
    QModelIndex resultIndex;
    if (_row < 0
        || _row > rowCount(_parent)
        || _column < 0
        || _column > columnCount(_parent)
        || (_parent.isValid() && (_parent.column() != 0))
        ) {
        resultIndex = QModelIndex();
    } else {
        ResearchModelItem* parentItem = itemForIndex(_parent);
        Q_ASSERT(parentItem);

        ResearchModelItem* indexItem = parentItem->childAt(_row);
        if (indexItem != 0) {
            resultIndex = createIndex(_row, _column, indexItem);
        }
    }
    return resultIndex;
}

QModelIndex ResearchModel::parent(const QModelIndex& _child) const
{
    QModelIndex parentIndex;
    if (_child.isValid()) {
        ResearchModelItem* childItem = itemForIndex(_child);
        ResearchModelItem* parentItem = childItem->parent();
        if (parentItem != 0
            && parentItem != m_rootItem) {
            ResearchModelItem* grandParentItem = parentItem->parent();
            if (grandParentItem != 0) {
                int row = grandParentItem->rowOfChild(parentItem);
                parentIndex = createIndex(row, 0, parentItem);
            }
        }
    }
    return parentIndex;
}

int ResearchModel::columnCount(const QModelIndex&) const
{
    return 1;
}

int ResearchModel::rowCount(const QModelIndex& _parent) const
{
    int rowCount = 0;
    if (_parent.isValid() && (_parent.column() != 0)) {
        //
        // Ноль строк
        //
    } else {
        ResearchModelItem* item = itemForIndex(_parent);
        if (item != 0) {
            rowCount = item->childCount();
        }
    }
    return rowCount;
}

Qt::ItemFlags ResearchModel::flags(const QModelIndex& _index) const
{
    Qt::ItemFlags flags = Qt::NoItemFlags;
    if (_index.isValid()) {
        flags |= Qt::ItemIsEnabled | Qt::ItemIsSelectable;

        ResearchModelItem* item = itemForIndex(_index);
        switch (item->research()->type()) {
            case Research::CharactersRoot:
            case Research::LocationsRoot:
            case Research::ResearchRoot: {
                flags |= Qt::ItemIsDropEnabled;
                break;
            }

            case Research::Character:
            case Research::Location:
            case Research::Folder: {
                //
                // Если в настоящий момент перемещается локация или персонаж, то опускаем флаг вложения
                //
                bool ignoreFlags = false;
                for (ResearchModelItem* researchItem : m_lastMimeItems) {
                    Domain::Research* research = researchItem->research();
                    if (research->type() == Research::Character
                        || research->type() == Research::Location) {
                        ignoreFlags = true;
                        break;
                    }
                }
                if (!ignoreFlags) {
                    flags |= Qt::ItemIsDropEnabled;
                }
                flags |= Qt::ItemIsDragEnabled;
                break;
            }

            case Research::Text:
            case Research::ImagesGallery:
            case Research::MindMap: {
                flags |= Qt::ItemIsDragEnabled;
                break;
            }

            case Research::Url: {
#ifdef Q_OS_WIN
                //
                // В виндовс XP webengine не работает, поэтому делаем ссылки недоступными для открытия
                //
                if (QSysInfo::windowsVersion() == QSysInfo::WV_XP) {
                    flags ^= Qt::ItemIsEnabled;
                }
#endif
                flags |= Qt::ItemIsDragEnabled;
                break;
            }

            default: {
                break;
            }
        }
    }

    return flags;
}

QVariant ResearchModel::data(const QModelIndex& _index, int _role) const
{
    QVariant result;

    ResearchModelItem* item = itemForIndex(_index);
    switch (_role) {
        case Qt::DisplayRole: {
            result = item->name();
            break;
        }

        case Qt::DecorationRole: {
            result = QVariant::fromValue(item->icon());
            break;
        }

        default: {
            break;
        }
    }

    return result;
}

bool ResearchModel::canDropMimeData(const QMimeData* _data, Qt::DropAction _action, int _row,
    int _column, const QModelIndex& _parent) const
{
    Q_UNUSED(_action);
    Q_UNUSED(_row);

    if (!_data->hasFormat(MIME_TYPE))
        return false;

    if (_column > 0)
        return false;

    //
    // Обработка конкретных случаев
    //
    ResearchModelItem* parentItem = itemForIndex(_parent);
    Domain::Research* parent  = parentItem->research();
    if (parent != nullptr) {
        for (ResearchModelItem* researchItem : m_lastMimeItems) {
            Domain::Research* research = researchItem->research();
            switch (research->type()) {
                case Research::Character: {
                    //
                    // ... персонажей только в список персонажей
                    //
                    if (parent->type() != Research::CharactersRoot) {
                        return false;
                    }
                    break;
                }

                case Research::Location: {
                    //
                    // ... локации только в список локаций
                    //
                    if (parent->type() != Research::LocationsRoot) {
                        return false;
                    }
                    break;
                }

                default: {
                    //
                    // ... остальных нельзя вставлять в общие списки локаций и персонажей
                    //
                    if (parent->type() == Research::CharactersRoot
                        || parent->type() == Research::LocationsRoot) {
                        return false;
                    }
                    break;
                }
            }
        }
    }

    //
    // Во всех остальных случаях можно
    //
    return true;
}

bool ResearchModel::dropMimeData(
        const QMimeData* _data, Qt::DropAction _action,
        int _row, int _column, const QModelIndex& _parent)
{
    if (!canDropMimeData(_data, _action, _row, _column, _parent)
        || !_parent.isValid()) {
        m_lastMimeItems.clear();
        return false;
    }

    if (_action == Qt::IgnoreAction)
        return true;

    QByteArray encodedData = _data->data(MIME_TYPE);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    ResearchModelItem* parentItem = itemForIndex(_parent);
    QVector<ResearchModelItem*> newItems;
    int row = 0;
    while (!stream.atEnd()) {
        int researchId;
        stream >> researchId;
        Domain::Research* research = m_lastMimeItems[row]->research();
        if (research->id().value() == researchId) {
            research->setParent(parentItem->research());
            ResearchModelItem* item = new ResearchModelItem(research);
            ::copyChildItems(item, m_lastMimeItems[row]);

            newItems << item;
        }

        ++row;
    }

    //
    // Вставляем элементы в нужное место
    //
    row = 0;
    const bool useAppend = !parentItem->hasChildren() || _row == -1;
    foreach (ResearchModelItem* newItem, newItems) {
        if (useAppend) {
            appendItem(newItem, parentItem);
        } else {
            //
            // -1, т.к. нужно вставить перед _row
            //
            const int rowForInsert = _row + row - 1;
            if (rowForInsert == -1) {
                prependItem(newItem, parentItem);
            } else {
                QModelIndex siblingItemIndex= _parent.child(rowForInsert, _column);
                ResearchModelItem* siblingItem = itemForIndex(siblingItemIndex);
                insertItem(newItem, siblingItem);
            }
            ++row;
        }
    }

    //
    // Уведомим о перемещении элемента
    //
    if (!newItems.isEmpty()) {
        emit itemMoved(indexForItem(newItems.first()));
    }

    //
    // Удаляем старые данные разработки
    // NOTE: Это необходимо делать перед обновлением порядка сортировки,
    //       чтобы не смешались старые и новые элементы в одном родителе
    //
    while (!m_lastMimeItems.isEmpty()) {
        removeItem(m_lastMimeItems.takeLast());
    }

    //
    // Обновляем порядок сортировки
    //
    for (int row = 0; row < parentItem->childCount(); ++row) {
        parentItem->childAt(row)->research()->setSortOrder(row);
    }

    return true;
}

QMimeData* ResearchModel::mimeData(const QModelIndexList& _indexes) const
{
    m_lastMimeItems.clear();

    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    foreach (const QModelIndex& index, _indexes) {
        if (index.isValid()) {
            ResearchModelItem* item = itemForIndex(index);
            stream << item->research()->id().value();

            m_lastMimeItems << item;
        }
    }
    mimeData->setData(MIME_TYPE, encodedData);

    return mimeData;
}

QStringList ResearchModel::mimeTypes() const
{
    return { MIME_TYPE };
}

Qt::DropActions ResearchModel::supportedDragActions() const
{
    return Qt::MoveAction;
}

Qt::DropActions ResearchModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

ResearchModelItem* ResearchModel::itemForIndex(const QModelIndex& _index) const
{
    ResearchModelItem* resultItem = m_rootItem;
    if (_index.isValid()) {
        ResearchModelItem* item = static_cast<ResearchModelItem*>(_index.internalPointer());
        if (item != 0) {
            resultItem = item;
        }
    }
    return resultItem;
}

QModelIndex ResearchModel::indexForItem(ResearchModelItem* _item) const
{
    if (_item == 0) {
        return QModelIndex();
    }

    QModelIndex parent;
    if (_item->hasParent()
        && _item->parent()->hasParent()) {
        parent = indexForItem(_item->parent());
    } else {
        parent = QModelIndex();
    }

    int row;
    if (_item->hasParent()
        && _item->parent()->hasParent()) {
        row = _item->parent()->rowOfChild(_item);
    } else {
        row = m_rootItem->rowOfChild(_item);
    }

    return index(row, 0, parent);
}

QModelIndexList ResearchModel::getPersistentIndexList() const
{
    return persistentIndexList();
}

void ResearchModel::reload()
{
    emit beginResetModel();
    load(m_researchData);
    emit endResetModel();
}

ResearchModelItem* ResearchModel::findResearchModelItem(ResearchModelItem* _item, Research* _researchParent)
{
    ResearchModelItem* result = 0;
    //
    // Если текущий элемент искомый
    //
    if (_item->research() == _researchParent) {
        result = _item;
    }
    //
    // Если нет, то проверяем его детей
    //
    else if (_item->hasChildren()) {
        for (int childIndex = 0; childIndex < _item->childCount(); ++childIndex) {
            result = findResearchModelItem(_item->childAt(childIndex), _researchParent);
            if (result != 0) {
                break;
            }
        }
    }

    return result;
}

void ResearchModel::researchRowsInserted(const QModelIndex& _parent, int _first, int _last)
{
    Q_UNUSED(_parent);

    //
    // Получим добавленные элементы разработки
    //
    for (int row = _first; row <= _last; ++row) {
        QModelIndex itemIndex = m_researchData->index(row, 0);
        Domain::DomainObject* domainObject = m_researchData->itemForIndex(itemIndex);
        if (Research* research = dynamic_cast<Research*>(domainObject)) {
            //
            // Определим родителя
            //
            ResearchModelItem* researchParent = nullptr;
            if (research->parent() == 0) {
                if (research->type() == Research::Character) {
                    researchParent = m_charactersRoot;
                } else if (research->type() == Research::Location) {
                    researchParent = m_locationsRoot;
                } else {
                    researchParent = m_researchRoot;
                }
            } else {
                //
                // Ищем родителя по всем возможным местам
                //
                researchParent = findResearchModelItem(m_researchRoot, research->parent());
                if (researchParent == nullptr) {
                    researchParent = findResearchModelItem(m_charactersRoot, research->parent());
                }
                if (researchParent == nullptr) {
                    researchParent = findResearchModelItem(m_locationsRoot, research->parent());
                }
            }
            //
            // Добавим в модель
            //
            bool isPrepend = false;
            ResearchModelItem* researchSibling = 0;
            if (researchParent->hasChildren()) {
                for (int childIndex = 0; childIndex < researchParent->childCount(); ++childIndex) {
                    if (researchParent->childAt(childIndex)->research()->sortOrder() >= research->sortOrder()) {
                        if (childIndex == 0) {
                            isPrepend = true;
                        } else {
                            //
                            // Нам нужен сосед, после которого будет вставлен текущий элемент
                            //
                            researchSibling = researchParent->childAt(childIndex - 1);
                        }
                        break;
                    }
                }
            }
            ResearchModelItem* researchItem = new ResearchModelItem(research);
            if (isPrepend) {
                prependItem(researchItem, researchParent);
            } else if (researchSibling == 0) {
                appendItem(researchItem, researchParent);
            } else {
                insertItem(researchItem, researchSibling);
            }
        }
    }
}

void ResearchModel::researchRowsRemoved(const QModelIndex& _parent, int _first, int _last)
{
    Q_UNUSED(_parent);

    //
    // Получим удаляемые элементы разработки
    //
    for (int row = _last; row >= _first; --row) {
        QModelIndex itemIndex = m_researchData->index(row, 0);
        Domain::DomainObject* domainObject = m_researchData->itemForIndex(itemIndex);
        if (Research* research = dynamic_cast<Research*>(domainObject)) {
            //
            // Определим родителя
            //
            ResearchModelItem* researchParent = 0;
            if (research->parent() == 0) {
                if (research->type() == Research::Character) {
                    researchParent = m_charactersRoot;
                } else if (research->type() == Research::Location) {
                    researchParent = m_locationsRoot;
                } else {
                    researchParent = m_researchRoot;
                }
            } else {
                //
                // Ищем родителя по всем возможным местам
                //
                researchParent = findResearchModelItem(m_researchRoot, research->parent());
                if (researchParent == nullptr) {
                    researchParent = findResearchModelItem(m_charactersRoot, research->parent());
                }
                if (researchParent == nullptr) {
                    researchParent = findResearchModelItem(m_locationsRoot, research->parent());
                }
            }

            //
            // Удалим из модели
            //
            ResearchModelItem* researchToRemove = 0;
            if (researchParent != 0
                && researchParent->hasChildren()) {
                for (int childIndex = 0; childIndex < researchParent->childCount(); ++childIndex) {
                    if (researchParent->childAt(childIndex)->research() == research) {
                        researchToRemove = researchParent->childAt(childIndex);
                        break;
                    }
                }
            }
            if (researchToRemove != 0) {
                removeItem(researchToRemove);
            }
        }
    }
}

void ResearchModel::researchDataChanged(const QModelIndex& _topLeft, const QModelIndex& _bottomRight)
{
    const int first = _topLeft.row();
    const int last = _bottomRight.row();

    //
    // Получим обновлённые элементы разработки
    //
    for (int row = first; row <= last; ++row) {
        QModelIndex itemIndex = m_researchData->index(row, 0);
        Domain::DomainObject* domainObject = m_researchData->itemForIndex(itemIndex);
        if (Research* research = dynamic_cast<Research*>(domainObject)) {
            //
            // Определим родителя
            //
            ResearchModelItem* researchParent = 0;
            if (research->parent() == 0) {
                if (research->type() == Research::Character) {
                    researchParent = m_charactersRoot;
                } else if (research->type() == Research::Location) {
                    researchParent = m_locationsRoot;
                } else {
                    researchParent = m_researchRoot;
                }
            } else {
                //
                // Ищем родителя по всем возможным местам
                //
                researchParent = findResearchModelItem(m_researchRoot, research->parent());
                if (researchParent == nullptr) {
                    researchParent = findResearchModelItem(m_charactersRoot, research->parent());
                }
                if (researchParent == nullptr) {
                    researchParent = findResearchModelItem(m_locationsRoot, research->parent());
                }
            }

            //
            // Обновим
            //
            ResearchModelItem* researchToUpdate = 0;
            if (researchParent->hasChildren()) {
                for (int childIndex = 0; childIndex < researchParent->childCount(); ++childIndex) {
                    if (researchParent->childAt(childIndex)->research() == research) {
                        researchToUpdate = researchParent->childAt(childIndex);
                        break;
                    }
                }
            }
            if (researchToUpdate != 0) {
                updateItem(researchToUpdate);
            }
        }
    }
}
