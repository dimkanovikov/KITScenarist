#include "ResearchModel.h"

#include "ResearchModelItem.h"

#include <Domain/Research.h>

using BusinessLogic::ResearchModel;
using BusinessLogic::ResearchModelItem;
using Domain::Research;

namespace {
	/**
	 * @brief Построить ветвь дерева разработки
	 */
	static void populateResearchTree(ResearchModelItem* _parent, Research* _research, const QMap<Research*, QList<Research*> >& _researchMap) {
		//
		// Добавляем очередной корневой элемент разработки в дерево
		//
		ResearchModelItem* researchItem = new ResearchModelItem(_research);
		_parent->appendItem(researchItem);

		//
		// Прорабатываем всех его детей
		//
		foreach (Research* researchChild, _researchMap.value(_research)) {
			populateResearchTree(researchItem, researchChild, _researchMap);
		}
	}
}


ResearchModel::ResearchModel(QObject* _parent = 0) :
	QAbstractItemModel(_parent),
	m_rootItem(new ResearchModelItem),
	m_researchRoot(0)
{
	//
	// Сценарий
	//
	ResearchModelItem* scenarioItem =
		new ResearchModelItem(
			new Research(
				Domain::Identifier(), 0, Research::Scenario, tr("Scenario"), QString::null, 0
			)
		);
	m_rootItem->appendItem(scenarioItem);
	//
	// Титульная страница
	//
	ResearchModelItem* titlePageItem =
		new ResearchModelItem(
			new Research(
				Domain::Identifier(), 0, Research::TitlePage, tr("Title Page"), QString::null, 0
			)
		);
	scenarioItem->appendItem(titlePageItem);
	//
	// Синопсис сценария
	//
	ResearchModelItem* synopsisItem =
		new ResearchModelItem(
			new Research(
				Domain::Identifier(), 0, Research::Synopsis, tr("Synopsis"), QString::null, 1
			)
		);
	scenarioItem->appendItem(synopsisItem);
	//
	// Корневая папка для разработки
	//
	m_researchRoot =
		new ResearchModelItem(
			new Research(
				Domain::Identifier(), 0, Research::ResearchRoot, tr("Research"), QString::null, 1
			)
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
	//
	// Формируем карту разработок
	// первыми в ней будут идти корневые элементы
	//
	QMap<Research*, QList<Research*> > researchMap;
	foreach (Domain::DomainObject* domainObject, _data->toList()) {
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
	// Обходим только корневые элементы дерева разработки
	//
	foreach (Research* research, researchMap.value(0)) {
		::populateResearchTree(m_researchRoot, research, researchMap);
	}
}

void ResearchModel::clear()
{
	//
	// Пересоздаём корень разработки
	//
	m_rootItem->removeItem(m_researchRoot);
	//
	m_researchRoot =
		new ResearchModelItem(
			new Research(
				Domain::Identifier(), 0, Research::ResearchRoot, tr("Research"), QString::null, 1
			)
		);
	m_rootItem->appendItem(m_researchRoot);
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

void ResearchModel::addItem(ResearchModelItem* _item, ResearchModelItem* _parentItem)
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
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;

//	ResearchModelItem* item = itemForIndex(_index);
//	if (item->type() == ResearchModelItem::Folder
//		|| item->type() == ResearchModelItem::SceneGroup
//		|| item->type() == ResearchModelItem::Scenario) {
//		flags |= Qt::ItemIsDropEnabled;
//	}

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

//bool ResearchModel::dropMimeData(
//		const QMimeData* _data, Qt::DropAction _action,
//		int _row, int _column, const QModelIndex& _parent)
//{
//	/*
//	 * Вставка данных в этом случае происходит напрямую в текст документа, а не в дерево,
//	 * само дерево просто перестраивается после всех манипуляций с текстовым редактором
//	 */

//	Q_UNUSED(_column);

//	//
//	// _row - индекс, куда вставлять, если в папку, то он равен -1 и если в самый низ списка, то он тоже равен -1
//	//

//	bool isDropSucceed = false;

//	if (_data != 0
//		&& _data->hasFormat(MIME_TYPE)) {

//		switch (_action) {
//			case Qt::IgnoreAction: {
//				isDropSucceed = true;
//				break;
//			}

//			case Qt::MoveAction:
//			case Qt::CopyAction: {

//				//
//				// Получим структурные элементы дерева, чтобы понять, куда вкладывать данные
//				//
//				// ... элемент, в который будут вкладываться данные
//				ResearchModelItem* parentItem = itemForIndex(_parent);
//				// ... элемент, перед которым будут вкладываться данные
//				ResearchModelItem* insertBeforeItem = parentItem->childAt(_row);

//				//
//				// Если производится перемещение данных
//				//
//				bool removeLastMime = false;
//				if (m_lastMime == _data) {
//					removeLastMime = true;
//				}

//				//
//				// Вставим данные
//				//
//				int insertPosition = m_xmlHandler->xmlToScenario(parentItem, insertBeforeItem, _data->data(MIME_TYPE), removeLastMime);
//				isDropSucceed = true;
//				emit mimeDropped(insertPosition);

//				break;
//			}

//			default: {
//				break;
//			}
//		}
//	}

//	return isDropSucceed;
//}

//QMimeData* ResearchModel::mimeData(const QModelIndexList& _indexes) const
//{
//	QMimeData* mimeData = new QMimeData;

//	if (!_indexes.isEmpty()) {
//		//
//		// Выделение может быть только последовательным, но нужно учесть ситуацию, когда в выделение
//		// попадает родительский элемент и не все его дочерние элементы, поэтому просто использовать
//		// последний элемент некорректно, нужно проверить, не входит ли его родитель в выделение
//		//

//		QModelIndexList correctedIndexes;
//		foreach (const QModelIndex& index, _indexes) {
//			if (!_indexes.contains(index.parent())) {
//				correctedIndexes.append(index);
//			}
//		}

//		//
//		// Для того, чтобы запретить разрывать папки проверяем выделены ли элементы одного уровня
//		//
//		bool itemsHaveSameParent = true;
//		{
//			const QModelIndex& genericParent = correctedIndexes.first().parent();
//			foreach (const QModelIndex& index, correctedIndexes) {
//				if (index.parent() != genericParent) {
//					itemsHaveSameParent = false;
//					break;
//				}
//			}
//		}

//		//
//		// Если выделены элементы одного уровня, то создаём майм-данные
//		//
//		if (itemsHaveSameParent) {
//			qSort(correctedIndexes);

//			QModelIndex fromIndex = correctedIndexes.first();
//			QModelIndex toIndex = correctedIndexes.last();

//			//
//			// Определяем элементы из которых будет состоять выделение
//			//
//			ResearchModelItem* fromItem = itemForIndex(fromIndex);
//			ResearchModelItem* toItem = itemForIndex(toIndex);

//			//
//			// Сформируем данные
//			//
//			mimeData->setData(
//						MIME_TYPE,
//						m_xmlHandler->scenarioToXml(fromItem, toItem).toUtf8());
//		}
//	}

//	m_lastMime = mimeData;

//	return mimeData;
//}

//QStringList ResearchModel::mimeTypes() const
//{
//	return QStringList() << MIME_TYPE;
//}

//Qt::DropActions ResearchModel::supportedDragActions() const
//{
//	return Qt::MoveAction;
//}

//Qt::DropActions ResearchModel::supportedDropActions() const
//{
//	return Qt::CopyAction | Qt::MoveAction;
//}

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
